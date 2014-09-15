/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        HWmap.h
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
  *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *  Andrea Seraghiti     2.2     11/21/2011        Added ethernet support
 *  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/
/*****************************************************************************
 *								--- CLOCK FREQUENCY ---					 	 *
 ****************************************************************************/
#ifndef __MAP_H
#define __MAP_H

#define GetSystemClock()		(32000000ul)      // Hz
#define GetInstructionClock()	(GetSystemClock()/2)
#define GetPeripheralClock()	GetInstructionClock()	

// Two Flyport PRO model supported: FLYPORT_G and FLYPORT_ETH
#define FLYPORT_PRO
#define FLYPORT_G

//	Old defines compatibility
#if defined (FLYPORT_ETH)
	#define FLYPORTETH
#endif

#if (defined (FLYPORT_B)||defined(FLYPORT_G))
	#define FLYPORT
#endif

/*****************************************************************************
 *							--- WIFI MODULE MAPPING ---					 	 *
 ****************************************************************************/
#if defined (FLYPORT_G)
	#define MRF24WG
	#define FLYPORT_WF
	
#elif defined (FLYPORT_B)
	#define FLYPORT_WF
#endif

#if defined (FLYPORT_WF)
	
	#define WF_CS_TRIS			(TRISBbits.TRISB12)
	#define WF_CS_IO			(LATBbits.LATB12)
	#define WF_SDI_TRIS			(TRISGbits.TRISG8)
	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
	#define WF_SDO_TRIS			(TRISGbits.TRISG7)
	#define WF_RESET_TRIS		(TRISDbits.TRISD6)
	#define WF_RESET_IO			(LATDbits.LATD6)
	#define WF_INT_TRIS	    	(TRISGbits.TRISG9)  // INT1
	#define WF_INT_IO			(PORTGbits.RG9)
	#define WF_HIBERNATE_TRIS	(TRISDbits.TRISD7)
	#define	WF_HIBERNATE_IO		(LATDbits.LATD7)
	#define WF_INT_EDGE		    (INTCON2bits.INT1EP)
	#define WF_INT_IE			(IEC1bits.INT1IE)
	#define WF_INT_IF			(IFS1bits.INT1IF)
	
	#define WF_SSPBUF			(SPI1BUF)
	#define WF_SPISTAT			(SPI1STAT)
	#define WF_SPISTATbits		(SPI1STATbits)
	#define WF_SPICON1			(SPI1CON1)
	#define WF_SPICON1bits		(SPI1CON1bits)
	#define WF_SPICON2			(SPI1CON2)
	#define WF_SPI_IE			(IEC0bits.SPI1IE)
	//#define WF_SPI_IP			(IPC2bits.SPI1IP)
	#define WF_SPI_IF			(IFS0bits.SPI1IF)
	
	//	Onboard SPI flash mapping
	#define SPIFLASH_CS_TRIS        (TRISBbits.TRISB11)
	#define SPIFLASH_CS_IO          (LATBbits.LATB11)
	#define SPIFLASH_SCK_TRIS       (TRISBbits.TRISB1)
	#define SPIFLASH_SDI_TRIS       (TRISBbits.TRISB9)
	#define SPIFLASH_SDI_IO         (PORTBbits.RB9)
	#define SPIFLASH_SDO_TRIS       (TRISDbits.TRISD8)
	#define SPIFLASH_SPI_IF         (IFS5bits.SPI3IF)
	#define SPIFLASH_SSPBUF         (SPI3BUF)
	#define SPIFLASH_SPICON1        (SPI3CON1)
	#define SPIFLASH_SPICON1bits	(SPI3CON1bits)
	#define SPIFLASH_SPICON2        (SPI3CON2)
	#define SPIFLASH_SPISTAT        (SPI3STAT)
	#define SPIFLASH_SPISTATbits	(SPI3STATbits)
		
	#define ADC_VREF_EN_TRIS		TRISDbits.TRISD2
	#define ADC_VREF_EN_IO			LATDbits.LATD2
	
#elif defined (FLYPORTETH)

    #define ENC100_ETH_ON_TRIS              (TRISBbits.TRISB12)
    #define ENC100_ETH_ON_IO                (LATBbits.LATB12)

    #define ENC100_POR_TRIS                 ENC100_ETH_ON_TRIS
    #define ENC100_POR_IO                   ENC100_ETH_ON_IO

    #define ENC100_INTERFACE_MODE			5
    #define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
    #define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
    
    #define ENC100_BIT_BANG_PMP
    #define ENC100_INT_TRIS					(TRISBbits.TRISB2)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
	#define ENC100_INT_IO					(PORTBbits.RB2)
    
    // PSP control signal pinout
	//#define ENC100_CS_TRIS				(TRISAbits.TRISA5)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
	//#define ENC100_CS_IO					(LATAbits.LATA5)
	//#define ENC100_POR_TRIS				(TRISCbits.TRISC1)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
	//#define ENC100_POR_IO					(LATCbits.LATC1)
    #define ENC100_SO_WR_B0SEL_EN_IO 		(PORTDbits.RD4)
	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISDbits.TRISD4)
	#define ENC100_SI_RD_RW_IO				(LATDbits.LATD5)
	#define ENC100_SI_RD_RW_TRIS			(TRISDbits.TRISD5)
    #define ENC100_SCK_AL_IO				(LATBbits.LATB15)
	#define	ENC100_SCK_AL_TRIS				(TRISBbits.TRISB15)

	#define ENC100_INIT_PSP_BIT_BANG()	do{/*ADCInit(); // just made it!*/ }while(0)/*do{ANSB &= 0x7FFF; ANSG &= 0xFEFF;} while(0)*/		// RE0-RE7, RD11, RD4, RD5 (AD0-AD7, AD8, WR, RD) pins are already digital only pins.  RB15, RG8 (AL, CS) needs to be made digital only.
	#define ENC100_SET_AD_TRIS_IN()		do{((volatile unsigned char*)&TRISE)[0] = 0xFF;}while(0)
	#define ENC100_SET_AD_TRIS_OUT()	do{((volatile unsigned char*)&TRISE)[0] = 0x00; TRISDbits.TRISD11 = 0;}while(0)
	#define ENC100_GET_AD_IO()			(((volatile unsigned char*)&PORTE)[0])
	#define ENC100_SET_AD_IO(data)		do{unsigned short _wSetMacro = (data); ((volatile unsigned char*)&LATE)[0] = (unsigned char)_wSetMacro; LATDbits.LATD11 = 0; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
	#define ENC100_SET_AD_IOL(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))

    
    //	Onboard SPI flash mapping
	#define SPIFLASH_CS_TRIS        (TRISBbits.TRISB11)
	#define SPIFLASH_CS_IO          (LATBbits.LATB11)
	#define SPIFLASH_SCK_TRIS       (TRISGbits.TRISG8)
	#define SPIFLASH_SDI_TRIS       (TRISGbits.TRISG9)
	#define SPIFLASH_SDI_IO         (PORTGbits.RG9)
	#define SPIFLASH_SDO_TRIS       (TRISDbits.TRISD8)
	#define SPIFLASH_SPI_IF         (IFS5bits.SPI3IF)
	#define SPIFLASH_SSPBUF         (SPI3BUF)
	#define SPIFLASH_SPICON1        (SPI3CON1)
	#define SPIFLASH_SPICON1bits	(SPI3CON1bits)
	#define SPIFLASH_SPICON2        (SPI3CON2)
	#define SPIFLASH_SPISTAT        (SPI3STAT)
	#define SPIFLASH_SPISTATbits	(SPI3STATbits)

	#define ADC_VREF_EN_TRIS		TRISDbits.TRISD7
	#define ADC_VREF_EN_IO			LATDbits.LATD7

#endif

/*****************************************************************************
 *				--- HARDWARE MAPPING ---					 *
 ****************************************************************************/
	//	Hardware configuration
	#define HWDEFAULT	(1)
	#define	hwdefault	HWDEFAULT 	

	//	IOs
	#define p1					(1)
	#define P1					(1)
	#define p2					(2)
	#define P2					(2)
	#define p3					(3)
	#define P3					(3)
	#define p4					(4)
	#define P4					(4)	
	#define p5					(5)
	#define P5					(5)	
	#define p6					(6)
	#define P6					(6)
	#define p7					(7)
	#define P7					(7)
	#define p8					(8)
	#define P8					(8)
	#define p9					(9)
	#define P9					(9)
	#define p10					(10)
	#define P10					(10)
	#define p11					(11)
	#define P11					(11)
	#define p12					(12)
	#define P12					(12)
	#define p13					(13)
	#define P13					(13)
	#define p14					(14)
	#define P14					(14)
	#define p15					(15)
	#define P15					(15)
    #define p16					(16)
	#define P16					(16)
	#define p17					(17)
	#define P17					(17)
	#define p18					(18)
	#define P18					(18)
	#define p19					(19)
	#define P19					(19)
	#define p20					(20)
	#define P20					(20)
	#define p21					(21)
	#define P21					(21)
    #define p22					(22)
	#define P22					(22)
	#define p23					(23)
	#define P23					(23)
	#define p24					(24)
	#define P24					(24)
	#define p25					(25)
	#define P25					(25)
	#define p26					(26)
	#define P26					(26)
	#define p27					(27)
	#define P27					(27)
	#define p28					(28)
	#define P28					(28)
	#define p29					(29)
	#define P29					(29)
	#define p30					(30)
	#define P30					(30)
	#define p31					(31)
	#define P31					(31)
	#define p32					(32)
	#define P32					(32)
	#define p33					(33)
	#define P33					(33)
	#define p34					(34)
	#define P34					(34)
	#define p35					(35)
	#define P35					(35)
	#define p36					(36)
	#define P36					(36)
	#define p37					(37)
	#define P37					(37)
	#define p38					(38)
	#define P38					(38)
	#define p39					(39)
	#define P39					(39)
	
	//	ADC
#if defined (FLYPORT_WF)
	#define ADCANSB		(0x65FC)
#elif defined (FLYPORT_ETH)
	#define ADCANSB		(0x67F8)
#endif
	
	#define ADCEnable	(AD1CON1bits.ADON)
	#define ADCVref		(AD1CON2bits.VCFG)
	
#endif
