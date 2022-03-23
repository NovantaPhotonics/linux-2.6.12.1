/*
 * User-space interface to the SPI bus on Atmel AT91RM9200
 *
 *  Copyright (C) 2003 SAN People (Pty) Ltd
 *
 * Based on SPI driver by Rick Bronson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>

#include <linux/at91_spi.h>
#include <linux/MAX3421E.h>
#include <asm/arch/AT91RM9200_SPI.h>
#include <asm/arch/AT91RM9200_PIO.h>
#include <asm/arch/at91_Flyer_Xilinx.h> //for the reads/writes to Xilinx
#include <asm/arch/pio.h>
#include <asm/arch/gpio.h>
#include <asm/byteorder.h>
#include <linux/pci.h>
#include <linux/usb_ch9.h>

#define SPI_LINK_MAJOR 244
#define SPI_IOCTL_BASE 0xAF
#define SPI_GET_SERVO_STATUS _IOWR(SPI_IOCTL_BASE,1,int)
#define SPI_SET_SPI_MODE _IOR(SPI_IOCTL_BASE,2,int)
#define SPI_GET_BUFFER_SIZE _IOWR(SPI_IOCTL_BASE,5,int)
#define SPI_SERVO_RESET _IOR(SPI_IOCTL_BASE,6,int)
#define SPI_SERVO_BUS _IOR(SPI_IOCTL_BASE,7,int)
#define SPI_SERVO_READY _IOR(SPI_IOCTL_BASE,8,int)
#define SPI_SERVO_STATUS _IOR(SPI_IOCTL_BASE,9,int)
#define ACKST	1

#define STRING_MANUFACTURER	25
#define STRING_PRODUCT		42
#define STRING_SERIAL		101
#define MAIN_STRING_CONFIG		102
#define URGENT_STRING_CONFIG		103
#define STRING_CONFIG   		250

#define CONFIG_FLYER		1
#define MAIN_INTERFACE_ID	0
#define URGENT_INTERFACE_ID	1

#define DRIVER_VENDOR_NUM	0x7967
#define DRIVER_PRODUCT_NUM	0x1593

#define MAXIM_VENDOR_NUM	0x0B6A
#define MAXIM_PRODUCT_NUM	0x5346

// SETUP packet offsets
#define bmRequestType           1
#define	bRequest		2
#define wValueL			3
#define wValueH			4
#define wIndexL			5
#define wIndexH			6
#define wLengthL		7
#define wLengthH		8

// Get Descriptor codes	
#define GD_DEVICE		0x01	// Get device descriptor: Device
#define GD_CONFIGURATION	0x02	// Get device descriptor: Configuration
#define GD_STRING		0x03	// Get device descriptor: String
#define GD_HID	            	0x21	// Get descriptor: HID
#define GD_REPORT	        0x22	// Get descriptor: Report


#define ENABLE_IRQS WriteUSBReg(rEPIEN,(bmSUDAVIE+bmIN2BAVIE+bmOUT1DAVIRQ),0); WriteUSBReg(rUSBIEN,(bmURESIE+bmURESDNIE),0);


//make it big enough to accept the boot code currently 11000 bytes
#define BUFSIZE PAGE_SIZE*8

#ifdef DEBUG
#define MSG(string, args...) printk(KERN_DEBUG "spi_link:" string, ##args)
#else
#define MSG(string, args...)
#endif

//typedef unsigned char BYTE;
static int iDev2 = 2;
static int iDev3 = 3;
static int bAcquiring = 0;
static int bAborted = 0;
DECLARE_WAIT_QUEUE_HEAD(wait_queue);
static int m_iStatus = 0;
char* readBuf;
char* writeBuf;
void* xil_addr;

BYTE SUD[9];
BYTE msgidx,msglen;	// Text string in EnumApp_enum_data.h--index and length
BYTE configval;		// Set/Get_Configuration value
BYTE ep3stall;		// Flag for EP3 Stall, set by Set_Feature, reported back in Get_Status
BYTE interfacenum;      // Set/Get interface value
BYTE inhibit_send;	// Flag for the keyboard character send routine
BYTE RWU_enabled;       // Set by Set/Clear_Feature RWU request, sent back for Get_Status-RWU
BYTE Suspended;         // Tells the main loop to look for host resume and RWU pushbutton
WORD msec_timer;        // Count off time in the main loop
WORD blinktimer;        // Count milliseconds to blink the "loop active" light
BYTE send3zeros;        // EP3-IN function uses this to send HID (key up) codes between keystrokes
BYTE usbtestdata[65];
BYTE usbrcvdata[257];

// STRING descriptors. An array of string arrays

const unsigned char strDesc[][64]= {
// STRING descriptor 0--Language string
{
        0x04,			// bLength
	0x03,			// bDescriptorType = string
	0x09,0x04		// wLANGID(L/H) = English-United Sates
},
// STRING descriptor 1--Manufacturer ID
{
        14,			// bLength
	0x03,			// bDescriptorType = string
	'S',0,'y',0,'n',0,'r',0,'a',0,'d',0 // text in Unicode
}, 
// STRING descriptor 2 - Product ID
{	16,			// bLength
	0x03,			// bDescriptorType = string
	'U',0,'S',0,'B',0,'-',0,'S',0,'P',0,'I',0
},

// STRING descriptor 3 - Serial Number ID
{       20,			// bLength
	0x03,			// bDescriptorType = string
	'S',0,				
	'/',0,
	'N',0,
	' ',0,
	'1',0,
	'4',0,
	'2',0,
	'0',0,
        'E',0,
}};

struct usb_config{
  struct usb_config_descriptor full_speed_config;
  //struct usb_otg_descriptor otg_descriptor;
  struct usb_interface_descriptor main_intf;
  //struct usb_hid_descriptor hid_desc;
  struct usb_endpoint_descriptor fs_source_main_desc;
  struct usb_endpoint_descriptor fs_sink_main_desc;
} __attribute__ ((packed));

static struct usb_device_descriptor device_desc;


static struct usb_config full_config;

static AT91PS_PIO pPIOD = (AT91PS_PIO)AT91_IO_P2V(AT91C_BASE_PIOD);

/* Allocate a single SPI transfer descriptor.  We're assuming that if multiple
   SPI transfers occur at the same time, spi_access_bus() will serialize them.
   If this is not valid, then either (i) each dataflash 'priv' structure
   needs it's own transfer descriptor, (ii) we lock this one, or (iii) use
   another mechanism.   */
static struct spi_transfer_list* spi_transfer_desc;

static irqreturn_t at91_usbspi_interrupt(int irq, void *dev_id, struct pt_regs *regs);
static int do_spi_transfer(int nr, char* tx, int tx_len, char* rx, int rx_len,
		char* txnext, int txnext_len, char* rxnext, int rxnext_len, int txtype);
BYTE ReadUSBReg(BYTE reg, BYTE ackstat);
BYTE ReadBytesN(unsigned char reg, BYTE* data, int size);
BYTE WriteUSBReg(BYTE reg, BYTE val, BYTE ackstat);
BYTE WriteBytesN(unsigned char reg, BYTE* data, int size);
void Initialize_USB(void);
void InitializeDescriptors(void);
void do_Setup(void);
void std_request(void);
void class_request(void);
void vendor_request(void);
void send_descriptor(void);
void feature(BYTE sc);
void get_status(void);
void set_configuration(void);
void get_configuration(void);
void set_interface(void);
void get_interface(void);
void do_IN2(void);
void do_OUT1(void);


void Initialize_USB(void)
{
  
  unsigned char dum = 0;
 // Always set the FDUPSPI bit in the PINCTL register FIRST if you are using the SPI port in 
  // full duplex mode. This configures the port properly for subsequent SPI accesses.
  // Set for Level based intterupts
  WriteUSBReg(rPINCTL, bmFDUPSPI+GPX_SOF,0);

  // Reset chip and wait for oscillator to stabilize
  WriteUSBReg(rUSBCTL, 0x20,0);
  WriteUSBReg(rUSBCTL, 0x0,0);

  // Wait for oscillator to stabilize
  do                 
    {
      dum = ReadUSBReg(rUSBIRQ,0);
      dum &=bmOSCOKIRQ;
    }
    while (dum==0);
  
    // Connect USB
  WriteUSBReg(rUSBCTL,bmCONNECT,0);
   
  // Enable IRQs
  
  WriteUSBReg(rEPIEN,bmSUDAVIE+bmIN2BAVIE+bmOUT1DAVIRQ,0);
  WriteUSBReg(rUSBIEN, bmURESIE+bmURESDNIE+bmNOVBUSIE,0);
  
  // Enable interrupt pin
  WriteUSBReg(rCPUCTL,bmIE,0);
  
}

void InitializeDescriptors(void)
{

 int i;
    // Device Descriptor
  device_desc.bLength = 		sizeof device_desc;
  device_desc.bDescriptorType = 	USB_DT_DEVICE;
  device_desc.bcdUSB = 		        0x0200;
  device_desc.bDeviceClass = 	        0;
  device_desc.bDeviceSubClass = 	0;
  device_desc.bDeviceProtocol = 	0;
  device_desc.bMaxPacketSize0 = 	0x40;
  device_desc.idVendor = 	        DRIVER_VENDOR_NUM;
  device_desc.idProduct = 	        DRIVER_PRODUCT_NUM;
  device_desc.bcdDevice = 	        0x1234;
  device_desc.iManufacturer = 	1;
  device_desc.iProduct = 	2;
  device_desc.iSerialNumber = 	3;
  device_desc.bNumConfigurations = 1;
  
  //ddata = (BYTE*)&device_desc;
  /*
  for (int i = 0; i<(device_desc.bLength); i++)
	   {
		printf("ddata[%d]: %x\n",i,ddata[i]);
	   }
  */
  // Configuration Descriptor
  full_config.full_speed_config.bLength =		USB_DT_CONFIG_SIZE;
  full_config.full_speed_config.bDescriptorType =	USB_DT_CONFIG;

	/* compute wTotalLength on the fly */
  full_config.full_speed_config.bNumInterfaces =	1;
  full_config.full_speed_config.bConfigurationValue =	1;
  full_config.full_speed_config.iConfiguration =	0;
  full_config.full_speed_config.bmAttributes =		USB_CONFIG_ATT_ONE | USB_CONFIG_ATT_SELFPOWER | USB_CONFIG_ATT_WAKEUP;
  full_config.full_speed_config.bMaxPower =		1;	/* self-powered */
  
  //Interface Descriptor
  full_config.main_intf.bLength =		USB_DT_INTERFACE_SIZE;
  full_config.main_intf.bDescriptorType =	USB_DT_INTERFACE;
  full_config.main_intf.bInterfaceNumber =	0;
  full_config.main_intf.bNumEndpoints =	2;
  full_config.main_intf.bInterfaceClass =	USB_CLASS_VENDOR_SPEC;
  full_config.main_intf.iInterface =		0;
  /*
  // HID descriptor
  full_config.hid_desc.bLength = 			USB_DT_HID_SIZE;
  full_config.hid_desc.bDescriptorType =		0x21;
  full_config.hid_desc.bcdHID = 			0x0110;
  full_config.hid_desc.bCountryCode = 			0x0;
  full_config.hid_desc.bNumDescriptors = 		0x1;
  full_config.hid_desc.bHIDDescriptorType = 		0x22;
  full_config.hid_desc.wDescriptorLength = 		43;
  */
  // Endpoint descriptors
  /* two full speed bulk endpoints; their use is config-dependent */
  // EP3IN
  full_config.fs_source_main_desc.bLength =		USB_DT_ENDPOINT_SIZE;
  full_config.fs_source_main_desc.bDescriptorType =	USB_DT_ENDPOINT;
  full_config.fs_source_main_desc.bEndpointAddress =	USB_DIR_IN;
  full_config.fs_source_main_desc.wMaxPacketSize =	0x40;
  full_config.fs_source_main_desc.bmAttributes =	USB_ENDPOINT_XFER_BULK;
  //full_config.fs_source_main_desc.bInterval =	        10;
  
  //EP1OUT
  full_config.fs_sink_main_desc.bLength =		USB_DT_ENDPOINT_SIZE;
  full_config.fs_sink_main_desc.bDescriptorType =	USB_DT_ENDPOINT;
  full_config.fs_sink_main_desc.wMaxPacketSize =	0x40;
  full_config.fs_sink_main_desc.bEndpointAddress =	USB_DIR_OUT;
  full_config.fs_sink_main_desc.bmAttributes =	USB_ENDPOINT_XFER_BULK;	
  
  full_config.full_speed_config.wTotalLength = full_config.full_speed_config.bLength + 
					       full_config.main_intf.bLength +
					       full_config.fs_source_main_desc.bLength +
					       full_config.fs_sink_main_desc.bLength;
  
  for(i = 1; i<65; i++)
      usbtestdata[i] = i;
  //ddata = (BYTE*)&full_config;
  /*
  for (int i = 0; i<(full_config.full_speed_config.wTotalLength); i++)
	   {
		printf("full_config[%d]: %x\n",i,ddata[i]);
	   }*/
}

void do_Setup(void)
{
    ReadBytesN(rSUDFIFO, SUD, 8);
    //for(i=0; i<8; i++)
	//printf("SUD[%d]: %x  AND 0x60: %x\n",i,SUD[i], (SUD[i] & 0x60));
    
    switch((SUD[bmRequestType] & 0x60))
    {
	case 0x00:	std_request();	break;
	case 0x20:	STALL_EP0;              break;
	case 0x40:	STALL_EP0;	        break;
	//default: STALL_EP0(device);
	default: std_request();
    }
    
}

void std_request(void)
{
    switch(SUD[bRequest])
    {
	case    USB_REQ_GET_DESCRIPTOR:	   send_descriptor();	break;
	case	USB_REQ_SET_FEATURE:	   feature(1);           break;
	case	USB_REQ_CLEAR_FEATURE:	   feature(0);           break;
	case	USB_REQ_GET_STATUS:	   get_status();         break;
	case	USB_REQ_SET_INTERFACE:	   set_interface();      break;
	case	USB_REQ_GET_INTERFACE:	   get_interface();      break;
	case	USB_REQ_GET_CONFIGURATION: get_configuration();  break;
	case	USB_REQ_SET_CONFIGURATION: set_configuration();  break;
	//case	USB_REQ_SET_ADDRESS:         ReadUSBRegAS(device,rFNADDR);      break;  // discard return value
	case	USB_REQ_SET_ADDRESS:         ReadUSBReg(rFNADDR,ACKST);      break;
	default: STALL_EP0;
    }
}

void set_configuration(void)
{
    configval=SUD[wValueL];           // Store the config value
    if(configval != 0)                // If we are configured, 
	SETBIT(rUSBIEN,bmSUSPIE);       // start looking for SUSPEND interrupts
    ReadUSBReg(rFNADDR,ACKST);                  // dummy read to set the ACKSTAT bit
}

void get_configuration(void)
{
WriteUSBReg(rEP0FIFO,configval,0);         // Send the config value
WriteUSBReg(rEP0BC,1,ACKST);   
}

//**********************
void set_interface(void)	// All we accept are Interface=0 and AlternateSetting=0, otherwise send STALL
{
BYTE dumval;
if((SUD[wValueL]==0)		// wValueL=Alternate Setting index
  &&(SUD[wIndexL]==0))		// wIndexL=Interface index
  	//dumval=ReadUSBRegAS(dev,rFNADDR);	// dummy read to set the ACKSTAT bit
    dumval=ReadUSBReg(rFNADDR,ACKST);
else STALL_EP0;
}

//**********************
void get_interface(void)	// Check for Interface=0, always report AlternateSetting=0
{
if(SUD[wIndexL]==0)		// wIndexL=Interface index
  {
  WriteUSBReg(rEP0FIFO,0,0);		// AS=0
  //WriteUSBRegAS(dev,rEP0BC,1);		// send one byte, ACKSTAT
  WriteUSBReg(rEP0BC,1,ACKST);
  }
else STALL_EP0;
}

void get_status(void)
{
BYTE testbyte;
testbyte=SUD[bmRequestType];
switch(testbyte)	
	{
	case 0x80: 			// directed to DEVICE
		WriteUSBReg(rEP0FIFO,(RWU_enabled+1),0);	// first byte is 000000rs where r=enabled for RWU and s=self-powered.
		WriteUSBReg(rEP0FIFO,0x00,0);		// second byte is always 0
		//WriteUSBRegAS(dev,rEP0BC,2); 		// load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
		WriteUSBReg(rEP0BC,2,1);
		break; 				
	case 0x81: 			// directed to INTERFACE
		WriteUSBReg(rEP0FIFO,0x00,0);		// this one is easy--two zero bytes
		WriteUSBReg(rEP0FIFO,0x00,0);		
		//WriteUSBRegAS(dev,rEP0BC,2); 		// load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
		WriteUSBReg(rEP0BC,2,1);
		break; 				
	case 0x82: 			// directed to ENDPOINT
		if(SUD[wIndexL]==0x82)		// We only reported ep3, so it's the only one the host can stall IN3=83
                  {
                  WriteUSBReg(rEP0FIFO,ep3stall,0);	// first byte is 0000000h where h is the halt (stall) bit
                  WriteUSBReg(rEP0FIFO,0x00,0);		// second byte is always 0
                  //WriteUSBRegAS(dev,rEP0BC,2); 		// load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
		  WriteUSBReg(rEP0BC,2,1);
                  break;
                  }
		else  STALL_EP0;	// Host tried to stall an invalid endpoint (not 3)				
	default:      STALL_EP0;		// don't recognize the request
	}
}

void feature(BYTE sc)
{
    BYTE mask;
  if((SUD[bmRequestType]==0x02)	// dir=h->p, recipient = ENDPOINT
  &&  (SUD[wValueL]==0x00)	// wValueL is feature selector, 00 is EP Halt
  &&  (SUD[wIndexL]==0x82))	// wIndexL is endpoint number IN3=83
      {
      mask=ReadUSBReg(rEPSTALLS,0);   // read existing bits
      if(sc==1)               // set_feature
        {
        mask += bmSTLEP2IN;       // Halt EP3IN
        ep3stall=1;
        }
      else                        // clear_feature
        {
        mask &= ~bmSTLEP2IN;      // UnHalt EP3IN
        ep3stall=0;
        WriteUSBReg(rCLRTOGS,bmCTGEP2IN,0);  // clear the EP3 data toggle
        }
      WriteUSBReg(rEPSTALLS,(mask|bmACKSTAT),0); // Don't use wregAS for this--directly writing the ACKSTAT bit
      }
  else if ((SUD[bmRequestType]==0x00)	// dir=h->p, recipient = DEVICE
           &&  (SUD[wValueL]==0x01))	// wValueL is feature selector, 01 is Device_Remote_Wakeup
            {
            RWU_enabled = sc<<1;	// =2 for set, =0 for clear feature. The shift puts it in the get_status bit position.			
            //ReadUSBRegAS(dev,rFNADDR);		// dummy read to set ACKSTAT
	    ReadUSBReg(rFNADDR,1);
            }
  else STALL_EP0;
}

void send_descriptor(void)
{
    WORD reqlen, sendlen, desclen;
    BYTE *pDdata;
    
    desclen = 0;					// check for zero as error condition (no case statements satisfied)
    reqlen = SUD[wLengthL] + 256*SUD[wLengthH];	// 16-bit
	switch (SUD[wValueH])			// wValueH is descriptor type
	{
	case  GD_DEVICE:
	      //printf("Device Desc\n");
              //desclen = DD[0];	// descriptor length
	      desclen = device_desc.bLength;
	      //printf("desclen: %d  reqlen: %d\n",desclen, reqlen);
              pDdata = (BYTE*)&device_desc;
              break;	
	case  GD_CONFIGURATION:
	      //printf("Config Desc\n");
              //desclen = CD[2];	// Config descriptor includes interface, HID, report and ep descriptors
	      desclen = full_config.full_speed_config.wTotalLength;
	      //printf("desclen: %d  reqlen: %d\n",desclen, reqlen);
              pDdata = (BYTE*)&full_config;
              break;
	case  GD_STRING:
	      //printf("String Desc\n");
              desclen = strDesc[SUD[wValueL]][0];   // wValueL=string index, array[0] is the length
	      //printf("desclen: %d  reqlen: %d\n",desclen, reqlen);
              pDdata = (BYTE*)strDesc[SUD[wValueL]];       // point to first array element
              break;
	      /*
	case  GD_HID:
	      printf("HID Desc\n");
              desclen = CD[18];
              pDdata = (BYTE*)&CD[18];
	      //desclen = full_config.hid_desc.bLength;
	      //pDdata = (BYTE*)&full_config.hid_desc;
              break;
	case  GD_REPORT:
	      printf("Report Desc\n");
              desclen = CD[25];
              pDdata = (BYTE*)RepD;
	      break;
	      */
	}	// end switch on descriptor type
//
	if (desclen!=0)                   // one of the case statements above filled in a value
	{
	   sendlen = (reqlen <= desclen) ? reqlen : desclen; // send the smaller of requested and avaiable
	   /* 
	   BYTE data[sendlen+1];
	    for (int i = 1; i<(sendlen+1); i++)
	   {
		data[i] = pDdata[i-1];
		//printf("pDdata[%d]: %x\n",i-1,pDdata[i-1]);
	   }
	   */	
	    WriteBytesN(rEP0FIFO,pDdata,sendlen);
	    
	   //maxdata->cmd = rEP0FIFO;
	   //maxdata->data = pDdata;
	   //WriteBytes(device,maxdata,sendlen+1);
	    //WriteUSBRegAS(device,rEP0BC,sendlen);   // load EP0BC to arm the EP0-IN transfer & ACKSTAT
	    WriteUSBReg(rEP0BC,sendlen,1);
	}
	else STALL_EP0;  // none of the descriptor types match
    
}

void do_IN2(void)
{
  //if (inhibit_send==0x00)
  {
	WriteBytesN(rEP2INFIFO,usbtestdata,65);
	WriteUSBReg(rEP2INBC,64,0);
  }
   
}
/*
 * Perform a SPI transfer to access the DataFlash device.
 */
static int do_spi_transfer(int nr, char* tx, int tx_len, char* rx, int rx_len,
		char* txnext, int txnext_len, char* rxnext, int rxnext_len, int txtype)
{
	struct spi_transfer_list* list = spi_transfer_desc;

	list->tx[0] = tx;	list->txlen[0] = tx_len;
	list->rx[0] = rx;	list->rxlen[0] = rx_len;

	list->tx[1] = txnext; 	list->txlen[1] = txnext_len;
	list->rx[1] = rxnext;	list->rxlen[1] = rxnext_len;

	list->nr_transfers = nr;
	list->txtype = txtype;
	return spi_transfer(list);
}

//read is used for the asynchronous write
static ssize_t spi_link_read(struct file* file, char* buf, size_t count, loff_t *offset)
{
    ssize_t ret;    
    int type = SPI_SYNC;
    int releasebus = 0;
    int *pMinor = (int*)file->private_data;
    if (*pMinor != iDev3) 
	return 0;     
    //unsigned short xil;
    //char wrBuf;
    
    int readCount =(count > BUFSIZE ? BUFSIZE : count);
    /*
    if (spi_ready(iDev3)==-1)
    {

        if (bAcquiring)
	{
	    wait_queue_t wait;
	    init_waitqueue_entry(&wait,current);
	    add_wait_queue(&wait_queue,&wait);
	    set_current_state(TASK_INTERRUPTIBLE);
	    if (bAcquiring)
	    {
		printk(KERN_ERR "%d:bAcquiring\n",current->pid);
		schedule();//put the calling process to sleep
	    }
	    set_current_state(TASK_RUNNING);
	    remove_wait_queue(&wait_queue,&wait);	    
	    if (bAborted)
	    {
		printk(KERN_ERR "spi_link_write: pid:%d, Aborted\n",current->pid);
		bAborted = 0;
		return -1;
	    }
	}
	else
	{
	    spi_access_bus(iDev3);
	    releasebus = 1;
	}
	
    }
    */
    copy_from_user(readBuf,buf,readCount);
    //while(!spi_ready(iDev3));
    
    do_spi_transfer(1,readBuf,readCount,readBuf,readCount,NULL,0,NULL,0,type);
    /*
    if (releasebus)
    {	
	spi_release_bus(iDev3);	
    }
    */
    //printk("data1: %x  data2: %x\n", readBuf[0], readBuf[1]);
    copy_to_user(buf, readBuf, readCount);
    
    
    ret = (ssize_t)readCount;
    return ret;
}

//write is ALWAYS synchronous
static ssize_t spi_link_write(struct file* file, const char* buf, size_t count, loff_t *offset)
{
    ssize_t ret;
    int type = SPI_ASYNC;    
    int *pMinor = (int*)file->private_data;
    if (*pMinor != iDev3)
	return 0;
    //unsigned short xil;
    int releasebus = 0;
    int writeCount =(count > BUFSIZE ? BUFSIZE : count);
    //if(!xil_addr)
	//xil_addr = at91_xil_get_mapped_address();
    /*
    if (spi_ready(iDev3)==-1)
    {

        if (bAcquiring)
	{
	    wait_queue_t wait;
	    init_waitqueue_entry(&wait,current);
	    add_wait_queue(&wait_queue,&wait);
	    set_current_state(TASK_INTERRUPTIBLE);
	    if (bAcquiring)
	    {
		printk(KERN_ERR "%d:bAcquiring\n",current->pid);
		schedule();//put the calling process to sleep
	    }
	    set_current_state(TASK_RUNNING);
	    remove_wait_queue(&wait_queue,&wait);	    
	    if (bAborted)
	    {
		printk(KERN_ERR "spi_link_write: pid:%d, Aborted\n",current->pid);
		bAborted = 0;
		return -1;
	    }
	}
	else
	{
	    spi_access_bus(*pMinor);
	    releasebus = 1;
	}
	
    }
    */
    copy_from_user(writeBuf,buf,writeCount);

    
    //while(!spi_ready(*pMinor));
    
    do_spi_transfer(1,writeBuf,writeCount,writeBuf,writeCount,NULL,0,NULL,0,type);
    /*
    if (releasebus)
    {	
	spi_release_bus(*pMinor);	
    }
    */
    //xil = *((unsigned short*)xil_addr + XIL_STATUS_OFFSET);
    //ret = (ssize_t)( ((int)xil << 16) + ((unsigned short*)readBuf)[writeCount/2 -1]);
    //m_iStatus = ret;
    
    ret = (ssize_t)writeCount;
    return ret;
}

static int spi_link_open(struct inode* inode, struct file* file)
{
	int iDevCurrent = iminor(inode);
	MSG("Module spi_link open, iMinor = %d\n",iDevCurrent );
	if (iDevCurrent < 2 || iDevCurrent > 3)
		return -ENODEV;
	if (iDevCurrent == 2)
		file->private_data = &iDev2;
	else if (iDevCurrent ==3)
		file->private_data = &iDev3;

	//Enable interrupts
	//pPIOD->PIO_IER |= AT91C_PIO_PD9;
	//InitializeDescriptors();
	//Initialize_USB();
	return 0;
}

static int spi_link_release(struct inode* inode, struct file* file)
{
    if (spi_ready(iDev3)!=-1)
    {	
	spi_release_bus(iDev3);		
    }	
    if (waitqueue_active(&wait_queue))
    {	
	bAborted = 1;
	wake_up_interruptible(&wait_queue);
	printk(KERN_ERR "Found process on wait_queue during releasebus!\n");
    }
    return 0;
}

static int spi_link_ioctl(struct inode* inode, struct file* file, unsigned int cmd, unsigned long arg)
{
    AT91PS_SPI controller = (AT91PS_SPI) AT91C_VA_BASE_SPI;
    
    //short* pBuf = (short*)0;
    int iMinor = iminor(inode);
    //short* pReadBuf = (short*)readBuf;
    unsigned int test=0;
    //int bufSize = 0;

	/* Make sure the command belongs to us*/
    if (_IOC_TYPE(cmd) != SPI_IOCTL_BASE)
    	return -ENOTTY;
    
    switch(cmd )
    {
    case SPI_GET_SERVO_STATUS:
	//the buffer size is passed in first, then the buffer location
	//the buffer size gives us the count of bytes sent which we can
	//then deduce the word that we want to send back.  We want to only
	//send back the last status.
	/*
	bufSize = *(int*)arg;
	pBuf = (short*) *(int*)(arg + sizeof(int));
	MSG("iMinor:%d, pReadBuf:0x%x, bufSize:%d\n",iMinor,(int)pReadBuf,bufSize);
	if (bufSize > BUFSIZE)
	    bufSize = BUFSIZE;
	i = copy_to_user(pBuf,&pReadBuf[(bufSize/2) -1],sizeof(short));
    	return sizeof(short) - i;
	*/
	
	test = (pPIOD->PIO_PDSR  & AT91C_PIO_PD9)>>9;
	//printk("Pin 9: %x\n", test);
	return (test);
	break;
    case SPI_SET_SPI_MODE:
	//Set the bit mode to 8 or 16 bit, polarity,phase and baud rate
	//The calling application will have to know how to fill out this register.
	if (iMinor == 2)
	{	    	
	    controller->SPI_CSR2 = arg;//servo
	}
	else if (iMinor == 3)
	{
	    controller->SPI_CSR3 = arg;//expansion slot
	}
	return arg;
	break;
    case SPI_GET_BUFFER_SIZE:
	return BUFSIZE;
	break;
    case SPI_SERVO_RESET:
	AT91_SYS->PIOD_PER = 1 << 8; //line 8
	AT91_SYS->PIOD_OER = 1 << 8; //set it as output, we want to drive it
	
	AT91_SYS->PIOD_SODR = 1 << 8;//set it high initially;
	//disable glitch filters
	AT91_SYS->PIOD_IFDR = 1 << 8;
	//disable interrupts
	AT91_SYS->PIOD_IDR = 1 << 8;
	//disable multiple drivers
	AT91_SYS->PIOD_MDDR = 1 << 8;
	//disable internal pull-up
	AT91_SYS->PIOD_PPUDR = 1 << 8; 
	
	//here is the actual reset
	AT91_SYS->PIOD_CODR = 1 << 8;
	//give it enough time so that it will hit the reset.
	nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
	nop();nop();nop();nop();nop();nop();nop();nop();nop();nop();
	AT91_SYS->PIOD_SODR = 1 << 8;
	return 1;
	break;
    case SPI_SERVO_BUS:
	if (arg)//grabbing the bus
	{
	    if (spi_ready(iDev3)==-1)
	    {
		//printk("Bus aqcuired\n");
		bAcquiring = 1;
		spi_access_bus(iDev3);		
		bAcquiring = 0;		
		//wake_up_interruptible(&wait_queue);
	    }
	}
	else //releasing the bus
	{
	    if (spi_ready(iDev3)!=-1)
	    {
		//printk("Bus released\n");
		spi_release_bus(iDev3);			
	    }
	    /*
	    if (waitqueue_active(&wait_queue))
	    {	
		bAborted = 1;
		wake_up_interruptible(&wait_queue);
		printk(KERN_ERR "Found process on wait_queue during releasebus!\n");
	    }
	    */
	}
	break;
    case SPI_SERVO_READY:
	return spi_ready(iDev2);
	break;
    case SPI_SERVO_STATUS:
	return m_iStatus;
	break;
    default:
    	return -ENOTTY;    	
    }
    return 0;
}


/********************************************************************/
static struct file_operations spi_link_fops = {
owner:		THIS_MODULE,
read:		spi_link_read,
write:		spi_link_write,
ioctl:		spi_link_ioctl,
open:		spi_link_open,
release:	spi_link_release,
};

static int __init at91_usbspi_init(void)
{
	int res = 0;
	int status = 0;
	xil_addr = NULL;
	BYTE gpiodata;
	MSG("Module at91_spi_link init\n" );		
	readBuf = kmalloc(BUFSIZE, GFP_KERNEL);
	writeBuf = kmalloc(BUFSIZE, GFP_KERNEL);
	spi_transfer_desc = kmalloc(sizeof(struct spi_transfer_list), GFP_KERNEL);
	printk("PIOD Address %x\n", (unsigned int)pPIOD);
	printk("bcdUSB: %x\n", __constant_cpu_to_le16 (0x0200));
	printk("VENDOR NUM: %x\n", __constant_cpu_to_le16 (0x7967));
	printk("PRODUCT NUM: %x\n", __constant_cpu_to_le16 (0x3593));
	if (!spi_transfer_desc || !readBuf || !writeBuf)
		return -ENOMEM;
	memset(readBuf,0,BUFSIZE);//clear the read buffer
	printk(KERN_DEBUG "readBuf:0x%x, writeBuf:0x%x\n",(int)readBuf,(int)writeBuf);
	/*register the device with the kernel*/	
	res = register_chrdev(SPI_LINK_MAJOR,"spi_link",&spi_link_fops);
	if (res)
	{
		MSG("Can't register device spi_link with kernel.\n");
		return res;
	}
	
	//status = request_irq(AT91_PIN_PD9, at91_usbspi_interrupt, 0, "usbspi", NULL);
	if (status) 
	{
	    printk(KERN_ERR "at91_flyer_usbspi: IRQ %d request failed - status %d!\n", AT91C_ID_PIOD, status);
	    return -EBUSY;
        }
	
	// Set up PD9 for to handle interrupts
	
	//Enable register
	pPIOD->PIO_PER |= AT91C_PIO_PD9;
	 //disable glitch filters
	pPIOD->PIO_IFDR |= AT91C_PIO_PD9;
	//disable interrupts
	pPIOD->PIO_IDR |= AT91C_PIO_PD9;
	//disable multiple drivers
	pPIOD->PIO_MDDR |= AT91C_PIO_PD9;
	//enable internal pull-up
	pPIOD->PIO_PPUER |= AT91C_PIO_PD9;
	gpiodata = ReadUSBReg(rGPIO,0);
	//Initialize_USB();
	
	
	for (res=0; res <8; res++)
	{
	   
	//printk("GPIO: %x \n", gpiodata);
	   WriteUSBReg(rGPIO, 0x1,0);
	   //gpiodata = ReadUSBReg(rGPIO,0);
        }
	
	//printk("GPIO: %x \n", gpiodata);
	//Enable interrupts
	//pPIOD->PIO_IER |= AT91C_PIO_PD9;
	
	return 0;
}

static void __exit at91_usbspi_exit(void)
{
	printk( KERN_DEBUG "Module at91_spi_link exit\n" );
	WriteUSBReg(rGPIO, 0x0,0);
	if (spi_transfer_desc)
		kfree(spi_transfer_desc);
	if (readBuf)
	{
		kfree(readBuf);
	}
	if (writeBuf)
	{
		kfree(writeBuf);
	}
	//free_irq(AT91_PIN_PD9,0);
	/*unregister the device with the kernel*/	
	int res = unregister_chrdev(SPI_LINK_MAJOR,"spi_link");
	if (res)
	{
		MSG("Can't unregister device spi_link with kernel.\n");
	}
	//Disable register
	pPIOD->PIO_PDR |= AT91C_PIO_PD9;
	
}

static irqreturn_t at91_usbspi_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
   unsigned int level = pPIOD->PIO_PDSR;
   printk("level %d -- level + mask: %d\n",level, level & AT91C_PIO_PD9);
   BYTE itest1,itest2;
   // Disable the interrupt
   //pPIOD->PIO_IDR |= AT91C_PIO_PD9;
   disable_irq(AT91_PIN_PD9);
   unsigned int irqstatus = pPIOD->PIO_ISR & AT91C_PIO_PD9;
   level = pPIOD->PIO_PDSR & AT91C_PIO_PD9;
   if(!irqstatus)
   {
        pPIOD->PIO_IER |= AT91C_PIO_PD9;
	return IRQ_HANDLED;
   }
       
   if(irqstatus && level)
   {
        pPIOD->PIO_IER |= AT91C_PIO_PD9;
	return IRQ_HANDLED;
   }
   printk("Interrupt fired\n");			
       // 
       // Disable the interrupt
       //pPIOD->PIO_IDR |= AT91C_PIO_PD9;
       
       // check the appropriate registers and bits ffrom the MAX3420
  
       itest1 =  ReadUSBReg(rEPIRQ,0);	// Check the EPIRQ bits
       itest2 =  ReadUSBReg(rUSBIRQ,0);   // Check the USBIRQ bits
       printk("EPIRQ: %x   USBIRQ: %x\n",itest1, itest2); 
       
       if(itest1 & bmSUDAVIRQ) 
       {
	   WriteUSBReg(rEPIRQ, bmSUDAVIRQ,0);    // clear the SUDAV IRQ
	   printk("Do Setup\n");
	   do_Setup();
       }
        
       if(itest1 & bmIN3BAVIRQ)          // Was an EP3-IN packet just dispatched to the host?
       {
	  // do_IN3();                     // Yes--load another keystroke and arm the endpoint
	  printk("Do IN2\n");
	  do_IN2();
       }                             // NOTE: don't clear the IN3BAVIRQ bit here--loading the EP3-IN byte
       if(itest2 & bmURESIRQ)
       {
	   printk("Do bmURESIRQ\n");
	  WriteUSBReg(rUSBIRQ,bmURESIRQ,0);   // clear the IRQ
       }
       if(itest2 & bmURESDNIRQ)
       {
	  printk("Do bmURESDNIRQ\n");
	   WriteUSBReg(rUSBIRQ,bmURESDNIRQ,0);   // clear the IRQ bit
	   ENABLE_IRQS                   // ...because a bus reset clears the IE bits
       }
       if(itest2 & bmNOVBUSIRQ)
       {
	   printk("Do bmNOVBUSIRQ\n");
	   WriteUSBReg(rUSBIRQ,bmNOVBUSIRQ,0);   // clear the IRQ bit
       }
      
       while(!(pPIOD->PIO_PDSR & AT91C_PIO_PD9));
       
        //pPIOD->PIO_IER |= AT91C_PIO_PD9;
       enable_irq(AT91_PIN_PD9);
       //printk("Handling USB_SPI interrupt\n");
       //irqstatus = pPIOD->PIO_IMR;pPIOD->PIO_IER |= AT91C_PIO_PD9;
       //pPIOD->PIO_IER |= AT91C_PIO_PD9;
       //printk("USB SPI interrupt cleared..\n");
      
   return IRQ_HANDLED;
   
    
}

BYTE ReadUSBReg(BYTE reg, BYTE ackstat)
{
    int type = SPI_SYNC;
    int releasebus = 0;
    BYTE rBuf[2];
    rBuf[0] = reg+ackstat;
    rBuf[1] = 0;
    
    if (spi_ready(iDev3)==-1)
    {
	spi_access_bus(iDev3);
	releasebus = 1;
    }
    
    while(!spi_ready(iDev3));
    
    do_spi_transfer(1,rBuf,2,rBuf,2,NULL,0,NULL,0,type);
    
    if (releasebus)
    {	
	spi_release_bus(iDev3);	
    }
    
    return rBuf[1];
}

BYTE ReadBytesN(unsigned char reg, BYTE* data, int size)
{
    int type = SPI_SYNC;
    int releasebus = 0;
    
    if (spi_ready(iDev3)==-1)
    {
	spi_access_bus(iDev3);
	releasebus = 1;
    }
    
    while(!spi_ready(iDev3));
    
    do_spi_transfer(2,&reg,1,&reg,2,data,size,data,size,type);
    if (releasebus)
    {	
	spi_release_bus(iDev3);	
    }
    
    return(0);
    
}

BYTE WriteBytesN(unsigned char reg, BYTE* data, int size)
{
    int type = SPI_SYNC;
    int releasebus = 0;
    unsigned char regn = reg+2;
    
    if (spi_ready(iDev3)==-1)
    {
	spi_access_bus(iDev3);
	releasebus = 1;
    }
    
    while(!spi_ready(iDev3));
    
    do_spi_transfer(2,&regn,1,&regn,2,data,size,data,size,type);
    if (releasebus)
    {	
	spi_release_bus(iDev3);	
    }
    
    return(0);
    
}

BYTE WriteUSBReg(BYTE reg, BYTE val, BYTE ackstat)
{
    int type = SPI_SYNC;
    int releasebus = 0;
    BYTE rBuf[2];
    rBuf[0] = (reg)+2+ackstat;
    rBuf[1] = val;
    
    if (spi_ready(iDev3)==-1)
    {
	spi_access_bus(iDev3);
	releasebus = 1;
    }
    
    while(!spi_ready(iDev3));
    
    do_spi_transfer(1,rBuf,2,rBuf,2,NULL,0,NULL,0,type);
    
    if (releasebus)
    {	
	spi_release_bus(iDev3);	
    }
    
    return rBuf[0];    
}

module_init(at91_usbspi_init);
module_exit(at91_usbspi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stevan Saban 2");
MODULE_DESCRIPTION("SPI /dev interface for Atmel AT91RM9200");
