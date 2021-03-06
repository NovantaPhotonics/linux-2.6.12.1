/*
 * Watchdog driver for Atmel AT91RM9200 (Thunder)
 *
 *  Copyright (C) 2003 SAN People (Pty) Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/config.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <asm/bitops.h> 
#include <asm/uaccess.h>
#include <linux/init.h>

#define WDT_DEFAULT_TIME 5	/* 5 seconds */
#define WDT_MAX_TIME 256	/* 256 seconds */

static int at91wdt_time = WDT_DEFAULT_TIME;
static unsigned long at91wdt_busy;

/* ......................................................................... */

/*
 * Disable the watchdog.
 */
static void at91_wdt_stop(void)
{
	AT91_SYS->ST_WDMR = AT91C_ST_EXTEN;
}

/*
 * Enable and reset the watchdog.
 */
static void at91_wdt_start(void)
{
	AT91_SYS->ST_WDMR = AT91C_ST_EXTEN | AT91C_ST_RSTEN | (((65536 * at91wdt_time) >> 8) & AT91C_ST_WDV);
	AT91_SYS->ST_CR = AT91C_ST_WDRST;
}

/* ......................................................................... */

/*
 * Watchdog device is opened, and watchdog starts running.
 */
static int at91_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(1, &at91wdt_busy))
		return -EBUSY;

	/*
	 * All counting occurs at SLOW_CLOCK / 128 = 0.256 Hz
	 *
	 * Since WDV is a 16-bit counter, the maximum period is
	 * 65536 / 0.256 = 256 seconds.
	 */

	at91_wdt_start();
	return 0;
}

/*
 * Close the watchdog device.
 * If CONFIG_WATCHDOG_NOWAYOUT is NOT defined then the watchdog is also
 *  disabled.
 */
static int at91_wdt_close(struct inode *inode, struct file *file)
{
#ifndef CONFIG_WATCHDOG_NOWAYOUT
	/* Disable the watchdog when file is closed */
	at91_wdt_stop();
#endif

	at91wdt_busy = 0;
	return 0;
}

/*
 * Handle commands from user-space.
 */
static int at91_wdt_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	unsigned int new_value;
	static struct watchdog_info info = {
		identity: "at91 watchdog",
		options:  WDIOF_SETTIMEOUT | WDIOF_MAGICCLOSE | WDIOF_KEEPALIVEPING,
	};

	switch(cmd) {
		case WDIOC_KEEPALIVE:
			AT91_SYS->ST_CR = AT91C_ST_WDRST;	/* Pat the watchdog */
			return 0;

		case WDIOC_GETSUPPORT:
			return copy_to_user((struct watchdog_info *)arg, &info, sizeof(info));

		case WDIOC_SETTIMEOUT:
			if (get_user(new_value, (int *)arg))
				return -EFAULT;
			if ((new_value <= 0) || (new_value > WDT_MAX_TIME))
				return -EINVAL;

			/* Restart watchdog with new time */
			at91wdt_time = new_value;
			at91_wdt_start();

			/* Return current value */
			return put_user(at91wdt_time, (int *)arg);

		case WDIOC_GETTIMEOUT:
			return put_user(at91wdt_time, (int *)arg);

		case WDIOC_GETSTATUS:
		case WDIOC_GETBOOTSTATUS:
			return put_user(0, (int *)arg);

		case WDIOC_SETOPTIONS:
			if (get_user(new_value, (int *)arg))
				return -EFAULT;
			if (new_value & WDIOS_DISABLECARD)
				at91_wdt_stop();
			if (new_value & WDIOS_ENABLECARD)
				at91_wdt_start();
			return 0;

		default:
			return -ENOIOCTLCMD;
	}
}

/*
 * Pat the watchdog whenever device is written to.
 */
static ssize_t at91_wdt_write(struct file *file, const char *data, size_t len, loff_t *ppos)
{
	AT91_SYS->ST_CR = AT91C_ST_WDRST;	/* Pat the watchdog */
	return len;
}

/* ......................................................................... */

static struct file_operations at91wdt_fops =
{
	.owner		= THIS_MODULE,
	.ioctl		= at91_wdt_ioctl,
	.open		= at91_wdt_open,
	.release	= at91_wdt_close,
	.write		= at91_wdt_write,
};

static struct miscdevice at91wdt_miscdev =
{
	.minor		= WATCHDOG_MINOR,
	.name		= "watchdog",
	.fops		= &at91wdt_fops,
};

static int __init at91_wdt_init(void)
{
	int res;

	res = misc_register(&at91wdt_miscdev);
	if (res)
		return res;

	printk("AT91 Watchdog Timer enabled (%d seconds)\n", WDT_DEFAULT_TIME);
	return 0;
}

static void __exit at91_wdt_exit(void)
{
	misc_deregister(&at91wdt_miscdev);
}

module_init(at91_wdt_init);
module_exit(at91_wdt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Victor");
MODULE_DESCRIPTION("Watchdog driver for Atmel AT91RM9200");
