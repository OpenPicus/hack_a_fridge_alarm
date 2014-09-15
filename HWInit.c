
/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        HWlib.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *  Stefano Cappello 							   Added I2C + support in PWM
 *  Andrea Seraghiti     2.2     11/21/2011        Added ethernet support
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
 
 

#include "HWlib.h"
#include "p24FJ256GB206.h"
#include "SPIFlash.h"

extern BOOL TimerOn[5];
extern int __C30_UART;
/****************************************************************************
  Function 		static void HWInit(int conf)

  Description 	This routine initializes the hardware.

  Precondition	None

  Parameters    None - None

  Returns	    None

  Remarks 		None
 ****************************************************************************/
void HWInit(int conf)
{	
	TimerOn[0] = TRUE;
	TimerOn[4] = TRUE;
	__C30_UART = 1;

    // D+/D- pins used as Digital I/Os
    #ifndef FLY_USB_ENABLED
    U1CNFG2bits.UTRDIS = 1;
    #endif

	if (conf == HWDEFAULT)
	{
		// Init all Analog channels as digital
		ANSB = 0x00;
		ANSC = 0x00;
		ANSD = 0x00;
		ANSF = 0x00;
		ANSG = 0x00;
		
//	ADC initialization	
		ADCInit();
		// ADC 0, 1, 2, 3 channels enabled by default
		ADCDetach(4);
		ADCDetach(5);
		ADCDetach(6);
		ADCDetach(7);
		ADCDetach(8);
		ADCDetach(9);
	
// Set all pins as input by default
		int ind;
		for(ind = p1; ind < p39; ind++)
			IOInit(ind, in);
	
	// Unlock registers to PPS configuration
	__builtin_write_OSCCONL(OSCCON & 0xBF);

    #if defined (FLYPORT_WF)
		WF_CS_IO = 1;
		WF_CS_TRIS = 0;

		// PPS configuration
		//Configure SPI1 PPS pins for WiFi
		RPOR10bits.RP21R = 8;					// Assign RP21 to SCK1 (output)
		RPOR9bits.RP19R = 7;					// Assign RP19 to SDO1 (output)
		RPINR20bits.SDI1R = 26;					// Assign RP26 to SDI1 (input)
	
		// Configure INT1 PPS pin 
		RPINR0bits.INT1R = 27;					// Assign RP27 to INT1 (input)
		
		// Configure SPI3 PPS pins for flash
		SPIFLASH_CS_TRIS = 0;
		SPIFLASH_SCK_TRIS = 0;
		SPIFLASH_SDO_TRIS = 0;
		SPIFLASH_SDI_TRIS = 1;
		RPOR0bits.RP1R = 33;					// Assign RP1 to SCK3 (opcode 33) (output)
		RPOR1bits.RP2R = 32;					// Assign RP2 to SDO3 (opcode 32) (output)
		RPINR28bits.SDI3R = 9;					// Assign RP9 to SDI3 (input)
	
		// Configure UART 1 pins
		RPINR18bits.U1RXR = 25;					// Assign RP25 to U1RX (input)
		RPOR14bits.RP29R = 3;					// Assign RP29 to U1TX (output)
		
    #elif defined (FLYPORT_ETH)

    	// Configure SPI3 PPS pins for flash
		SPIFLASH_CS_TRIS = 0;
		SPIFLASH_SCK_TRIS = 0;
		SPIFLASH_SDO_TRIS = 0;
		SPIFLASH_SDI_TRIS = 1;
    
		// Configure SPI3 PPS pins for flash
        RPOR9bits.RP19R = 33;					// Assign RP19 to SCK3 (opcode 33) (output)
		RPOR1bits.RP2R = 32;					// Assign RP2 to SDO3 (opcode 32) (output)
		RPINR28bits.SDI3R = 27;					// Assign RP27 to SDI3 (input)
        
        // Configure UART 1 pins
		RPINR18bits.U1RXR = 23;					// Assign RP23 to U1RX (input)
		RPOR0bits.RP1R = 3;                     // Assign RP29 to U1TX (output)

        /* Used the defines ENC100_POR_TRIS and ENC100_POR_IO
        // Init ENC100 Pins
        ENC100_ETH_ON_TRIS = 0;
        ENC100_ETH_ON_IO = 0;
        */
    #endif
			
		// Lock Registers
		__builtin_write_OSCCONL(OSCCON | 0x40);

        // Initialize SPI Flash
        #ifndef MPFS_USE_SPI_FLASH
        SPIFlashInit();
        #endif
	}

}

