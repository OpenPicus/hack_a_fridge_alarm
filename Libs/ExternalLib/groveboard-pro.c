/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        groveboard-pro.c
 *  Module:          FlyPort PRO WI-FI, ETH, GPRS
 *  Compiler:        Microchip C30 v3.24 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Simone Marra	     1.0     05/16/2014		   First release  
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "taskFlyport.h"
#include "grovelib.h"
#include "groveboard-pro.h"

//DIGITAL ports type
BYTE g_d1[] = { p13, p15 };	// pin number 1 of digital ports
BYTE g_d2[] = { p14, p17 };	// pin number 2 of digital ports

//SPI port type 
BYTE g_SCK[] 	= { uSD_SCK }; 	// sd-clk
BYTE g_MISO[] 	= { uSD_MISO }; // sd-do
BYTE g_MOSI[] 	= { uSD_MOSI };	// sd-di
BYTE g_SPI_CS[] = { uSD_CS }; 	// sd-cs
BYTE g_SD_CD[] 	= { uSD_CD };	// sd-sw

//I2C port type 
BYTE g_I2CSCL[] = { p26 };	// pin number 1 of i2c ports
BYTE g_I2CSDA[] = { p24 };	// pin number 2 of i2c ports

//AN ports type
BYTE g_an[] ={ 5, 4 };		// analog channels of analog ports

// Return the voltage battery level converted (pay attention it may vary a bit from actual battery level on "Battery" connecter,
// Please, see IoT Kit datasheet and scheme for further info.
float ReadBatteryLevel()
{
	return (float)((float)ADCVal(V_BAT_Channel)*4.2/775); 
}


// Enables or disables Grove Devices Power Supply to reduce current consuption
void GroveDevicesPowerSupply(BOOL enable)
{
	if(enable == TRUE)
		IOPut(V_GROVE_Pin, V_GROVE_Enable);
	else
		IOPut(V_GROVE_Pin, V_GROVE_Disable);
}

struct Board
{
	const void *class;
	int boardType;
	struct Interface *i2cstart;
	struct Interface *i2cnext;
	struct Interface *diostart;
	struct Interface *dionext;
	struct Interface *anistart;
	struct Interface *aninext;
	struct Interface *uartstart;
	struct Interface *uartnext;
};

BOOL GroveStarted = FALSE;


static void *Board_ctor (void * _self, va_list *app)
{
	// Setup analog channels
	ADCDetach(0);	// p1, uSD_MISO
	ADCDetach(1);	// p3, uSD_MOSI
	ADCDetach(2);	// p5, uSD_CS
	ADCDetach(3);	// p7, uSD_SCK
	ADCAttach(4);	// AN2 - analog grove 
	ADCAttach(5);	// AN1 - analog grove 
	ADCDetach(6);
	ADCDetach(7);	
	ADCAttach(8);	// V_BAT - onboard voltage reference
	ADCDetach(9);
	
	//SPI2 pins configuration:
	IOInit(g_MISO[0], SPI_IN);
	IOInit(g_SCK[0] , SPICLKOUT);
	IOInit(g_MOSI[0], SPI_OUT);
	// init CS line as ouput, default high
	IOInit(g_SPI_CS[0], out); // SD-CS line
	IOPut(g_SPI_CS[0], on);
	// sd card detect line as input, with pullup
	IOInit(g_SD_CD[0], inup); // SD-CS line
	// pullup and pulldown resistors:
	#ifdef FLYPORTETH
	_CN27PDE = 1;	// SCK pulldown
	#else
	_CN4PDE = 1;	// SCK pulldown
	#endif
	_CN7PUE = 1;	// SDO pullup
	_CN6PUE = 1;	// SDI pullup

    // Digital Output on board:
    IOInit(V_GROVE_Pin, out); // Grove devices Power Supply MOSFET
    IOPut(V_GROVE_Pin, V_GROVE_Enable);

    IOInit(LED1_Pin, out);  // LED1
    IOPut(LED1_Pin, off);
    IOInit(LED2_Pin, out);  // LED2
    IOPut(LED2_Pin, off);
	
	struct Board *self = _self;
	self->boardType = va_arg(*app, const BYTE);
	self->i2cstart = NULL;
	self->diostart = NULL;
	self->anistart = NULL;
	self->uartstart = NULL;
	return self;
	
}


struct Interface *attachSensorToI2CBus(void *_board,int i2cbus,int add)
{
	struct Board *self = _board;
	struct Interface *p;
	self->i2cnext = self->i2cstart;
	if(!self->i2cstart)
	{
		self->i2cstart = (struct Interface *)malloc(sizeof(struct Interface));
		self->i2cnext = self->i2cstart;
		self->i2cnext->port = NULL;
	}	
	if(!self->i2cstart)
		return NULL;
	p = self->i2cnext;
	if(!p->port)
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		}	
		else
		{
			//internal pull-up for SCL and SDA wires
			CNPU2bits.CN17PUE = 1;	// i2c2 sda
			CNPU2bits.CN18PUE = 1;	// i2c2 scl
			p->port->Pin1 = g_I2CSCL[i2cbus];
			p->port->Pin2 = g_I2CSDA[i2cbus];
			p->port->Use = DOUBLE_PORT;
			p->next = NULL;
			I2C2Init(LOW_SPEED);
		}
	}
	return p;

}		

struct Interface *attachSensorToDigioBus(void *_board,int n,int type)
{
	struct Board *self = _board;
	struct Interface *p;
	if(self->diostart == NULL)
	{
		self->diostart = (struct Interface *)malloc(sizeof(struct Interface));
		if(!self->diostart)
			return NULL;
		self->dionext = self->diostart;
		self->dionext->port = NULL;
		p = self->dionext;
	}
	else
	{
		self->dionext = self->diostart;
		p = self->dionext;
		while(p != NULL)
		{
			p = p->next;
		}
		if(p == NULL)
		{
			p = (struct Interface *)malloc(sizeof(struct Interface));
			if(!self->diostart)
				return NULL;
			p->port = NULL;
		}	
	}

	if(p->port == NULL)	
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		}
		p->port->Pin1 = g_d1[n];
		
		if(type != 4 && type != 5)
		{	
			IOInit(p->port->Pin1, type);
			p->port->Use = SINGLE_PORT;
		}
		else if(type == 5)
		{
			IOInit(p->port->Pin1, OUT);
			p->port->Pin2 = g_d2[n];
			IOInit(p->port->Pin2, OUT);
			p->port->Use = DOUBLE_PORT;			
		}
		p->next = NULL;
	}
	return p;
}


struct Interface *	attachSensorToAnBus (void *_board,int n)
{
	struct Board *self = _board;
	struct Interface *p;

	if(self->anistart == NULL)
	{
		self->anistart = (struct Interface *)malloc(sizeof(struct Interface));
		if(!self->anistart)
			return NULL;
		self->aninext = self->anistart;
		self->aninext->port = NULL;
		p = self->aninext;
	}
	else
	{
		self->aninext = self->anistart;
		p = self->aninext;
		while(p != NULL)
		{
			p = p->next;
		}
		if(p == NULL)
		{
			p = (struct Interface *)malloc(sizeof(struct Interface));
			if(!p)
				return NULL;
			p->port = NULL;
		}	
	}

	if(p->port == NULL)
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		}
		p->port->Pin1 = g_an[n];
		p->port->Use = SINGLE_PORT;		
		p->next = NULL;
	}
	return p;
		
}
	
struct Interface *attachSensorToUartBus(void *_board,int port,int speed,BYTE module,BYTE interf)
{
	struct Board *self = _board;
	struct Interface *p;
	if(self->uartstart == NULL)
	{
		self->uartstart = (struct Interface *)malloc(sizeof(struct Interface));//punta al primo bus DIG_IO
		if(!self->uartstart)
			return NULL;
		self->uartnext = self->uartstart;
		self->uartnext->port = NULL;
		p = self->uartnext;
	}
	else
	{
		self->uartnext = self->uartstart;
		p = self->uartnext;
		while(p != NULL)
		{
			p = p->next;
		}
		if(p == NULL)
		{
			p = (struct Interface *)malloc(sizeof(struct Interface));
			if(!p)
				return NULL;
			p->port = NULL;
		}	
	}

	if(p->port == NULL)
	{	
		p->port = (GrovePort *) malloc(sizeof (GrovePort));
		if(!p->port)
		{
			free(p);
			return NULL;
		} 
		p->port->Pin1 = g_d1[port];
		p->port->Pin2 = g_d2[port];
		p->port->Use = DOUBLE_PORT;
		p->next = NULL;
		if(module == 1)
			return p;//Uart1 already initialized
		// else initialize other uart modules
		IOInit(p->port->Pin1,interf);
		IOInit(p->port->Pin2,(interf+26));
		UARTInit(module,speed);
		UARTOn(module);
	}
	return p;	
}
	

static const struct BoardClass _Board =
{	
	sizeof(struct Board),	
	Board_ctor,
	0,
	0,
};

const void *Board = &_Board;







