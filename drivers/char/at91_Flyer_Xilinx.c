/***************************************************************************
 *   Copyright (C) 2005 by J. Warren                                       *
 *   jeffw@synrad.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/arch/at91_Flyer_Xilinx.h>
#include <asm/arch/AT91RM9200_PIO.h>
#include <asm/arch/AT91RM9200_TC.h>
#include <asm/arch/hardware.h>

#define BUFSIZE 4000
#define XIL_DONE_DELAY 30

#ifdef DEBUG
#define MSG(string, args...) printk(KERN_DEBUG "flyer_xil:" string, ##args)
#else
#define MSG(string, args...)
#endif

#define KEY_INTERRUPT		0x1
#define IO_INTERRUPT		0x2
#define TRACK_INTERRUPT		0x4
#define TEMP_INTERRUPTS  	0x78
#define TIMEOUT_INTERRUPT	0x80

#define TESTMARK_KEY 1

#define XILINX_ALIVE 1
#define XILINX_DEAD 0

#define XILINX_INIT AT91C_PIO_PC0
#define XILINX_DATA_IN AT91C_PIO_PC1
#define XILINX_DONE AT91C_PIO_PC2
#define XILINX_PROGRAM AT91C_PIO_PC3
#define XILINX_CLOCK AT91C_PIO_PC4

#define XILINX_SLOW_CLOCK AT91C_PA24_PCK1
#define XILINX_FAST_CLOCK AT91C_PB27_PCK0

#define XILINX_ALL (XILINX_INIT | XILINX_DATA_IN | XILINX_DONE | XILINX_PROGRAM | XILINX_CLOCK)

#define FLYER_XILINX_BASE 0x30000000

/*LED defines*/
#define USB_RED_LINE AT91C_PIO_PD21
#define USB_GREEN_LINE AT91C_PIO_PD20
#define STAT_RED_LINE AT91C_PIO_PB9
#define STAT_GREEN_LINE AT91C_PIO_PB8
#define MAX_FREQ 10
/*END LED defines*/

char* readBuf;
char* writeBuf;
char bitmask[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
void* xil_addr_base = NULL;
DECLARE_WAIT_QUEUE_HEAD(io_queue);
DECLARE_WAIT_QUEUE_HEAD(track_queue);

static irqreturn_t at91_flyer_xil_interrupt(int irq, void *dev_id, struct pt_regs *regs);
static irqreturn_t at91_flyer_xil_tc_interrupt(int irq, void *dev_id, struct pt_regs *regs);
    
static AT91PS_PIO pPIOB = (AT91PS_PIO)AT91_IO_P2V(AT91C_BASE_PIOB);
static AT91PS_PIO pPIOC = (AT91PS_PIO)AT91_IO_P2V(AT91C_BASE_PIOC);
static AT91PS_PIO pPIOD = (AT91PS_PIO)AT91_IO_P2V(AT91C_BASE_PIOD);
static AT91PS_TC ptc4 = (AT91PS_TC)AT91_IO_P2V(AT91C_BASE_TC4);

int src = 0,sra = 0,srb = 0,bLEDChanged = 0;
int sfreq = 0,sfreqnew = 0;

const int clock = 32768;//slow clock

void set_usb_led(int state);
void set_status_led(int state);
void setLED(int freq,int red,int green);
int checkSwitch(int switchNum);
static const int Xilinx_Size = 78756;
int tracking=0;
int enable_part_interrupt = 0;
int marking_testmark = 0;
int waitIOSuccess = 0;
pid_t main_pid = 0;
int temp_status = 0;
unsigned short enc_cfg = 0;
//unsigned short enable_main_interrupts = 0;
// 05-Aug-2009 sbs 2.57: Allow Mark Aborts from a predetermined user-enabled input (Input 7).
// Ran out of signals so Abort and IO Change share a signal. The interrupt type determines
// What the signal handler should do.
unsigned long interrupt_type = 0;

int flyer_xil_alive(void)
{
    if (pPIOC->PIO_PDSR & XILINX_DONE)
	return XILINX_ALIVE; //it has been programmed
    
    return XILINX_DEAD;//return the status of the Xilinx
}

int flyer_xil_program_init(void)
{
    pPIOC->PIO_SODR = XILINX_PROGRAM;
    udelay(50);
    pPIOC->PIO_CODR = XILINX_PROGRAM;
    udelay(100); //wait 100 usecs;
    pPIOC->PIO_SODR = XILINX_PROGRAM;
    
    while(!pPIOC->PIO_ODSR & XILINX_INIT);//wait for init to go high
    
    udelay(10); //delay 10 usecs
    
    return 1;
}

int flyer_xil_program_done(void)
{
    int start = jiffies;
    while ( (!pPIOC->PIO_PDSR & XILINX_DONE) && ((jiffies - start) < XIL_DONE_DELAY) );
    
    if ( (jiffies - start) >= XIL_DONE_DELAY)
	return 0;//timed out
    
    return 1;
}

// Function that actually does the clocking out of the data through the GPIO to the Xilinx
void flyer_xil_send(int size, char* buf)
{
    int i=0, j=0;
    for (i=0;i<size;i++)
    {
	for (j=0;j<8;j++)
	{
	    //set clock low
	    pPIOC->PIO_CODR = XILINX_CLOCK;	    
	    if (buf[i] & bitmask[j])
		pPIOC->PIO_SODR = XILINX_DATA_IN;//clock in a 1
	    else
		pPIOC->PIO_CODR = XILINX_DATA_IN;//clock in a 0
	    pPIOC->PIO_SODR = XILINX_CLOCK;
	}	
    }
    
}

void set_usb_led(int state)
{        
    if (state & SOLID_GREEN)
	pPIOD->PIO_SODR = USB_GREEN_LINE;
    else
	pPIOD->PIO_CODR = USB_GREEN_LINE;
    if (state & SOLID_RED)
	pPIOD->PIO_SODR = USB_RED_LINE;
    else
	pPIOD->PIO_CODR = USB_RED_LINE;
}

void set_status_led(int state)
{
    setLED((state & FREQ_MASK) >> 2,state & SOLID_RED,state & SOLID_GREEN);
}

//if freq is <= 0, light is solid.
//if both colors are 0 and freq is <=0 turns off LED
void setLED(int freq,int red,int green)
{
    int rc,ra,rb;
    
    //calculate ra,rb, and rc
    if (freq <=0)
	freq = 0;
    
    if (freq == 0 || freq >= MAX_FREQ)
	rc = clock/MAX_FREQ;
    else
	rc = clock/freq;
    
    if (rc > 0xffff)
	rc = 0xffff;
    if (green)
    {
	ra = freq ? rc/2 : rc;
    }
    else
    {
	ra = 1;
    }
    if (red)
    {
	rb = freq ? rc/2 : 1;
    }
    else
    {
	rb = rc;
    }
    
    if (red && green && (freq <= 0))
    {
	//if both are set to solid, run red only...
	ra = 1;
    }
    
    /*change the timer counter setup when the next interrupt fires.
     *Use of the interrupt and all of this is done so that no matter what mark is done,
     *a minimum of one cycle is completed so that the blinking always occurs.  Without this,
     *if the status blinks at 5Hz for marking and you are marking parts at >10Hz, the light
     *turns solid.  Using the interrupt allows the light to always complete at least one cycle.
     */
    //printk(KERN_ERR "rc:%d, src:%d, rb:%d, srb:%d, ra:%d, sra:%d, freq:%d, sfreq:%d\n",
    //	   rc,src,rb,srb,ra,sra,freq,sfreq);
    if (rc !=src || rb != srb || ra != sra || sfreq != freq)
    {
	sfreqnew = freq; 
	src = rc;
	srb = rb;
	sra = ra;
	
	if (!sfreq)//currently solid, change it here...
	{
	    ptc4->TC_CCR = AT91C_TC_CLKDIS; /*Disable the clock counter*/  
	
	    ptc4->TC_CMR = 
		AT91C_TC_BSWTRG_CLEAR   | //clear b so that it is the inverse of a
		AT91C_TC_BCPC_CLEAR		| //clear b at the end of the cycle
		AT91C_TC_BCPB_SET		| //set b at rb
		AT91C_TC_ASWTRG_SET		| 
		AT91C_TC_ACPC_SET		|
		AT91C_TC_ACPA_CLEAR		|
		AT91C_TC_WAVE			|
		AT91C_TC_CPCTRG			|
		AT91C_TC_EEVT_RISING 	|
		AT91C_TC_TIMER_DIV5_CLOCK ;
	    
	    ptc4->TC_RC = src;
	    ptc4->TC_RB = srb;
	    ptc4->TC_RA = sra;
	    if (sfreqnew)
	    {
		ptc4->TC_IER = 0x10;
	    }
	    else
	    {
		ptc4->TC_IDR = 0x10;
	    }
		
	    
	    ptc4->TC_CCR = AT91C_TC_CLKEN;
	    ptc4->TC_CCR = AT91C_TC_SWTRG; 
	    sfreq = freq;
	}
	else
	    bLEDChanged = 1; //indicate a change...
    
    }
    
    
}

static irqreturn_t at91_flyer_xil_tc_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{    
    uint status = 0;
    status = ptc4->TC_SR;

    if (bLEDChanged)
    {
	//check to see if we are actually changing, as it may be the same as before, 
	//just that there was an intermediate change...
	if (ptc4->TC_RC == src && ptc4->TC_RB == srb && ptc4->TC_RA == sra)
	{
	    bLEDChanged = 0;	    
	    return IRQ_HANDLED;
	}

	sfreq = sfreqnew;
	ptc4->TC_CCR = AT91C_TC_CLKDIS; /*Disable the clock counter*/
	ptc4->TC_CMR = 
	AT91C_TC_BSWTRG_CLEAR   | //clear b so that it is the inverse of a
	AT91C_TC_BCPC_CLEAR		| //clear b at the end of the cycle
	AT91C_TC_BCPB_SET		| //set b at rb
	AT91C_TC_ASWTRG_SET		| 
	AT91C_TC_ACPC_SET		|
	AT91C_TC_ACPA_CLEAR		|
	AT91C_TC_WAVE			|
	AT91C_TC_CPCTRG			|
	AT91C_TC_EEVT_RISING 	|
	AT91C_TC_TIMER_DIV5_CLOCK ;
    
	ptc4->TC_RC = src;
	ptc4->TC_RB = srb;
	ptc4->TC_RA = sra;
    
	if (sfreq)
	{
	    ptc4->TC_IER = 0x10;
	}
	else
	{
	    ptc4->TC_IDR = 0x10;
	}
	
	ptc4->TC_CCR = AT91C_TC_CLKEN;
	ptc4->TC_CCR = AT91C_TC_SWTRG; 
	bLEDChanged = 0;
    }    
    return IRQ_HANDLED;
}

int checkSwitch(int switchNum)
{
    int ret;
    ulong result;
    AT91_SYS->PMC_PCER = 1 << AT91C_ID_PIOD;
    pPIOD->PIO_PER = 7 << 16;//pins 16(switch 2),17(switch 3), and 18(switch 4)
    pPIOD->PIO_ODR = 7 << 16;
    result = pPIOD->PIO_PDSR;
    switch (switchNum)
    {
	case 2:
	ret = (result & (1 << 16)) ? 0:1 ;
	break;
	case 3:
	ret = (result & (1 << 17)) ? 0:1 ;
	break;
	case 4:
	ret = (result & (1 << 18)) ? 0:1 ;
	break;
	default:
	ret = -1;
	break;
    }
    return ret;
}



static ssize_t flyer_xil_read(struct file* file, char* buf, size_t count, loff_t *offset)
{
    int readCount = 1;//(count > BUFSIZE ? BUFSIZE : count);
   
    /* Actually read something*/
    MSG("Read cmd, %s, count: %d\n",readBuf,readCount);
    readBuf[0] = flyer_xil_alive();
    copy_to_user(buf,readBuf,readCount);
    main_pid = current->pid;
    return readCount;
}

static ssize_t flyer_xil_write(struct file* file, const char* buf, size_t count, loff_t *offset)
{
    int writeCount = 0;
    int MasterCount = count;
    int BytesTxferd = 0;
    char* bufPtr = (char*)buf;
    
    if (count != Xilinx_Size)
    {
	MSG("Not the right size, got %d, should have %d\n",count,Xilinx_Size);
	return 0;
    }
    main_pid = current->pid;
    /* Actually write something*/
    MSG("Write cmd, %s, count: %d\n",writeBuf,MasterCount);
    flyer_xil_program_init();
    while (MasterCount)
    {
	writeCount = (MasterCount > BUFSIZE ? BUFSIZE : MasterCount);
	//copy to buffer
	copy_from_user(writeBuf,bufPtr,writeCount);
	// write it out...
	flyer_xil_send(writeCount,writeBuf);
	//increment the vars
	BytesTxferd += writeCount;
	MasterCount -= writeCount;
	bufPtr = (char*)((unsigned int)bufPtr + writeCount);
			
    }   
    //set clock low
    pPIOC->PIO_CODR = XILINX_CLOCK;
    if (flyer_xil_program_done()) //check to see it was programmed
	return BytesTxferd;
    return 0;    
}

static int flyer_xil_open(struct inode* inode, struct file* file)
{
    //int iDevCurrent = iminor(inode);
    MSG("Module at91_flyer_xil open, iMinor = %d\n",iDevCurrent );
    //printk("SBS Module at91_flyer_xil open\n");  
    
    return 0;
}

static int flyer_xil_release(struct inode* inode, struct file* file)
{
    MSG("Module at91_flyer_xil release\n" );
    
    return 0;
}

static int flyer_xil_ioctl(struct inode* inode, struct file* file, unsigned int cmd, unsigned long arg)
{
    AT91PS_PIO pPIOA = (AT91PS_PIO)AT91_IO_P2V(AT91C_BASE_PIOA);
    AT91PS_PIO pPIOB = (AT91PS_PIO)AT91_IO_P2V(AT91C_BASE_PIOB);
    
    /* Make sure the command belongs to us*/
    if (_IOC_TYPE(cmd) != XILINX_CONFIG_IOCTL_BASE)
	return -ENOTTY;
    int ret_val = 0;
    unsigned char io_stat;
    switch(cmd )
    {
    case XIL_CHECK_STATUS:
	MSG("at91_flyer_xil Check Status\n");
	return flyer_xil_alive();
	
    case XIL_START_CLOCKS:
	if (arg)
	{
	    //enable the PIO for the clocks
	    pPIOA->PIO_PDR = XILINX_SLOW_CLOCK;
	    pPIOA->PIO_BSR = XILINX_SLOW_CLOCK;
	    pPIOB->PIO_PDR = XILINX_FAST_CLOCK;
	    pPIOB->PIO_ASR = XILINX_FAST_CLOCK;
	    
	    AT91_SYS->PMC_SCER = 0x300;//enable PCKO and PCK1
	    AT91_SYS->PMC_IDR = 0xf00;//disable interrupts, probably not needed
	    AT91_SYS->PMC_PCKR[0] = 0x07; //PLLB/2 for 48 MHz
	    AT91_SYS->PMC_PCKR[1] = 0x11; //MainClock/16 for 1 MHz 
	    
	}
	else
	{
	    //disable the PIO for the clocks
	    pPIOA->PIO_PER = XILINX_SLOW_CLOCK;
	    pPIOA->PIO_CODR = XILINX_SLOW_CLOCK;
	    pPIOA->PIO_OER = XILINX_SLOW_CLOCK;
	    
	    pPIOB->PIO_PER = XILINX_FAST_CLOCK;
	    pPIOB->PIO_CODR = XILINX_FAST_CLOCK;
	    pPIOB->PIO_OER = XILINX_FAST_CLOCK;
	}
	return arg;
	
    case XIL_ENC_PULSE_SET:
	{
	    AT91S_TC* ptc1 = (AT91S_TC*)AT91_IO_P2V(AT91C_BASE_TC1);
	    /*Select the B register for 19*/
	    AT91_SYS->PIOA_BSR = AT91C_PA19_TIOA1 ;
	    /*Turn on the peripheral access to TIOA1 */
	    AT91_SYS->PIOA_PDR = (AT91C_PA19_TIOA1 );
	    
	    ptc1->TC_CCR = AT91C_TC_CLKDIS; /*Disable the clock counter*/
	    
	    if (arg)
	    {
		if (arg < RC_PULSE_MIN)
		    arg = RC_PULSE_MIN;
		AT91_SYS->PMC_PCER = 1 << AT91C_ID_TC1;	/* enable peripheral clock for TC1*/
		ptc1->TC_CMR = 
			AT91C_TC_ASWTRG_CLEAR		| 
			AT91C_TC_ACPC_CLEAR		|
			AT91C_TC_ACPA_SET		|
			AT91C_TC_WAVE			|
			AT91C_TC_CPCTRG			|
			AT91C_TC_EEVT_RISING 	|
			AT91C_TC_TIMER_DIV3_CLOCK ;
		ptc1->TC_RC = arg;
		ptc1->TC_RA = ptc1->TC_RB = arg/2;
		
		ptc1->TC_CCR = AT91C_TC_CLKEN;
		ptc1->TC_CCR = AT91C_TC_SWTRG; 
	    }
	    else
	    {
		AT91_SYS->PMC_PCDR = 1 << AT91C_ID_TC1;	/* disable peripheral clock for TC1*/
	    }
	    
	}
	return arg;
    
    case XIL_WAKE_IO_SLEEPERS: //called by the thread that accepts data from WinMark on an abort.
	wake_up_interruptible(&io_queue);
	wake_up_interruptible(&track_queue);
	
	break;
	
    case XIL_TESTMARK_DONE: //called so that the driver knows the testmark is complete..
	marking_testmark = 0;
	break;
    case XIL_CHECK_SWITCH:
	return checkSwitch(arg);
	break;
	
    
    //Here are the Read commands for the Xilinx...
    case XIL_GET_IO:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_IO_OFFSET);   
	}
	else
	    arg = -1;
	return arg;
    
    case XIL_SYSTEM_STATUS:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_STATUS_OFFSET);   
	}
	else
	    arg = -1;
	return arg;	
	
    case XIL_GET_VERSION:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_VERSION_OFFSET);   
	}
	else
	    arg = -1;
	return arg;
	
    case XIL_GET_KEYPAD:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_KEYPAD_OFFSET);   
	}
	else
	    arg = -1;
	return arg;
	
    case XIL_GET_TESTMARK:
	return marking_testmark;
    
    case XIL_GET_WAIT_DIGITAL:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_WAIT_DIGITAL_OFFSET); 
	}
	else
	    arg = -1;
	return arg;
    case XIL_GET_TEMP_STATUS://Get the temp status of the last interrupt
	return temp_status;
    case XIL_GET_SWITCHES:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_SWITCHES); 
	}
	else
	    arg = -1;
	return arg;
    //Here are the Write commands for the Xilinx...
    case XIL_SET_IO:
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_IO_OFFSET) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
	
    case XIL_DEBUG_SET:
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_DEBUG_SET_OFFSET) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
    case XIL_DEBUG_CLEAR:
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_DEBUG_CLR_OFFSET) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
    case XIL_ENCODER_CFG:
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_ENC_CFG_OFFSET) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
    case XIL_ENCODER_CFG_STR:
	if (xil_addr_base)
	{
	    // 07-Nov-2011 sbs 3.11 Issue: Make internal part triggering work in Banner marking 
	    // Store the enc_cfg so that it can be sent with wait digital below
	    enc_cfg = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
    case XIL_SET_PART_PITCH:
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_PPC_OFFSET) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
// 01-Nov-2011 sbs 3.11 Issue: Banner marking with user part pitch enabled 
    case XIL_SET_BANNER_PITCH:
	if (xil_addr_base)
	{
	    *((unsigned short*)xil_addr_base + XIL_BANNER_PITCH) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
    case XIL_SET_BANNER_COUNT:
	if (xil_addr_base)
	{
	    *((unsigned short*)xil_addr_base + XIL_BANNER_COUNT) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
	
    case XIL_SET_WAIT_DIGITAL://This will cause the calling process to be placed on the IO wait queue...
	if (xil_addr_base)
	{
	    waitIOSuccess = 0;
	    waitStruct Wait = * (waitStruct*)(arg);
	    if (Wait.iTimeout == 0)//shouldn't happen, but handle it just as well...
	    {
		ret_val = (unsigned long)*((unsigned short*)xil_addr_base + XIL_IO_OFFSET);
	    }
	    else if (Wait.iTimeout > 0)
	    {
		int iTimeout = Wait.iTimeout -1;//for Xilinx, goes from 0-999999 for 1-1000000
		unsigned short usTimeLow,usTimeHigh; //low has 16, high has four...
		usTimeLow = (unsigned short)(iTimeout & 0xffff);
		usTimeHigh = (unsigned short)( (iTimeout & 0xf0000) >> 16);
		*((unsigned short*)xil_addr_base + XIL_TIMEOUT_LSB) = usTimeLow; 
		*((unsigned short*)xil_addr_base + XIL_TIMEOUT_MSB) = usTimeHigh; 
	    }
	    
	    wait_queue_t wait;
	    init_waitqueue_entry(&wait,current);
	    add_wait_queue(&io_queue,&wait);
	    set_current_state(TASK_INTERRUPTIBLE);
	    if (enable_part_interrupt)//should only occur once at the start of the mark session.
	    {
		enable_part_interrupt = 0;
		AT91_SYS->PIOB_CODR = AT91C_PIO_PB24;//generate interrupt.pull this down so that the DSP is on the same part sense
		AT91_SYS->PIOB_SODR = AT91C_PIO_PB24;//set it back.
		*((unsigned short*)xil_addr_base + XIL_PARTSENSE_ENABLE) = (unsigned short)1;
		*((unsigned short*)xil_addr_base + XIL_WAIT_DIGITAL_OFFSET) = (unsigned short)Wait.dwState;
		//udelay(200);
		// 07-Nov-2011 sbs 3.11 Issue: Make internal part triggering work in Banner marking 
	        // Send the encoder cfg now that Flyer is ready to receive interrupts from the Xilinx 
		*((unsigned short*)xil_addr_base + XIL_ENC_CFG_OFFSET) = enc_cfg;    
	    }
	    else		
		*((unsigned short*)xil_addr_base + XIL_WAIT_DIGITAL_OFFSET) = (unsigned short)Wait.dwState; 
	    if (Wait.dwState & 0xff00)//there is a bit in the mask set, or tracking bit set
	    {
		
		schedule();//put the calling process to sleep
	    }
	    set_current_state(TASK_RUNNING);
	    remove_wait_queue(&io_queue,&wait);
	    if (waitIOSuccess)
		return 0;
	    else
		return -1;
	}
	else
	    ret_val = -1;
	return ret_val;
    case XIL_TRACK_WAIT://This will cause the calling process to be placed on the track wait queue...
	if (xil_addr_base)
	{
	    
	    wait_queue_t wait;
	    //printk("current pid = %d\n", current->pid);
	    init_waitqueue_entry(&wait,current);
	    add_wait_queue(&track_queue,&wait);
	    set_current_state(TASK_INTERRUPTIBLE);
	    //send the interrupt to the DSP telling it we are ready to handle the tracking interrupt.
	    *((unsigned short*)xil_addr_base + XIL_DEBUG_SET_OFFSET) = 0x10;   
	    AT91_SYS->PIOB_CODR = AT91C_PIO_PB24;//generate interrupt
	    AT91_SYS->PIOB_SODR = AT91C_PIO_PB24;//set it back.
	    *((unsigned short*)xil_addr_base + XIL_DEBUG_CLR_OFFSET) = 0x10; 
	    
	    schedule();//put the calling process to sleep
	    set_current_state(TASK_RUNNING);
	    remove_wait_queue(&track_queue,&wait);
	}
	else
	    arg = -1;
	return arg;
    case XIL_LED_SET_USB:
	set_usb_led(arg);
	return arg;
    case XIL_LED_SET_STATUS:
        set_status_led(arg);
	return arg;
    case XIL_SET_SWITCHES:
	//If this is disabled, the very next XIL_SET_WAIT_DIGITAL will then call it again to enable.
	//That way the Xilinx part sense is enable right when we look for the first part.
	if (xil_addr_base)
	{
	  // 05-Aug-2009 sbs 2.57: Allow Mark Aborts from a predetermined user-enabled input (Input 7).
	 // Read XIL switches (which includes enabling/disabling ABORT IO in order to clear the last
	 // interrupt prior to enabling/disabling Abort IO
	    
	    io_stat = *((unsigned char*)xil_addr_base + XIL_SWITCHES);
	 *((unsigned short*)xil_addr_base + XIL_SWITCHES) = (unsigned short)arg;
	
	}
	else
	    arg = -1;
	return arg;
    case XIL_SET_PART_INTERRUPT:
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_PARTSENSE_ENABLE) = (unsigned short)arg;
	 if (!arg)
	 {
	     //printk("enable_part_interrupt\n");	    
	     enable_part_interrupt = 1;
	 }
        }
	return arg;
// sbs 2.21 11-Jul-2008 MOSAIC: Z-axis servo status is read indirectly through the Xilinx
    case XIL_Z_SERVO_STATUS:
	if (xil_addr_base)
	{
	    arg = (unsigned long)*((unsigned short*)xil_addr_base + XIL_Z_STATUS_OFFSET);   
	}
	else
	    arg = -1;
	//printk("XIL_Z_SERVO_STATUS %x\n",arg);
	return arg;
//sbs 2.21 07-Jul-2008 Add an IO Change Event for PANNIER
    case XIL_SET_IO_CHANGE:
	//printk("Set IO change %x\n",(unsigned short)arg);
	if (xil_addr_base)
	{
	 *((unsigned short*)xil_addr_base + XIL_IO_CHANGE_OFFSET) = (unsigned short)arg;   
	}
	else
	    arg = -1;
	return arg;
    
	// 05-Aug-2009 sbs 2.57: Allow Mark Aborts from a predetermined user-enabled input (Input 7).
	// Ran out of signals so Abort and IO Change share a signal. The interrupt type determines
	// What the signal handler should do.
    case XIL_GET_INTERRUPT_TYPE:	
	arg = interrupt_type;
	return arg;
	
	
/*
    case XIL_ENABLE_INTERRUPTS:
	//printk("Interrupts %x\n",(unsigned short)arg);
	if((unsigned short)arg > 0)
	    enable_main_interrupts = 1;
	else
	    enable_main_interrupts = 0;
	return arg;
*/
    default:
	return -ENOTTY;    	
    }
    return 0;
}
/***************************************
  * Function so that other modules can communicate with the Xilinx, namely the SPI module...
  **************************************/
void* at91_xil_get_mapped_address(void)
{
   return xil_addr_base; 
}
/********************************************************************/
static struct file_operations flyer_xil_fops = {
    owner:		THIS_MODULE,
    read:		flyer_xil_read,
    write:		flyer_xil_write,
    ioctl:		flyer_xil_ioctl,
    open:		flyer_xil_open,
    release:	        flyer_xil_release,
    };

static int __init at91_flyer_xil_init_module(void)
{
    int res = 0;
    int status;
    MSG("Module at91_flyer_xil init\n" );
    readBuf = kmalloc(BUFSIZE, GFP_KERNEL);
    writeBuf = kmalloc(BUFSIZE,GFP_KERNEL);
    if (!readBuf || !writeBuf)
	return -ENOMEM;
    /*register the device with the kernel*/	
    res = register_chrdev(XILINX_CONFIG_MAJOR,"flyer_xil",&flyer_xil_fops);
    if (res)
    {
	MSG("Can't register device flyer_xil with kernel.\n");
	return res;
    }
    
    if (!xil_addr_base)
    {
	AT91PS_SYS pSys = (AT91PS_SYS) AT91C_VA_BASE_SYS;
	pSys->EBI_SMC2_CSR[2]=0x11003082;
	xil_addr_base = (void*) ioremap(FLYER_XILINX_BASE, SZ_16K);
	printk(KERN_DEBUG "xil_addr_base=0x%x\n",(unsigned int)xil_addr_base);
    }
    
    //write a mask of zero and a input input wait of zero prior to getting the interrupt
    *((unsigned short*)xil_addr_base + XIL_WAIT_DIGITAL_OFFSET) = 0x0000;
    AT91_SYS->PIOB_PER = AT91C_PIO_PB24; //interrupt line to the DSP
    AT91_SYS->PIOB_SODR = AT91C_PIO_PB24;// clear it initially
    AT91_SYS->PIOB_OER = AT91C_PIO_PB24; //set as output
    //disable glitch filters
    AT91_SYS->PIOB_IFDR = AT91C_PIO_PB24;
    //disable interrupts
    AT91_SYS->PIOB_IDR = AT91C_PIO_PB24;
    //disable multiple drivers
    AT91_SYS->PIOB_MDDR = AT91C_PIO_PB24;
    //disable internal pull-up
    AT91_SYS->PIOB_PPUDR = AT91C_PIO_PB24;
    
    //USB is simple PIO output
    pPIOD->PIO_PER = USB_RED_LINE | USB_GREEN_LINE;
    pPIOD->PIO_CODR = USB_RED_LINE | USB_GREEN_LINE;
    pPIOD->PIO_OER = USB_RED_LINE | USB_GREEN_LINE;

    //status is controlled by the timer/counter peripheral
    AT91_SYS->PMC_PCER = 1 << AT91C_ID_TC4;	/* enable peripheral clock for TC4*/
    /*Select the B register for 8 and 9*/
    pPIOB->PIO_BSR = (STAT_RED_LINE | STAT_GREEN_LINE);
    /*Turn on the peripheral access to TIOA4 and TIOB4*/
    pPIOB->PIO_PDR = (STAT_RED_LINE | STAT_GREEN_LINE);	
    
    //setup the gpio for the irq...
    /*Select the A register for PB29*/
    AT91_SYS->PIOB_ASR = AT91C_PB29_IRQ0 ;
    /*Turn on the peripheral access to IRQ0 */
    AT91_SYS->PIOB_PDR = AT91C_PB29_IRQ0;
    AT91_SYS->AIC_SMR[AT91C_ID_IRQ0] = (AT91C_AIC_SRCTYPE_EXT_POSITIVE_EDGE | AT91C_AIC_PRIOR_LOWEST);
    status = request_irq(AT91C_ID_IRQ0, at91_flyer_xil_interrupt, 0, "flyer_xil", xil_addr_base);
    if (status) 
    {
	printk(KERN_ERR "at91_flyer_xil: IRQ %d request failed - status %d!\n", AT91C_ID_IRQ0, status);
	return -EBUSY;
    }   
    status = request_irq(AT91C_ID_TC4, at91_flyer_xil_tc_interrupt, 0, "flyer_xil", 0);
    if (status) 
    {
	printk(KERN_ERR "at91_flyer_xil: IRQ %d request failed - status %d!\n", AT91C_ID_TC4, status);
	return -EBUSY;
    } 
    
    //configure the GPIO registers
    pPIOC->PIO_PER = XILINX_ALL;
    pPIOC->PIO_SODR = XILINX_PROGRAM;
    pPIOC->PIO_OER = XILINX_DATA_IN | XILINX_CLOCK | XILINX_PROGRAM;
    pPIOC->PIO_ODR = XILINX_INIT | XILINX_DONE;
    //disable glitch filters
    pPIOC->PIO_IFDR = XILINX_ALL;
    //disable interrupts
    pPIOC->PIO_IDR = XILINX_ALL;
    //disable multiple drivers
    pPIOC->PIO_MDDR = XILINX_ALL;
    //disable internal pull-up
    pPIOC->PIO_PPUDR = XILINX_ALL; 
    
    //due to atmel errata
    pPIOC->PIO_PPUER = AT91C_PIO_PC6;  
    
    *((unsigned short*)xil_addr_base + XIL_IO_CHANGE_OFFSET) = 0;   
    return 0;
}

static void __exit at91_flyer_xil_exit_module(void)
{
    printk( KERN_DEBUG "Module at91_flyer_xil exit\n" );
    if (readBuf)
	kfree(readBuf);
    if (writeBuf)
	kfree(writeBuf);
    free_irq(AT91C_ID_IRQ0,0);
    /*unregister the device with the kernel*/	
    int res = unregister_chrdev(XILINX_CONFIG_MAJOR,"flyer_xil");
    if (res)
    {
	MSG("Can't unregister Xilinx device with kernel.\n");
    }
    if (xil_addr_base)
    {
	iounmap(xil_addr_base);
        printk(KERN_DEBUG "Releasing xil_addr_base @ 0x%x\n",(unsigned int)xil_addr_base);
	xil_addr_base = NULL;
    }
    
    //Turn off the GPIO lines...
    pPIOC->PIO_PDR = XILINX_ALL;
    pPIOC->PIO_ODR = XILINX_DATA_IN | XILINX_CLOCK | XILINX_PROGRAM;
    
}

static irqreturn_t at91_flyer_xil_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    
    unsigned short int_table = 0;
    unsigned char io_stat;
    //if(!enable_main_interrupts)
	//return IRQ_HANDLED;
    
    if (!xil_addr_base)
    {
	printk (KERN_ERR "In xilinx interrupt and xil_addr_base not set!\n");
	return IRQ_HANDLED;
    }
    int_table = *((unsigned short*)xil_addr_base + XIL_INT_TABLE); 
    //printk("int_table: %x\n",int_table);
    if ( (int_table & IO_INTERRUPT) )
    {
	//printk("IO_INTERRUPT\n");
	interrupt_type = IO_INTERRUPT;
	int_table &= ~TIMEOUT_INTERRUPT;
	waitIOSuccess = 1;
	*((unsigned short*)xil_addr_base + XIL_WAIT_DIGITAL_OFFSET) = 0;//clear the mask!
	wake_up_interruptible(&io_queue);//wake up the waiting process(es)
    }
    if ( (int_table & TIMEOUT_INTERRUPT) )
    {
	//printk("TIMEOUT_INTERRUPT\n");
	waitIOSuccess = 0;
	interrupt_type = TIMEOUT_INTERRUPT;
	*((unsigned short*)xil_addr_base + XIL_WAIT_DIGITAL_OFFSET) = 0;//clear the mask!
	wake_up_interruptible(&io_queue);//wake up the waiting process(es)
    }
    if ( (int_table & TRACK_INTERRUPT) )
    {
	//printk("TRACK_INTERRUPT\n");
	interrupt_type = TRACK_INTERRUPT;
	wake_up_interruptible(&track_queue);//wake up the waiting process(es)
    }
    
    if (int_table & KEY_INTERRUPT)
    {
	//read the key_stat to clear the interrupt with the Xilinx
	unsigned char key_stat = *((unsigned char*)xil_addr_base + XIL_KEYPAD_OFFSET);
	interrupt_type = KEY_INTERRUPT;
	if (marking_testmark)
	{
	    //printk(KERN_ERR "Already Marking testmark, ignoring...\n");
	    return IRQ_HANDLED;
	}
	if (main_pid && key_stat & TESTMARK_KEY)
	{
	    printk(KERN_ERR "Running TestMark, key register: 0x%02x\n",key_stat);
	    marking_testmark = 1;
	    kill_proc(main_pid,SIG_TESTMARK,1);
	}
    }
    
    if (int_table & TEMP_INTERRUPTS)
    {
	//printk("Got Overtemp\n");
	temp_status = (int_table & TEMP_INTERRUPTS) >> 3;
	interrupt_type = TEMP_INTERRUPTS;
	if (main_pid)
	    kill_proc(main_pid,SIG_OVERTEMP,1);
	
    }
    
    //sbs 2.21 07-Jul-2008 Add an IO Change Event for PANNIER
    if (int_table & IOCHANGE_INTERRUPT)
    {
	io_stat = *((unsigned char*)xil_addr_base + XIL_IO_CHANGE_OFFSET);
	//printk("Got IO Change %x  : %d  :%x\n",int_table, SIG_IOCHANGE, io_stat);
	interrupt_type = IOCHANGE_INTERRUPT;
	if (main_pid)
	    kill_proc(main_pid,SIG_IOCHANGE,1);
    }
    
    if (int_table & ABORT_INTERRUPT)
    {
	io_stat = *((unsigned char*)xil_addr_base + XIL_SWITCHES);
	interrupt_type = ABORT_INTERRUPT;
	//printk("Got IO Change %x  : %d  :%x\n",int_table, SIG_IOCHANGE, io_stat);
	if (main_pid)
	    kill_proc(main_pid,SIG_IOCHANGE,1);
    }    
    return IRQ_HANDLED;
}

EXPORT_SYMBOL(at91_xil_get_mapped_address);

module_init(at91_flyer_xil_init_module);
module_exit(at91_flyer_xil_exit_module);


MODULE_DESCRIPTION("Module to initialize the Xilinx XC2S50E FPGA");
MODULE_AUTHOR("J. Warren (jeffw@synrad.com)");
MODULE_LICENSE("GPL");
