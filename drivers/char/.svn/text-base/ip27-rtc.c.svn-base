/*
 *	Driver for the SGS-Thomson M48T35 Timekeeper RAM chip
 *
 *	Real Time Clock interface for Linux
 *
 *	TODO: Implement periodic interrupts.
 *
 *	Copyright (C) 2000 Silicon Graphics, Inc.
 *	Written by Ulf Carlsson (ulfc@engr.sgi.com)
 *
 *	Based on code written by Paul Gortmaker.
 *
 *	This driver allows use of the real time clock (built into
 *	nearly all computers) from user space. It exports the /dev/rtc
 *	interface supporting various ioctl() and also the /proc/rtc
 *	pseudo-file for status information.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 */

#define RTC_VERSION		"1.09b"

#include <linux/bcd.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/smp_lock.h>

#include <asm/m48t35.h>
//#include <asm/sn/ioc3.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
//#include <asm/sn/klconfig.h>
//#include <asm/sn/sn0/ip27.h>
//#include <asm/sn/sn0/hub.h>
//#include <asm/sn/sn_private.h>

#define RTC_MEM_RANGE 0x7EFB /*The RTC data starts at 0x7FF8 - the BOK byte - 8 bytes for bootcount*/
#define FLYER_RTC_BASE 0x40000000
void* rtc_addr_base = NULL;
static int rtc_ioctl(struct inode *inode, struct file *file,
		     unsigned int cmd, unsigned long arg);

static int rtc_read_proc(char *page, char **start, off_t off,
                         int count, int *eof, void *data);

static void get_rtc_time(struct rtc_time *rtc_tm);
static int  set_rtc_time(struct rtc_time *rtc_tm);

//static int  check_batt(void);
/*The reset_boot_count function clears the bootcount that is used in u-boot to decide to do an alternate boot
 *configuration on bootup.  Since this driver accesses the memory that it is stored in, it is also the 
 *responsibility of this driver to reset the boot count so that u-boot can tell that the kernel is booting properly.
 */
static int  reset_boot_count(void);

const char* wday[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
/*
 *	Bits in rtc_status. (6 bits of room for future expansion)
 */

#define RTC_IS_OPEN		0x01	/* means /dev/rtc is in use	*/
#define RTC_TIMER_ON		0x02	/* missed irq timer active	*/


#define BOOT_START 0x7fec
#define BOK_BYTE   0x7ff7
#define BOK_CHECK  0xaa

static unsigned char rtc_status;	/* bitmapped status byte.	*/
static unsigned long rtc_freq;	/* Current periodic IRQ rate	*/
static struct m48t35_rtc *rtc;
static int batt_good;
/*
 *	If this driver ever becomes modularised, it will be really nice
 *	to make the epoch retain its value across module reload...
 */
#define BOOT_MAGIC 0xB001C041
typedef struct 
{
    unsigned long count;
    unsigned long magic;
    unsigned long bok; //batt ok
}BootCount;

static unsigned long epoch = 1970;	/* year corresponding to 0x00	*/

static const unsigned char days_in_mo[] =
{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int rtc_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		     unsigned long arg)
{
	struct rtc_time wtime;

	switch (cmd) {
	case RTC_RD_TIME:	/* Read the time/date from RTC	*/
	{
		get_rtc_time(&wtime);
		return copy_to_user((void *)arg, &wtime, sizeof wtime) ? -EFAULT : 0;
	}
	case RTC_SET_TIME:	/* Set the RTC */
	{
		if (copy_from_user(&wtime, (struct rtc_time*)arg,
		       sizeof(struct rtc_time)))
		    return -EFAULT;
		return set_rtc_time(&wtime);
	}
	default:
		return -EINVAL;
	}
	
}

/*
 *	We enforce only one user at a time here with the open/close.
 *	Also clear the previous interrupt data on an open, and clean
 *	up things on a close.
 */

static int rtc_open(struct inode *inode, struct file *file)
{
	spin_lock_irq(&rtc_lock);

	if (rtc_status & RTC_IS_OPEN) {
		spin_unlock_irq(&rtc_lock);
		return -EBUSY;
	}

	rtc_status |= RTC_IS_OPEN;
	spin_unlock_irq(&rtc_lock);

	return 0;
}

static int rtc_release(struct inode *inode, struct file *file)
{
	/*
	 * Turn off all interrupts once the device is no longer
	 * in use, and clear the data.
	 */

	spin_lock_irq(&rtc_lock);
	rtc_status &= ~RTC_IS_OPEN;
	spin_unlock_irq(&rtc_lock);

	return 0;
}

static ssize_t rtc_read(struct file* file, char* buf, size_t count, loff_t *offset)
{
    //printk(KERN_ERR "offset:%d, count:%d not to exceed %d\n",(int)*offset,(int)count,(int)RTC_MEM_RANGE);
    if ( *offset + count > RTC_MEM_RANGE)
	return -EFAULT;
    
    if (copy_to_user(buf,(char*)&rtc->pad[*offset],count))
	return -EFAULT;
    *offset +=count;
    return count;
}

static ssize_t rtc_write(struct file* file, const char* buf, size_t count, loff_t *offset)
{
    if ( *offset + count > RTC_MEM_RANGE)
	return -EFAULT;
    
    if (copy_from_user((char*)&rtc->pad[*offset],buf,count))
	return -EFAULT;
    *offset +=count;
    return count;
}

static loff_t rtc_seek(struct file *file, loff_t offset, int origin)
{
    long long retval;
    
    switch (origin) 
    {
        case 2://SEEK_END
	    offset += RTC_MEM_RANGE;
	    break;
	case 1://SEEK_CUR
	    offset += file->f_pos;
	    break;
	case 0://SEEK_SET
	    break;
    }
    retval = -EINVAL;
    if (offset >= 0) 
    {
	if (offset != file->f_pos) 
	{
	    file->f_pos = offset;
	    file->f_version = 0;
	}
	retval = offset;
    }
    return retval;
}

/*
 *	The various file operations we support.
 */

static struct file_operations rtc_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= rtc_ioctl,
	.open		= rtc_open,
	.release	= rtc_release,
	.read		= rtc_read,
	.write		= rtc_write,
	.llseek		= rtc_seek,
};

static struct miscdevice rtc_dev=
{
	RTC_MINOR,
	"rtc",
	&rtc_fops
};

static int __init rtc_init(void)
{
    if (!rtc_addr_base)
    {
	AT91PS_SYS pSys = (AT91PS_SYS) AT91C_VA_BASE_SYS;
	pSys->EBI_CSA &= ~0x08;//ChipSelect 3
	pSys->EBI_SMC2_CSR[3]=0x11004485;
	rtc_addr_base = (void*) ioremap(FLYER_RTC_BASE, SZ_16K*2);
	printk(KERN_DEBUG "rtc_addr_base=0x%x\n",(unsigned int)rtc_addr_base);
    }
    
    rtc = (struct m48t35_rtc *)rtc_addr_base;
    
    printk(KERN_INFO "Real Time Clock Driver v%s\n", RTC_VERSION);
    if (misc_register(&rtc_dev)) {
	printk(KERN_ERR "rtc: cannot register misc device.\n");
	return -ENODEV;
    }
    if (!create_proc_read_entry("driver/rtc", 0, NULL, rtc_read_proc, NULL)) {
	printk(KERN_ERR "rtc: cannot create /proc/rtc.\n");
	misc_deregister(&rtc_dev);
	return -ENOENT;
    }

    rtc_freq = 1024;
    
    //u-boot is the first to write, so the batt check is done there, then saved in the bootcount struct.
    //batt_good = check_batt();
    reset_boot_count();
    return 0;
}

static void __exit rtc_exit (void)
{
    /* interrupts and timer disabled at this point by rtc_release */
    
    remove_proc_entry ("rtc", NULL);
    misc_deregister(&rtc_dev);
    
    if (rtc_addr_base)
    {
	iounmap(rtc_addr_base);
	printk(KERN_DEBUG "Releasing rtc_addr_base @ 0x%x\n",(unsigned int)rtc_addr_base);
	rtc_addr_base = NULL;
    }
}

module_init(rtc_init);
module_exit(rtc_exit);
MODULE_LICENSE("GPL");
/*
 *	Info exported via "/proc/rtc".
 */

static int rtc_get_status(char *buf)
{
	char *p;
	struct rtc_time tm;

	/*
	 * Just emulate the standard /proc/rtc
	 */

	p = buf;

	get_rtc_time(&tm);

	/*
	 * There is no way to tell if the luser has the RTC set for local
	 * time or for Universal Standard Time (GMT). Probably local though.
	 */
	p += sprintf(p,
		     "rtc_time\t: %02d:%02d:%02d\n"
		     "rtc_date\t: %04d-%02d-%02d\n"
	 	     "rtc_weekday\t: %s\n"
		     "24hr\t\t: yes\n"
		     "batt_status\t: %s\n",
		     tm.tm_hour, tm.tm_min, tm.tm_sec,
		     tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, wday[tm.tm_wday],
		     batt_good ? "Okay" : "Failing");

	return  p - buf;
}

static int rtc_read_proc(char *page, char **start, off_t off,
                                 int count, int *eof, void *data)
{
        int len = rtc_get_status(page);
        if (len <= off+count) *eof = 1;
        *start = page + off;
        len -= off;
        if (len>count) len = count;
        if (len<0) len = 0;
        return len;
}

static void get_rtc_time(struct rtc_time *rtc_tm)
{
	/*
	 * Do we need to wait for the last update to finish?
	 */

	/*
	 * Only the values that we read from the RTC are set. We leave
	 * tm_wday, tm_yday and tm_isdst untouched. Even though the
	 * RTC has RTC_DAY_OF_WEEK, we ignore it, as it is only updated
	 * by the RTC when initially set to a non-zero value.
	 */
	spin_lock_irq(&rtc_lock);
	rtc->control |= M48T35_RTC_READ;
	rtc_tm->tm_sec = rtc->sec;
	rtc_tm->tm_min = rtc->min;
	rtc_tm->tm_hour = rtc->hour;
	rtc_tm->tm_wday = rtc->day;
	rtc_tm->tm_mday = rtc->date;
	rtc_tm->tm_mon = rtc->month;
	rtc_tm->tm_year = rtc->year;
	rtc->control &= ~M48T35_RTC_READ;	
	spin_unlock_irq(&rtc_lock);

	rtc_tm->tm_sec = BCD2BIN(rtc_tm->tm_sec);
	rtc_tm->tm_min = BCD2BIN(rtc_tm->tm_min);
	rtc_tm->tm_hour = BCD2BIN(rtc_tm->tm_hour);
	rtc_tm->tm_mday = BCD2BIN(rtc_tm->tm_mday);
	rtc_tm->tm_mon = BCD2BIN(rtc_tm->tm_mon);
	rtc_tm->tm_year = BCD2BIN(rtc_tm->tm_year);
	/*
	 * Account for differences between how the RTC uses the values
	 * and how they are defined in a struct rtc_time;
	 */
	if ((rtc_tm->tm_year += (epoch - 1900)) <= 69)
		rtc_tm->tm_year += 100;
	rtc_tm->tm_mon--;
	rtc_tm->tm_wday--;
}

static int set_rtc_time(struct rtc_time *rtc_tm)
{
    unsigned char mon, day, hrs, min, sec, leap_yr, wday;
    unsigned int yrs;
    
    if (!capable(CAP_SYS_TIME))
	return -EACCES;
    
    yrs = rtc_tm->tm_year + 1900;
    mon = rtc_tm->tm_mon + 1;   //(January)0-11
    day = rtc_tm->tm_mday;
    hrs = rtc_tm->tm_hour;
    min = rtc_tm->tm_min;
    sec = rtc_tm->tm_sec;
    wday = rtc_tm->tm_wday + 1;
    if (yrs < 1970)
	return -EINVAL;
    
    leap_yr = ((!(yrs % 4) && (yrs % 100)) || !(yrs % 400));
    
    if ((mon > 12) || (day == 0))
	return -EINVAL;
    
    if (day > (days_in_mo[mon] + ((mon == 2) && leap_yr)))
	return -EINVAL;
    
    if ((hrs >= 24) || (min >= 60) || (sec >= 60))
	return -EINVAL;
    
    if ((yrs -= epoch) > 255)    /* They are unsigned */
	return -EINVAL;
    
    if (yrs > 169)
	return -EINVAL;
    
    if (yrs >= 100)
	yrs -= 100;
    
    if (wday < 1 || wday > 7)
	return -EINVAL;
    
    sec = BIN2BCD(sec);
    min = BIN2BCD(min);
    hrs = BIN2BCD(hrs);
    day = BIN2BCD(day);
    mon = BIN2BCD(mon);
    yrs = BIN2BCD(yrs);
    wday = BIN2BCD(wday);
    
    spin_lock_irq(&rtc_lock);
    if (rtc->sec & M48T35_RTC_STOPPED)/*clear the stop bit*/
	rtc->sec &= ~M48T35_RTC_STOPPED;
    rtc->control |= M48T35_RTC_SET;
    rtc->year = yrs;
    rtc->month = mon;
    rtc->date = day;
    rtc->day = wday;
    rtc->hour = hrs;
    rtc->min = min;
    rtc->sec = sec;
    rtc->control &= ~M48T35_RTC_SET;
    rtc->pad[BOK_BYTE] = BOK_CHECK;
    spin_unlock_irq(&rtc_lock);
    
    return 0;
    
}

/*************************
  *This algorithm checks the battery.  If the battery is failing, the first write attempt on powerup is blocked
  *This checks for that condition.
  ***********************
static int check_batt(void)
{
    int ret = 0;
    unsigned char test,testcomp;
    test = rtc->pad[BOK_BYTE];
    //write in the complement;
    rtc->pad[BOK_BYTE] = ~test;
    testcomp = rtc->pad[BOK_BYTE];
    if ( (testcomp ^ test) == 0xff)
	ret = 1;
    
    rtc->pad[BOK_BYTE] = test;
    return ret;
}*/

static int  reset_boot_count(void)
{
    BootCount* pBoot = (BootCount*)&rtc->pad[BOOT_START];
    if (pBoot->magic != BOOT_MAGIC)
    {
	printk(KERN_INFO "BootMagic Number incorrect, got:0x%08lx, expected:0x%08x\n",pBoot->magic,BOOT_MAGIC);
	pBoot->magic = BOOT_MAGIC;
    }
    batt_good = pBoot->bok;//set the batt_good to what u-boot detected.
    
    if (pBoot->count > 1)/* print out a message if it is greater than one*/
    {
	printk(KERN_INFO "BootCount is:0x%lx, setting to zero\n",pBoot->count);
    }
    /* Do the actual reset of the boot count, set it to zero*/
    pBoot->count = 0;
    return 0;
}
