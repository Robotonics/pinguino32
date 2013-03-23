/*	----------------------------------------------------------------------------
	FILE:				pinguinoi2c2.c
	PROJECT:			pinguino
	PURPOSE:			I2C2 functions
	PROGRAMER:			regis blanchot <rblanchot@gmail.com>
	FIRST RELEASE:		11 apr. 2011
	LAST RELEASE:		25 feb. 2012
	----------------------------------------------------------------------------
	TODO :
	----------------------------------------------------------------------------
	CHANGELOG :
		13 may. 2011 I2C_init default mode is master 400 khz jp.mandon@gmail.com
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

#ifndef __PINGUINOI2C2_C
#define __PINGUINOI2C2_C

#include <typedef.h>
#include <const.h>
#include <macro.h>
#include <system.c>
#include <i2c.c>

	/// PROTOTYPES

void I2C2_init();
u8 I2C2_send1(u16, u8);
u8 I2C2_get1(u16);
void I2C2_sendID1(u16, u8);
u8 I2C2_writechar1(u8);
u8 I2C2_readchar();
void I2C2_wait();
void I2C2_start();
void I2C2_stop();
void I2C2_restart();
void I2C2_sendNack();
void I2C2_sendAck();

/*	----------------------------------------------------------------------------
	---------- Open the I2C bus
	--------------------------------------------------------------------------*/

void I2C2_init()
{
	I2C_init(I2C2, I2C_MASTER_MODE, I2C_SLEW_OFF);
}

/*	----------------------------------------------------------------------------
	---------- Send a byte to the slave
	--------------------------------------------------------------------------*/

u8 I2C2_send(u16 adress, u8 byte)
{
	return I2C_send(I2C2, adress, byte);
}

/*	----------------------------------------------------------------------------
	---------- Get a byte from slave
	--------------------------------------------------------------------------*/

u8 I2C2_get(u16 adress)
{
	return I2C_get(I2C2, adress);
}

/*	----------------------------------------------------------------------------
	---------- Send the start condition, device address and r/w indication
	--------------------------------------------------------------------------*/

void I2C2_sendID(u16 DeviceID, u8 rw)
{         
	I2C_sendID(I2C2, DeviceID, rw);
}

/*	----------------------------------------------------------------------------
	---------- Send byte and return ack bit
	--------------------------------------------------------------------------*/

u8 I2C2_writechar(u8 byte)
{
	return I2C_writechar(I2C2, byte);
}

/*	----------------------------------------------------------------------------
	---------- Get a byte from the slave
	--------------------------------------------------------------------------*/

u8 I2C2_readchar()
{
	return I2C_readchar(I2C2);
}

/*	----------------------------------------------------------------------------
	---------- Wait for the module to finish it's last action
	--------------------------------------------------------------------------*/

void I2C2_wait()
{
	I2C_wait(I2C2);
}

/*	----------------------------------------------------------------------------
	---------- I2C_ start bit
	----------------------------------------------------------------------------
	--------------------------------------------------------------------------*/

void I2C2_start()
{
	I2C_start(I2C2);
}

/*	----------------------------------------------------------------------------
	---------- Send stop bit
	----------------------------------------------------------------------------
	--------------------------------------------------------------------------*/

void I2C2_stop()
{
	I2C_stop(I2C2);
}

/*	----------------------------------------------------------------------------
	---------- Send stop bit
	----------------------------------------------------------------------------
	--------------------------------------------------------------------------*/

void I2C2_restart()
{
	I2C_restart(I2C2);
}

/*	----------------------------------------------------------------------------
	---------- Send a Not Acknowledge (NAck) to the slave
	--------------------------------------------------------------------------*/

void I2C2_sendNack()
{
	I2C_sendNack(I2C2);
}

/*	----------------------------------------------------------------------------
	---------- Send an Acknowledge (Ack) to the slave
	--------------------------------------------------------------------------*/

void I2C2_sendAck()
{
	I2C_sendAck(I2C2);
}

#endif	/* __PINGUINOI2C_C */

