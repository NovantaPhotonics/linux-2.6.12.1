// ----------------------------------------------------------------------------
//          ATMEL Microcontroller Software Support  -  ROUSSET  -
// ----------------------------------------------------------------------------
// Copyright (c) 2004, Atmel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// -  Redistributions of source code must retain the above copyright notice,
// this list of conditions and the disclaimer below.
// -  Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the disclaimer below in the documentation and/or
// other materials provided with the distribution.
//
// Atmel's name may not be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------
// File Name           : AT91RM9200.h
// Object              : AT91RM9200 definitions
// Generated           : AT91 SW Application Group  04/16/2003 (12:30:06)
//
// ----------------------------------------------------------------------------

#ifndef AT91RM9200_H
#define AT91RM9200_H

#ifndef __ASSEMBLY__

 /* Hardware register definition */
typedef volatile unsigned int AT91_REG;

#endif	/* __ASSEMBLY__ */

// *****************************************************************************
//               PERIPHERAL ID DEFINITIONS FOR AT91RM9200
// *****************************************************************************
#define AT91C_ID_FIQ    ( 0) // Advanced Interrupt Controller (FIQ)
#define AT91C_ID_SYS    ( 1) // System Peripheral
#define AT91C_ID_PIOA   ( 2) // Parallel IO Controller A
#define AT91C_ID_PIOB   ( 3) // Parallel IO Controller B
#define AT91C_ID_PIOC   ( 4) // Parallel IO Controller C
#define AT91C_ID_PIOD   ( 5) // Parallel IO Controller D
#define AT91C_ID_US0    ( 6) // USART 0
#define AT91C_ID_US1    ( 7) // USART 1
#define AT91C_ID_US2    ( 8) // USART 2
#define AT91C_ID_US3    ( 9) // USART 3
#define AT91C_ID_MCI    (10) // Multimedia Card Interface
#define AT91C_ID_UDP    (11) // USB Device Port
#define AT91C_ID_TWI    (12) // Two-Wire Interface
#define AT91C_ID_SPI    (13) // Serial Peripheral Interface
#define AT91C_ID_SSC0   (14) // Serial Synchronous Controller 0
#define AT91C_ID_SSC1   (15) // Serial Synchronous Controller 1
#define AT91C_ID_SSC2   (16) // Serial Synchronous Controller 2
#define AT91C_ID_TC0    (17) // Timer Counter 0
#define AT91C_ID_TC1    (18) // Timer Counter 1
#define AT91C_ID_TC2    (19) // Timer Counter 2
#define AT91C_ID_TC3    (20) // Timer Counter 3
#define AT91C_ID_TC4    (21) // Timer Counter 4
#define AT91C_ID_TC5    (22) // Timer Counter 5
#define AT91C_ID_UHP    (23) // USB Host port
#define AT91C_ID_EMAC   (24) // Ethernet MAC
#define AT91C_ID_IRQ0   (25) // Advanced Interrupt Controller (IRQ0)
#define AT91C_ID_IRQ1   (26) // Advanced Interrupt Controller (IRQ1)
#define AT91C_ID_IRQ2   (27) // Advanced Interrupt Controller (IRQ2)
#define AT91C_ID_IRQ3   (28) // Advanced Interrupt Controller (IRQ3)
#define AT91C_ID_IRQ4   (29) // Advanced Interrupt Controller (IRQ4)
#define AT91C_ID_IRQ5   (30) // Advanced Interrupt Controller (IRQ5)
#define AT91C_ID_IRQ6   (31) // Advanced Interrupt Controller (IRQ6)


// *****************************************************************************
//               BASE ADDRESS DEFINITIONS FOR AT91RM9200
// *****************************************************************************
#define AT91C_BASE_SYS		(0xFFFFF000) // (SYS) Base Address
#define AT91C_BASE_PDC_SPI	(0xFFFE0100) // (PDC_SPI) Base Address
#define AT91C_BASE_SPI		(0xFFFE0000) // (SPI) Base Address
#define AT91C_BASE_PDC_SSC2	(0xFFFD8100) // (PDC_SSC2) Base Address
#define AT91C_BASE_SSC2		(0xFFFD8000) // (SSC2) Base Address
#define AT91C_BASE_PDC_SSC1	(0xFFFD4100) // (PDC_SSC1) Base Address
#define AT91C_BASE_SSC1		(0xFFFD4000) // (SSC1) Base Address
#define AT91C_BASE_PDC_SSC0	(0xFFFD0100) // (PDC_SSC0) Base Address
#define AT91C_BASE_SSC0		(0xFFFD0000) // (SSC0) Base Address
#define AT91C_BASE_PDC_US3	(0xFFFCC100) // (PDC_US3) Base Address
#define AT91C_BASE_US3		(0xFFFCC000) // (US3) Base Address
#define AT91C_BASE_PDC_US2	(0xFFFC8100) // (PDC_US2) Base Address
#define AT91C_BASE_US2		(0xFFFC8000) // (US2) Base Address
#define AT91C_BASE_PDC_US1	(0xFFFC4100) // (PDC_US1) Base Address
#define AT91C_BASE_US1		(0xFFFC4000) // (US1) Base Address
#define AT91C_BASE_PDC_US0	(0xFFFC0100) // (PDC_US0) Base Address
#define AT91C_BASE_US0		(0xFFFC0000) // (US0) Base Address
#define AT91C_BASE_EMAC		(0xFFFBC000) // (EMAC) Base Address
#define AT91C_BASE_PDC_MCI	(0xFFFB4100) // (PDC_MCI) Base Address
#define AT91C_BASE_TWI		(0xFFFB8000) // (TWI) Base Address
#define AT91C_BASE_MCI		(0xFFFB4000) // (MCI) Base Address
#define AT91C_BASE_UDP		(0xFFFB0000) // (UDP) Base Address
#define AT91C_BASE_TC5		(0xFFFA4080) // (TC5) Base Address
#define AT91C_BASE_TC4		(0xFFFA4040) // (TC4) Base Address
#define AT91C_BASE_TC3		(0xFFFA4000) // (TC3) Base Address
#define AT91C_BASE_TCB1		(0xFFFA4000) // (TCB1) Base Address
#define AT91C_BASE_TC2		(0xFFFA0080) // (TC2) Base Address
#define AT91C_BASE_TC1		(0xFFFA0040) // (TC1) Base Address
#define AT91C_BASE_TC0		(0xFFFA0000) // (TC0) Base Address
#define AT91C_BASE_TCB0		(0xFFFA0000) // (TCB0) Base Address


// *****************************************************************************
//               PIO DEFINITIONS FOR AT91RM9200
// *****************************************************************************
#define AT91C_PIO_PA0		(1 <<  0)
#define AT91C_PA0_MISO		(AT91C_PIO_PA0) //  SPI Master In Slave
#define AT91C_PA0_PCK3		(AT91C_PIO_PA0) //  PMC Programmable Clock Output 3
#define AT91C_PIO_PA1		(1 <<  1)
#define AT91C_PA1_MOSI		(AT91C_PIO_PA1) //  SPI Master Out Slave
#define AT91C_PA1_PCK0		(AT91C_PIO_PA1) //  PMC Programmable Clock Output 0
#define AT91C_PIO_PA2		(1 <<  2)
#define AT91C_PA2_SPCK		(AT91C_PIO_PA2) //  SPI Serial Clock
#define AT91C_PA2_IRQ4		(AT91C_PIO_PA2) //  AIC Interrupt Input 4
#define AT91C_PIO_PA3		(1 <<  3)
#define AT91C_PA3_NPCS0		(AT91C_PIO_PA3) //  SPI Peripheral Chip Select 0
#define AT91C_PA3_IRQ5		(AT91C_PIO_PA3) //  AIC Interrupt Input 5
#define AT91C_PIO_PA4		(1 <<  4)
#define AT91C_PA4_NPCS1		(AT91C_PIO_PA4) //  SPI Peripheral Chip Select 1
#define AT91C_PA4_PCK1		(AT91C_PIO_PA4) //  PMC Programmable Clock Output 1
#define AT91C_PIO_PA5		(1 <<  5)
#define AT91C_PA5_NPCS2		(AT91C_PIO_PA5) //  SPI Peripheral Chip Select 2
#define AT91C_PA5_TXD3		(AT91C_PIO_PA5) //  USART 3 Transmit Data
#define AT91C_PIO_PA6		(1 <<  6)
#define AT91C_PA6_NPCS3		(AT91C_PIO_PA6) //  SPI Peripheral Chip Select 3
#define AT91C_PA6_RXD3		(AT91C_PIO_PA6) //  USART 3 Receive Data
#define AT91C_PIO_PA7		(1 <<  7)
#define AT91C_PA7_ETXCK_EREFCK	(AT91C_PIO_PA7) //  Ethernet MAC Transmit Clock/Reference Clock
#define AT91C_PA7_PCK2		(AT91C_PIO_PA7) //  PMC Programmable Clock 2
#define AT91C_PIO_PA8		(1 <<  8)
#define AT91C_PA8_ETXEN		(AT91C_PIO_PA8) //  Ethernet MAC Transmit Enable
#define AT91C_PA8_MCCDB		(AT91C_PIO_PA8) //  Multimedia Card B Command
#define AT91C_PIO_PA9		(1 <<  9)
#define AT91C_PA9_ETX0		(AT91C_PIO_PA9) //  Ethernet MAC Transmit Data 0
#define AT91C_PA9_MCDB0		(AT91C_PIO_PA9) //  Multimedia Card B Data 0
#define AT91C_PIO_PA10		(1 << 10)
#define AT91C_PA10_ETX1		(AT91C_PIO_PA10) //  Ethernet MAC Transmit Data 1
#define AT91C_PA10_MCDB1	(AT91C_PIO_PA10) //  Multimedia Card B Data 1
#define AT91C_PIO_PA11		(1 << 11)
#define AT91C_PA11_ECRS_ECRSDV	(AT91C_PIO_PA11) //  Ethernet MAC Carrier Sense/Carrier Sense and Data Valid
#define AT91C_PA11_MCDB2	(AT91C_PIO_PA11) //  Multimedia Card B Data 2
#define AT91C_PIO_PA12		(1 << 12)
#define AT91C_PA12_ERX0		(AT91C_PIO_PA12) //  Ethernet MAC Receive Data 0
#define AT91C_PA12_MCDB3	(AT91C_PIO_PA12) //  Multimedia Card B Data 3
#define AT91C_PIO_PA13 		(1 << 13)
#define AT91C_PA13_ERX1		(AT91C_PIO_PA13) //  Ethernet MAC Receive Data 1
#define AT91C_PA13_TCLK0	(AT91C_PIO_PA13) //  Timer Counter 0 external clock input
#define AT91C_PIO_PA14		(1 << 14)
#define AT91C_PA14_ERXER	(AT91C_PIO_PA14) //  Ethernet MAC Receive Error
#define AT91C_PA14_TCLK1	(AT91C_PIO_PA14) //  Timer Counter 1 external clock input
#define AT91C_PIO_PA15		(1 << 15)
#define AT91C_PA15_EMDC		(AT91C_PIO_PA15) //  Ethernet MAC Management Data Clock
#define AT91C_PA15_TCLK2	(AT91C_PIO_PA15) //  Timer Counter 2 external clock input
#define AT91C_PIO_PA16		(1 << 16)
#define AT91C_PA16_EMDIO	(AT91C_PIO_PA16) //  Ethernet MAC Management Data Input/Output
#define AT91C_PA16_IRQ6		(AT91C_PIO_PA16) //  AIC Interrupt input 6
#define AT91C_PIO_PA17		(1 << 17)
#define AT91C_PA17_TXD0		(AT91C_PIO_PA17) //  USART 0 Transmit Data
#define AT91C_PA17_TIOA0	(AT91C_PIO_PA17) //  Timer Counter 0 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PA18		(1 << 18)
#define AT91C_PA18_RXD0		(AT91C_PIO_PA18) //  USART 0 Receive Data
#define AT91C_PA18_TIOB0	(AT91C_PIO_PA18) //  Timer Counter 0 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PA19		(1 << 19)
#define AT91C_PA19_SCK0		(AT91C_PIO_PA19) //  USART 0 Serial Clock
#define AT91C_PA19_TIOA1	(AT91C_PIO_PA19) //  Timer Counter 1 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PA20		(1 << 20)
#define AT91C_PA20_CTS0		(AT91C_PIO_PA20) //  USART 0 Clear To Send
#define AT91C_PA20_TIOB1	(AT91C_PIO_PA20) //  Timer Counter 1 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PA21		(1 << 21)
#define AT91C_PA21_RTS0		(AT91C_PIO_PA21) //  USART 0 Ready To Send
#define AT91C_PA21_TIOA2	(AT91C_PIO_PA21) //  Timer Counter 2 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PA22		(1 << 22)
#define AT91C_PA22_RXD2		(AT91C_PIO_PA22) //  USART 2 Receive Data
#define AT91C_PA22_TIOB2	(AT91C_PIO_PA22) //  Timer Counter 2 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PA23		(1 << 23)
#define AT91C_PA23_TXD2		(AT91C_PIO_PA23) //  USART 2 Transmit Data
#define AT91C_PA23_IRQ3		(AT91C_PIO_PA23) //  Interrupt input 3
#define AT91C_PIO_PA24		(1 << 24)
#define AT91C_PA24_SCK2		(AT91C_PIO_PA24) //  USART 2 Serial Clock
#define AT91C_PA24_PCK1		(AT91C_PIO_PA24) //  PMC Programmable Clock Output 1
#define AT91C_PIO_PA25		(1 << 25)
#define AT91C_PA25_TWD		(AT91C_PIO_PA25) //  TWI Two-wire Serial Data
#define AT91C_PA25_IRQ2		(AT91C_PIO_PA25) //  Interrupt input 2
#define AT91C_PIO_PA26		(1 << 26)
#define AT91C_PA26_TWCK		(AT91C_PIO_PA26) //  TWI Two-wire Serial Clock
#define AT91C_PA26_IRQ1		(AT91C_PIO_PA26) //  Interrupt input 1
#define AT91C_PIO_PA27		(1 << 27)
#define AT91C_PA27_MCCK		(AT91C_PIO_PA27) //  Multimedia Card Clock
#define AT91C_PA27_TCLK3	(AT91C_PIO_PA27) //  Timer Counter 3 External Clock Input
#define AT91C_PIO_PA28		(1 << 28)
#define AT91C_PA28_MCCDA	(AT91C_PIO_PA28) //  Multimedia Card A Command
#define AT91C_PA28_TCLK4	(AT91C_PIO_PA28) //  Timer Counter 4 external Clock Input
#define AT91C_PIO_PA29		(1 << 29)
#define AT91C_PA29_MCDA0	(AT91C_PIO_PA29) //  Multimedia Card A Data 0
#define AT91C_PA29_TCLK5	(AT91C_PIO_PA29) //  Timer Counter 5 external clock input
#define AT91C_PIO_PA30		(1 << 30)
#define AT91C_PA30_DRXD		(AT91C_PIO_PA30) //  DBGU Debug Receive Data
#define AT91C_PA30_CTS2		(AT91C_PIO_PA30) //  USART 2 Clear To Send
#define AT91C_PIO_PA31		(1 << 31)
#define AT91C_PA31_DTXD		(AT91C_PIO_PA31) //  DBGU Debug Transmit Data
#define AT91C_PA31_RTS2		(AT91C_PIO_PA31) //  USART 2 Ready To Send

#define AT91C_PIO_PB0		(1 <<  0)
#define AT91C_PB0_TF0		(AT91C_PIO_PB0) //  SSC Transmit Frame Sync 0
#define AT91C_PB0_RTS3		(AT91C_PIO_PB0) //  USART 3 Ready To Send
#define AT91C_PIO_PB1		(1 <<  1)
#define AT91C_PB1_TK0		(AT91C_PIO_PB1) //  SSC Transmit Clock 0
#define AT91C_PB1_CTS3		(AT91C_PIO_PB1) //  USART 3 Clear To Send
#define AT91C_PIO_PB2		(1 <<  2)
#define AT91C_PB2_TD0		(AT91C_PIO_PB2) //  SSC Transmit data
#define AT91C_PB2_SCK3		(AT91C_PIO_PB2) //  USART 3 Serial Clock
#define AT91C_PIO_PB3		(1 <<  3)
#define AT91C_PB3_RD0		(AT91C_PIO_PB3) //  SSC Receive Data
#define AT91C_PB3_MCDA1		(AT91C_PIO_PB3) //  Multimedia Card A Data 1
#define AT91C_PIO_PB4		(1 <<  4)
#define AT91C_PB4_RK0		(AT91C_PIO_PB4) //  SSC Receive Clock
#define AT91C_PB4_MCDA2		(AT91C_PIO_PB4) //  Multimedia Card A Data 2
#define AT91C_PIO_PB5		(1 <<  5)
#define AT91C_PB5_RF0		(AT91C_PIO_PB5) //  SSC Receive Frame Sync 0
#define AT91C_PB5_MCDA3		(AT91C_PIO_PB5) //  Multimedia Card A Data 3
#define AT91C_PIO_PB6		(1 <<  6)
#define AT91C_PB6_TF1		(AT91C_PIO_PB6) //  SSC Transmit Frame Sync 1
#define AT91C_PB6_TIOA3		(AT91C_PIO_PB6) //  Timer Counter 4 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PB7		(1 <<  7)
#define AT91C_PB7_TK1		(AT91C_PIO_PB7) //  SSC Transmit Clock 1
#define AT91C_PB7_TIOB3		(AT91C_PIO_PB7) //  Timer Counter 3 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PB8		(1 <<  8)
#define AT91C_PB8_TD1		(AT91C_PIO_PB8) //  SSC Transmit Data 1
#define AT91C_PB8_TIOA4		(AT91C_PIO_PB8) //  Timer Counter 4 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PB9		(1 <<  9)
#define AT91C_PB9_RD1		(AT91C_PIO_PB9) //  SSC Receive Data 1
#define AT91C_PB9_TIOB4		(AT91C_PIO_PB9) //  Timer Counter 4 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PB10		(1 << 10)
#define AT91C_PB10_RK1		(AT91C_PIO_PB10) //  SSC Receive Clock 1
#define AT91C_PB10_TIOA5	(AT91C_PIO_PB10) //  Timer Counter 5 Multipurpose Timer I/O Pin A
#define AT91C_PIO_PB11		(1 << 11)
#define AT91C_PB11_RF1		(AT91C_PIO_PB11) //  SSC Receive Frame Sync 1
#define AT91C_PB11_TIOB5	(AT91C_PIO_PB11) //  Timer Counter 5 Multipurpose Timer I/O Pin B
#define AT91C_PIO_PB12		(1 << 12)
#define AT91C_PB12_TF2		(AT91C_PIO_PB12) //  SSC Transmit Frame Sync 2
#define AT91C_PB12_ETX2		(AT91C_PIO_PB12) //  Ethernet MAC Transmit Data 2
#define AT91C_PIO_PB13		(1 << 13)
#define AT91C_PB13_TK2		(AT91C_PIO_PB13) //  SSC Transmit Clock 2
#define AT91C_PB13_ETX3		(AT91C_PIO_PB13) //  Ethernet MAC Transmit Data 3
#define AT91C_PIO_PB14		(1 << 14)
#define AT91C_PB14_TD2		(AT91C_PIO_PB14) //  SSC Transmit Data 2
#define AT91C_PB14_ETXER	(AT91C_PIO_PB14) //  Ethernet MAC Transmikt Coding Error
#define AT91C_PIO_PB15		(1 << 15)
#define AT91C_PB15_RD2		(AT91C_PIO_PB15) //  SSC Receive Data 2
#define AT91C_PB15_ERX2		(AT91C_PIO_PB15) //  Ethernet MAC Receive Data 2
#define AT91C_PIO_PB16		(1 << 16)
#define AT91C_PB16_RK2		(AT91C_PIO_PB16) //  SSC Receive Clock 2
#define AT91C_PB16_ERX3		(AT91C_PIO_PB16) //  Ethernet MAC Receive Data 3
#define AT91C_PIO_PB17		(1 << 17)
#define AT91C_PB17_RF2		(AT91C_PIO_PB17) //  SSC Receive Frame Sync 2
#define AT91C_PB17_ERXDV	(AT91C_PIO_PB17) //  Ethernet MAC Receive Data Valid
#define AT91C_PIO_PB18		(1 << 18)
#define AT91C_PB18_RI1		(AT91C_PIO_PB18) //  USART 1 Ring Indicator
#define AT91C_PB18_ECOL		(AT91C_PIO_PB18) //  Ethernet MAC Collision Detected
#define AT91C_PIO_PB19		(1 << 19)
#define AT91C_PB19_DTR1		(AT91C_PIO_PB19) //  USART 1 Data Terminal ready
#define AT91C_PB19_ERXCK	(AT91C_PIO_PB19) //  Ethernet MAC Receive Clock
#define AT91C_PIO_PB20		(1 << 20)
#define AT91C_PB20_TXD1		(AT91C_PIO_PB20) //  USART 1 Transmit Data
#define AT91C_PIO_PB21		(1 << 21)
#define AT91C_PB21_RXD1		(AT91C_PIO_PB21) //  USART 1 Receive Data
#define AT91C_PIO_PB22		(1 << 22)
#define AT91C_PB22_SCK1		(AT91C_PIO_PB22) //  USART 1 Serial Clock
#define AT91C_PIO_PB23		(1 << 23)
#define AT91C_PB23_DCD1		(AT91C_PIO_PB23) //  USART 1 Data Carrier Detect
#define AT91C_PIO_PB24		(1 << 24)
#define AT91C_PB24_CTS1		(AT91C_PIO_PB24) //  USART 1 Clear To Send
#define AT91C_PIO_PB25		(1 << 25)
#define AT91C_PB25_DSR1		(AT91C_PIO_PB25) //  USART 1 Data Set ready
#define AT91C_PB25_EF100	(AT91C_PIO_PB25) //  Ethernet MAC Force 100 Mbits/sec
#define AT91C_PIO_PB26		(1 << 26)
#define AT91C_PB26_RTS1		(AT91C_PIO_PB26) //  USART 1 Ready To Send
#define AT91C_PIO_PB27		(1 << 27)
#define AT91C_PB27_PCK0		(AT91C_PIO_PB27) //  PMC Programmable Clock Output 0
#define AT91C_PIO_PB28		(1 << 28)
#define AT91C_PB28_FIQ		(AT91C_PIO_PB28) //  AIC Fast Interrupt Input
#define AT91C_PIO_PB29		(1 << 29)
#define AT91C_PB29_IRQ0		(AT91C_PIO_PB29) //  Interrupt input 0

#define AT91C_PIO_PC0		(1 <<  0)
#define AT91C_PC0_BFCK		(AT91C_PIO_PC0) //  Burst Flash Clock
#define AT91C_PIO_PC1		(1 <<  1)
#define AT91C_PC1_BFRDY_SMOE	(AT91C_PIO_PC1) //  Burst Flash Ready
#define AT91C_PIO_PC2		(1 <<  2)
#define AT91C_PC2_BFAVD		(AT91C_PIO_PC2) //  Burst Flash Address Valid
#define AT91C_PIO_PC3		(1 <<  3)
#define AT91C_PC3_BFBAA_SMWE	(AT91C_PIO_PC3) //  Burst Flash Address Advance / SmartMedia Write Enable
#define AT91C_PIO_PC4		(1 <<  4)
#define AT91C_PC4_BFOE		(AT91C_PIO_PC4) //  Burst Flash Output Enable
#define AT91C_PIO_PC5		(1 <<  5)
#define AT91C_PC5_BFWE		(AT91C_PIO_PC5) //  Burst Flash Write Enable
#define AT91C_PIO_PC6		(1 <<  6)
#define AT91C_PC6_NWAIT		(AT91C_PIO_PC6) //  NWAIT
#define AT91C_PIO_PC7		(1 <<  7)
#define AT91C_PC7_A23		(AT91C_PIO_PC7) //  Address Bus[23]
#define AT91C_PIO_PC8		(1 <<  8)
#define AT91C_PC8_A24		(AT91C_PIO_PC8) //  Address Bus[24]
#define AT91C_PIO_PC9		(1 <<  9)
#define AT91C_PC9_A25_CFRNW	(AT91C_PIO_PC9) //  Address Bus[25] /  Compact Flash Read Not Write
#define AT91C_PIO_PC10		(1 << 10)
#define AT91C_PC10_NCS4_CFCS	(AT91C_PIO_PC10) //  Compact Flash Chip Select
#define AT91C_PIO_PC11		(1 << 11)
#define AT91C_PC11_NCS5_CFCE1	(AT91C_PIO_PC11) //  Chip Select 5 / Compact Flash Chip Enable 1
#define AT91C_PIO_PC12		(1 << 12)
#define AT91C_PC12_NCS6_CFCE2	(AT91C_PIO_PC12) //  Chip Select 6 / Compact Flash Chip Enable 2
#define AT91C_PIO_PC13		(1 << 13)
#define AT91C_PC13_NCS7		(AT91C_PIO_PC13) //  Chip Select 7
#define AT91C_PIO_PC14		(1 << 14)
#define AT91C_PIO_PC15		(1 << 15)
#define AT91C_PIO_PC16		(1 << 16)
#define AT91C_PC16_D16		(AT91C_PIO_PC16) //  Data Bus [16]
#define AT91C_PIO_PC17		(1 << 17)
#define AT91C_PC17_D17		(AT91C_PIO_PC17) //  Data Bus [17]
#define AT91C_PIO_PC18		(1 << 18)
#define AT91C_PC18_D18		(AT91C_PIO_PC18) //  Data Bus [18]
#define AT91C_PIO_PC19		(1 << 19)
#define AT91C_PC19_D19		(AT91C_PIO_PC19) //  Data Bus [19]
#define AT91C_PIO_PC20		(1 << 20)
#define AT91C_PC20_D20		(AT91C_PIO_PC20) //  Data Bus [20]
#define AT91C_PIO_PC21		(1 << 21)
#define AT91C_PC21_D21		(AT91C_PIO_PC21) //  Data Bus [21]
#define AT91C_PIO_PC22		(1 << 22)
#define AT91C_PC22_D22		(AT91C_PIO_PC22) //  Data Bus [22]
#define AT91C_PIO_PC23		(1 << 23)
#define AT91C_PC23_D23		(AT91C_PIO_PC23) //  Data Bus [23]
#define AT91C_PIO_PC24		(1 << 24)
#define AT91C_PC24_D24		(AT91C_PIO_PC24) //  Data Bus [24]
#define AT91C_PIO_PC25		(1 << 25)
#define AT91C_PC25_D25		(AT91C_PIO_PC25) //  Data Bus [25]
#define AT91C_PIO_PC26		(1 << 26)
#define AT91C_PC26_D26		(AT91C_PIO_PC26) //  Data Bus [26]
#define AT91C_PIO_PC27		(1 << 27)
#define AT91C_PC27_D27		(AT91C_PIO_PC27) //  Data Bus [27]
#define AT91C_PIO_PC28		(1 << 28)
#define AT91C_PC28_D28		(AT91C_PIO_PC28) //  Data Bus [28]
#define AT91C_PIO_PC29		(1 << 29)
#define AT91C_PC29_D29		(AT91C_PIO_PC29) //  Data Bus [29]
#define AT91C_PIO_PC30		(1 << 30)
#define AT91C_PC30_D30		(AT91C_PIO_PC30) //  Data Bus [30]
#define AT91C_PIO_PC31		(1 << 31)
#define AT91C_PC31_D31		(AT91C_PIO_PC31) //  Data Bus [31]

#define AT91C_PIO_PD0		(1 <<  0)
#define AT91C_PD0_ETX0		(AT91C_PIO_PD0) //  Ethernet MAC Transmit Data 0
#define AT91C_PIO_PD1		(1 <<  1)
#define AT91C_PD1_ETX1		(AT91C_PIO_PD1) //  Ethernet MAC Transmit Data 1
#define AT91C_PIO_PD2		(1 <<  2)
#define AT91C_PD2_ETX2		(AT91C_PIO_PD2) //  Ethernet MAC Transmit Data 2
#define AT91C_PIO_PD3		(1 <<  3)
#define AT91C_PD3_ETX3		(AT91C_PIO_PD3) //  Ethernet MAC Transmit Data 3
#define AT91C_PIO_PD4		(1 <<  4)
#define AT91C_PD4_ETXEN		(AT91C_PIO_PD4) //  Ethernet MAC Transmit Enable
#define AT91C_PIO_PD5		(1 <<  5)
#define AT91C_PD5_ETXER		(AT91C_PIO_PD5) //  Ethernet MAC Transmit Coding Error
#define AT91C_PIO_PD6		(1 <<  6)
#define AT91C_PD6_DTXD		(AT91C_PIO_PD6) //  DBGU Debug Transmit Data
#define AT91C_PIO_PD7		(1 <<  7)
#define AT91C_PD7_PCK0		(AT91C_PIO_PD7) //  PMC Programmable Clock Output 0
#define AT91C_PD7_TSYNC		(AT91C_PIO_PD7) //  ETM Synchronization signal
#define AT91C_PIO_PD8		(1 <<  8)
#define AT91C_PD8_PCK1		(AT91C_PIO_PD8) //  PMC Programmable Clock Output 1
#define AT91C_PD8_TCLK		(AT91C_PIO_PD8) //  ETM Trace Clock signal
#define AT91C_PIO_PD9		(1 <<  9)
#define AT91C_PD9_PCK2		(AT91C_PIO_PD9) //  PMC Programmable Clock 2
#define AT91C_PD9_TPS0		(AT91C_PIO_PD9) //  ETM ARM9 pipeline status 0
#define AT91C_PIO_PD10		(1 << 10)
#define AT91C_PD10_PCK3		(AT91C_PIO_PD10) //  PMC Programmable Clock Output 3
#define AT91C_PD10_TPS1		(AT91C_PIO_PD10) //  ETM ARM9 pipeline status 1
#define AT91C_PIO_PD11		(1 << 11)
#define AT91C_PD11_TPS2		(AT91C_PIO_PD11) //  ETM ARM9 pipeline status 2
#define AT91C_PIO_PD12		(1 << 12)
#define AT91C_PD12_TPK0		(AT91C_PIO_PD12) //  ETM Trace Packet 0
#define AT91C_PIO_PD13		(1 << 13)
#define AT91C_PD13_TPK1		(AT91C_PIO_PD13) //  ETM Trace Packet 1
#define AT91C_PIO_PD14		(1 << 14)
#define AT91C_PD14_TPK2		(AT91C_PIO_PD14) //  ETM Trace Packet 2
#define AT91C_PIO_PD15		(1 << 15)
#define AT91C_PD15_TD0		(AT91C_PIO_PD15) //  SSC Transmit data
#define AT91C_PD15_TPK3		(AT91C_PIO_PD15) //  ETM Trace Packet 3
#define AT91C_PIO_PD16		(1 << 16)
#define AT91C_PD16_TD1		(AT91C_PIO_PD16) //  SSC Transmit Data 1
#define AT91C_PD16_TPK4		(AT91C_PIO_PD16) //  ETM Trace Packet 4
#define AT91C_PIO_PD17		(1 << 17)
#define AT91C_PD17_TD2		(AT91C_PIO_PD17) //  SSC Transmit Data 2
#define AT91C_PD17_TPK5		(AT91C_PIO_PD17) //  ETM Trace Packet 5
#define AT91C_PIO_PD18		(1 << 18)
#define AT91C_PD18_NPCS1	(AT91C_PIO_PD18) //  SPI Peripheral Chip Select 1
#define AT91C_PD18_TPK6		(AT91C_PIO_PD18) //  ETM Trace Packet 6
#define AT91C_PIO_PD19		(1 << 19)
#define AT91C_PD19_NPCS2	(AT91C_PIO_PD19) //  SPI Peripheral Chip Select 2
#define AT91C_PD19_TPK7		(AT91C_PIO_PD19) //  ETM Trace Packet 7
#define AT91C_PIO_PD20		(1 << 20)
#define AT91C_PD20_NPCS3	(AT91C_PIO_PD20) //  SPI Peripheral Chip Select 3
#define AT91C_PD20_TPK8		(AT91C_PIO_PD20) //  ETM Trace Packet 8
#define AT91C_PIO_PD21		(1 << 21)
#define AT91C_PD21_RTS0		(AT91C_PIO_PD21) //  Usart 0 Ready To Send
#define AT91C_PD21_TPK9		(AT91C_PIO_PD21) //  ETM Trace Packet 9
#define AT91C_PIO_PD22		(1 << 22)
#define AT91C_PD22_RTS1		(AT91C_PIO_PD22) //  Usart 0 Ready To Send
#define AT91C_PD22_TPK10	(AT91C_PIO_PD22) //  ETM Trace Packet 10
#define AT91C_PIO_PD23		(1 << 23)
#define AT91C_PD23_RTS2		(AT91C_PIO_PD23) //  USART 2 Ready To Send
#define AT91C_PD23_TPK11	(AT91C_PIO_PD23) //  ETM Trace Packet 11
#define AT91C_PIO_PD24		(1 << 24)
#define AT91C_PD24_RTS3		(AT91C_PIO_PD24) //  USART 3 Ready To Send
#define AT91C_PD24_TPK12	(AT91C_PIO_PD24) //  ETM Trace Packet 12
#define AT91C_PIO_PD25		(1 << 25)
#define AT91C_PD25_DTR1		(AT91C_PIO_PD25) //  USART 1 Data Terminal ready
#define AT91C_PD25_TPK13	(AT91C_PIO_PD25) //  ETM Trace Packet 13
#define AT91C_PIO_PD26		(1 << 26)
#define AT91C_PD26_TPK14	(AT91C_PIO_PD26) //  ETM Trace Packet 14
#define AT91C_PIO_PD27		(1 << 27)
#define AT91C_PD27_TPK15	(AT91C_PIO_PD27) //  ETM Trace Packet 15

#endif