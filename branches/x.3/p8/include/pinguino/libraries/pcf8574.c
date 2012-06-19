/*	----------------------------------------------------------------------------
	FILE:				pcf8574.c
	PROJECT:			pinguino - http://www.pinguino.cc/
	PURPOSE:			driving pcf8574 i/o expander
	PROGRAMER:		regis blanchot <rblanchot@gmail.com>
	FIRST RELEASE:	29 jul. 2008
	LAST RELEASE:	06 apr. 2011
	----------------------------------------------------------------------------
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	--------------------------------------------------------------------------*/

/*	----------------------------------------------------------------------------
	---------- PCF8574P
	----------------------------------------------------------------------------

	+5V		A0		-|o|-		VDD	+5V
	+5V		A1		-|	|-		SDA	pull-up 1K8 au +5V
	+5V		A2		-|	|-		SCL 	pull-up 1K8 au +5V
	LCD_BL	P0		-|	|-		INT
	LCD_RS	P1		-|	|-		P7		LCD_D7
	LCD_RW	P2		-|	|-		P6		LCD_D6
	LCD_EN	P3		-|	|-		P5		LCD_D5
	GRND		VSS	-|	|-		P4		LCD_D4

	SYMBOL 	PIN	DESCRIPTION						NB
	A0			1		address input 0				adress = 0 1 0 0 A2 A1 A0 0
	A1			2		address input 1				A0, A1 et A2 relies au +5V
	A2			3		address input 2				donc adress = 01001110 = 0x4E
	P0			4		quasi-bidirectional I/O 0	LCD_BL
	P1			5		quasi-bidirectional I/O 1	LCD_RS
	P2			6		quasi-bidirectional I/O 2	LCD_RW
	P3			7		quasi-bidirectional I/O 3	LCD_EN
	VSS		8		supply ground
	P4			9		quasi-bidirectional I/O 4	LCD_D4
	P5			10		quasi-bidirectional I/O 5	LCD_D5
	P6			11		quasi-bidirectional I/O 6	LCD_D6
	P7			12		quasi-bidirectional I/O 7	LCD_D7
	INT		13		interrupt output (active LOW)
	SCL		14		serial clock line				uC_SCL
	SDA		15		serial data line				uC_SDA
	VDD		16		supply voltage
	--------------------------------------------------------------------------*/

#ifndef __PCF8574_C
	#define __PCF8574_C

#include <typedef.h>
#include <macro.h>
#include <pinguinoi2c.c>
#include <pcf8574.h>

u8 _PCF8574_module;

/**	----------------------------------------------------------------------------
	---------- PCF8574_init
	----------------------------------------------------------------------------
	---------- RAZ de la variable d'echange entre Pic et PCF8574
	--------------------------------------------------------------------------*/

void PCF8574_init(u8 module, u16 speed)
{
	//_PCF8574_data.val = 0;
    #if !defined(PIC18F26J50)
        _PCF8574_module = I2C1;
    #else
        _PCF8574_module = module;
    #endif
    I2C_init(_PCF8574_module, I2C_MASTER_MODE, speed);
}

/**	----------------------------------------------------------------------------
	---------- PCF8574_write
	----------------------------------------------------------------------------
	---------- Write a byte on the PCF8574
	--------------------------------------------------------------------------*/

u8 PCF8574_write(u8 address, u8 mydata)
{
	/** In Master Transmitter mode, the
	first byte transmitted contains the slave address of the
	receiving device (seven bits) and the Read/Write (R/W)
	bit. In this case, the R/W bit will be logic ‘0’. Serial data
	is transmitted eight bits at a time. After each byte is
	transmitted, an Acknowledge bit is received. Start and
	Stop conditions are output to indicate the beginning
	and the end of a serial transfer. */

	I2C_send(_PCF8574_module, address, mydata);

/*
	u8 bRet = 0x00;

	I2C_start(_PCF8574_module);
	I2C_writechar(_PCF8574_module, address | I2C_WRITE);

    #if !defined(PIC18F26J50)
	if (SSPCON2bits.ACKSTAT)    // no Acknowledgment received
	{
		I2C_stop(_PCF8574_module);
		return bRet;
	}
	I2C_writechar(_PCF8574_module, mydata);
	if (!SSPCON2bits.ACKSTAT )  // Acknowledgment received
	{
		bRet = 0x01;
	}
    #else
	if (SSP2CON2bits.ACKSTAT)   // no Acknowledgment received
	{
		I2C_stop(_PCF8574_module);
		return bRet;
	}
	I2C_writechar(_PCF8574_module, mydata);
	if (!SSP2CON2bits.ACKSTAT )  // Acknowledgment received
	{
		bRet = 0x01;
	}
    #endif
    
	I2C_stop(_PCF8574_module);
	return bRet;
*/
}

#endif
