/*	----------------------------------------------------------------------------
	FILE:			serial.c
	PROJECT:		pinguinoX
	PURPOSE:		
	PROGRAMER:		regis blanchot <rblanchot@gmail.com>
	FIRST RELEASE:	10 nov. 2010
	LAST RELEASE:	11 jun. 2013
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
	
	// 13 feb.2011 jp mandon added #define for RX/TX pin on 32mx440f256h
	// 21 set.2011 Marcus Fazzi added support for UART3
	// 23 set.2011 Marcus Fazzi added support for UART4,5 AND 6
	// 18 feb.2012 jp mandon added support for PIC32-PIGUINO-220
	// 19 may.2012 jp mandon added support for GENERIC32MX250F128 and GENERIC32MX220F032
  // 11 jun. 2013 MM OERR Gestion on UART 1
	
#ifndef __SERIAL__
#define __SERIAL__

#include <typedef.h>
#include <system.c>
#include <interrupt.c>
#include <digitalw.c>
#include <printf.c>

#define UART1			1
#define UART2			2
//UART3 is shared with SPI2
//32MX4xx do not have UART3,4,5 AND 6
#ifdef ENABLE_UART3
#define UART3			3
#endif
#ifdef ENABLE_UART4
#define UART4			4
#endif
#ifdef ENABLE_UART5
#define UART5			5
#endif
#ifdef ENABLE_UART6
#define UART6			6
#endif
// -------------------------------------------------------------------------
// UxMODE
// -------------------------------------------------------------------------

// bit 15 ON: UARTx Enable bit
#define UART_DISABLE								0x0000
#define UART_ENABLE								0x8000
/*
#define UART_PERIPHERAL	0x01
#define UART_RX			0x02	// UART Module receiver
#define UART_TX			0x04	// UART Module transmitter
*/
// bit 13 SIDL: Stop in Idle Mode bit
#define UART_ENABLE_STOP_ON_IDLE				0x2000

// bit 12 IREN: IrDA Encoder and Decoder Enable bit
#define UART_ENABLE_IRDA						0x1000

// bit 11 RTSMD: Mode Selection for UxRTS Pin bit
#define UART_RTS_WHEN_RX_NOT_FULL			0x000
#define UART_RTS_WHEN_TX_NOT_EMPTY			0x800

// bit 9-8 UEN<1:0>: UARTx Enable bits
#define UART_ENABLE_PINS_BIT_CLOCK			0x300 // UxTX, UxRX, and UxBCLK pins are enabled and used; UxCTS pin is controlled by port latches
#define UART_ENABLE_PINS_CTS_RTS				0x200 // UxTX, UxRX, UxCTS, and UxRTS pins are enabled and used
#define UART_ENABLE_PINS_RTS					0x100 // UxTX, UxRX and UxRTS pins are enabled and used; UxCTS pin is controlled by port latches
#define UART_ENABLE_PINS_TX_RX_ONLY			0x000 // UxTX and UxRX pins are enabled and used; UxCTS and UxRTS/UxBCLK pins are controlled by port latches

// bit 7 WAKE: Enable Wake-up on Start bit Detect During Sleep mode bit
#define UART_ENABLE_WAKE_ON_START			0x80

// bit 6 LPBACK: UARTx Loopback Mode Select bit
#define UART_ENABLE_LOOPBACK					0x40

// bit 5 ABAUD: Auto-Baud Enable bit

// bit 4 RXINV: Receive Polarity Inversion bit
#define UART_INVERT_RECEIVE_POLARITY		0x10

// bit 3 BRGH: High Baud Rate Enable bit
//#define UART_ENABLE_HIGH_SPEED		0x00000008
#define UART_ENABLE_HIGH_SPEED				1
#define UART_ENABLE_STANDARD_SPEED			0

// bit 2-1 PDSEL<1:0>: Parity and Data Selection bits
 
#define UART_9_BITS_NO_PARITY					0x06
#define UART_8_BITS_ODD_PARITY				0x04
#define UART_8_BITS_EVEN_PARITY				0x02
#define UART_8_BITS_NO_PARITY					0x00

// bit 0 STSEL: Stop Selection bit
#define UART_STOP_BITS_2						0x01	// Enables generation of 2 stop bits per frame.
#define UART_STOP_BITS_1						0x00	// Enables generation of 1 stop bit per frame (default).

// -------------------------------------------------------------------------
// UxSTA
// -------------------------------------------------------------------------

// bit 24 ADM_EN: Automatic Address Detect Mode Enable bit

// bit 15-14 UTXISEL<1:0>: Tx Interrupt Mode Selection bits
#define UART_INTERRUPT_ON_TX_BUFFER_EMPTY	0x8000
#define UART_INTERRUPT_ON_TX_DONE			0x4000
#define UART_INTERRUPT_ON_TX_NOT_FULL		0x0000

// bit 13 UTXINV: Transmit Polarity Inversion bit
#define UART_INVERT_TRANSMIT_POLARITY		0x2000

// bit 12 URXEN: Receiver Enable bit
#define UART_RX_ENABLED							0x1000	// UARTx receiver is enabled, UxRX pin controlled by UARTx (if ON = 1)
#define UART_RX_DISABLED						0x0000	// UARTx receiver is disabled, the UxRX pin is ignored by the UARTx module. UxRX pin controlled by PORT.

// bit 11 UTXBRK: Transmit Break bit

// bit 10 UTXEN: Transmit Enable bit
#define UART_TX_ENABLED							0x400		// UARTx transmitter enabled, UxTX pin controlled by UARTx (if ON = 1)
#define UART_TX_DISABLED						0x000		// UARTx transmitter disabled, any pending transmission is aborted and buffer is reset. UxTX pin controlled by PORT.

#define UART_RX_TX_ENABLED						0x1400

// bit 7-6 URXISEL<1:0>: Receive Interrupt Mode Selection bit
#define UART_INTERRUPT_ON_RX_FULL			0xC0
#define UART_INTERRUPT_ON_RX_HALF_FULL		0x40
#define UART_INTERRUPT_ON_RX_NOT_EMPTY		0x00

// bit 5 ADDEN: Address Character Detect (bit 8 of received data = 1)

// UART_CONFIGURATION
#define UART_SUPPORT_IEEE_485					0x00000900

// UART_LINE_STATUS;
#define UART_TRANSMITTER_NOT_FULL			0x00000200	// The transmitter is able to accept data to transmit.
#define UART_TRANSMITTER_EMPTY				0x00000100	// The transmitter is empty (no data is available to transmit).
#define UART_RECEIVER_IDLE						0x00000010	// The receiver is currently idle.
#define UART_PARITY_ERROR						0x00000008	// A received data parity error was detected.
#define UART_FRAMING_ERROR						0x00000004	// Data was received that violated the framing protocol
#define UART_OVERRUN_ERROR						0x00000002	// The UART has received more data than it can buffer.  Data has been lost.
#define UART_DATA_READY							0x00000001	// UART data has been received and is avaiable in the FIFO.

//#ifndef SERIAL_BUFFERLENGTH
	#define SERIAL_BUFFERLENGTH 				128				// rx buffer length
//#endif

volatile char UART1SerialBuffer[SERIAL_BUFFERLENGTH];	// UART1 buffer
volatile char UART2SerialBuffer[SERIAL_BUFFERLENGTH];	// UART2 buffer
#ifdef ENABLE_UART3
volatile char UART3SerialBuffer[SERIAL_BUFFERLENGTH];	// UART3 buffer
#endif
#ifdef ENABLE_UART4
volatile char UART4SerialBuffer[SERIAL_BUFFERLENGTH];	// UART4 buffer
#endif
#ifdef ENABLE_UART5
volatile char UART5SerialBuffer[SERIAL_BUFFERLENGTH];	// UART5 buffer
#endif
#ifdef ENABLE_UART6
volatile char UART6SerialBuffer[SERIAL_BUFFERLENGTH];	// UART6 buffer
#endif

volatile long UART1wpointer, UART1rpointer;				// write and read pointer
volatile long UART2wpointer, UART2rpointer;				// write and read pointer
#ifdef ENABLE_UART3
volatile long UART3wpointer, UART3rpointer;				// write and read pointer
#endif
#ifdef ENABLE_UART4
volatile long UART4wpointer, UART4rpointer;				// write and read pointer
#endif
#ifdef ENABLE_UART5
volatile long UART5wpointer, UART5rpointer;				// write and read pointer
#endif
#ifdef ENABLE_UART6
volatile long UART6wpointer, UART6rpointer;				// write and read pointer
#endif
/*	----------------------------------------------------------------------------
	SerialSetDataRate()
	----------------------------------------------------------------------------
	@param		port		1 (UART1) or 2 (UART2)
	@param		baudrate	baud rate
	@return		baudrate
	----------------------------------------------------------------------------
	BRGH: High Baud Rate Enable bit
	if BRGH = 1 = High-Speed mode - 4x baud clock enabled
		then UxBRG = ((FPB/Desired Baud Rate)/ 4) - 1
	if BRGH 0 = Standard Speed mode - 16x baud clock enabled
		then UxBRG = ((FPB/Desired Baud Rate)/16) - 1
	--------------------------------------------------------------------------*/

void SerialSetDataRate(u8 port, u32 baudrate)
{
	u8 speed;
	u32 max, max1, max2;
	u32 min1, min2;
	u32 pbclock;

	pbclock = GetPeripheralClock();
	max1 = pbclock / 4;
	min1 = max1 / 65536;
	max2 = pbclock / 16;
	min2 = max2 / 65536;
	if (baudrate > max1) baudrate = max1;
	if (baudrate < min2) baudrate = min2;
	max = (min1 + max2) / 2;
	//if (baudrate > min2 && baudrate < max) speed = UART_ENABLE_STANDARD_SPEED;
	if (baudrate > max && baudrate < max1) speed = UART_ENABLE_HIGH_SPEED;

	switch (port)
	{
		case UART1:
			if (speed == UART_ENABLE_HIGH_SPEED)
			{
				U1MODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
				U1BRG = ((pbclock / baudrate) / 4) - 1;
			}
			else
				U1BRG = ((pbclock / baudrate) / 16) - 1;
			break;
		case UART2:
			if (speed == UART_ENABLE_HIGH_SPEED)
			{
				U2MODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
				U2BRG = ((pbclock / baudrate) / 4) - 1;
			}
			else
				U2BRG = ((pbclock / baudrate) / 16) - 1;
			break;
#ifdef ENABLE_UART3
		case UART3:
			if (speed == UART_ENABLE_HIGH_SPEED)
			{
				U2AMODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
				U2ABRG = ((pbclock / baudrate) / 4) - 1;
			}
			else
				U2ABRG = ((pbclock / baudrate) / 16) - 1;
			break;
#endif
#ifdef ENABLE_UART4
		case UART4:
			if (speed == UART_ENABLE_HIGH_SPEED)
			{
				U1BMODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
				U1BBRG = ((pbclock / baudrate) / 4) - 1;
			}
			else
				U1BBRG = ((pbclock / baudrate) / 16) - 1;
			break;
#endif
#ifdef ENABLE_UART5
		case UART5:
			if (speed == UART_ENABLE_HIGH_SPEED)
			{
				U3BMODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
				U3BBRG = ((pbclock / baudrate) / 4) - 1;
			}
			else
				U3BBRG = ((pbclock / baudrate) / 16) - 1;
			break;
#endif
#ifdef ENABLE_UART6
		case UART6:
			if (speed == UART_ENABLE_HIGH_SPEED)
			{
				U2BMODEbits.BRGH = UART_ENABLE_HIGH_SPEED;
				U2BBRG = ((pbclock / baudrate) / 4) - 1;
			}
			else
				U2BBRG = ((pbclock / baudrate) / 16) - 1;
			break;
#endif
	}
}

#ifndef __PIC32MX220F032D__

/*	----------------------------------------------------------------------------
	SerialGetDataRate
	--------------------------------------------------------------------------*/

u32 SerialGetDataRate(u8 port)
{
	u8 speed;
	u32 baudrate;
	u32 pbclock;

	pbclock = GetPeripheralClock();

	switch (port)
	{
		case UART1:
			speed = U1MODEbits.BRGH;
			if (speed == UART_ENABLE_HIGH_SPEED)
				baudrate = pbclock / (4 * (U1BRG + 1));
			else
				baudrate = pbclock / (16 * (U1BRG + 1));
			break;
		case UART2:
			speed = U2MODEbits.BRGH;
			if (speed == UART_ENABLE_HIGH_SPEED)
				baudrate = pbclock / (4 * (U2BRG + 1));
			else
				baudrate = pbclock / (16 * (U2BRG + 1));
			break;
#ifdef ENABLE_UART3
		case UART3:
			speed = U2AMODEbits.BRGH;
			if (speed == UART_ENABLE_HIGH_SPEED)
				baudrate = pbclock / (4 * (U2ABRG + 1));
			else
				baudrate = pbclock / (16 * (U2ABRG + 1));
			break;
#endif
#ifdef ENABLE_UART4
		case UART4:
			speed = U1BMODEbits.BRGH;
			if (speed == UART_ENABLE_HIGH_SPEED)
				baudrate = pbclock / (4 * (U1BBRG + 1));
			else
				baudrate = pbclock / (16 * (U1BBRG + 1));
			break;
#endif
#ifdef ENABLE_UART5
		case UART5:
			speed = U3BMODEbits.BRGH;
			if (speed == UART_ENABLE_HIGH_SPEED)
				baudrate = pbclock / (4 * (U3BBRG + 1));
			else
				baudrate = pbclock / (16 * (U3BBRG + 1));
			break;
#endif
#ifdef ENABLE_UART6
		case UART6:
			speed = U2BMODEbits.BRGH;
			if (speed == UART_ENABLE_HIGH_SPEED)
				baudrate = pbclock / (4 * (U2BBRG + 1));
			else
				baudrate = pbclock / (16 * (U2BBRG + 1));
			break;
#endif
		default:
			baudrate = 0;
	}
	return baudrate;
}

#endif

/*	----------------------------------------------------------------------------
	SerialEnable
	----------------------------------------------------------------------------
	ex : SerialEnable(UART1, UART_RX_TX_ENABLED | UART_INTERRUPT_ON_RX_FULL)
	--------------------------------------------------------------------------*/

void SerialEnable(u8 port, u32 config)
{
	switch (port)
	{
		case UART1: U1STASET = config; break;
		case UART2:	U2STASET = config; break;
#ifdef ENABLE_UART3
		case UART3:	U2ASTASET = config; break;
#endif
#ifdef ENABLE_UART4
		case UART4:	U1BSTASET = config; break;
#endif
#ifdef ENABLE_UART5
		case UART5:	U3BSTASET = config; break;
#endif
#ifdef ENABLE_UART6
		case UART6:	U2BSTASET = config; break;
#endif
	}
}

/*	----------------------------------------------------------------------------
	SerialSetLineControl
	----------------------------------------------------------------------------
	ex : SerialSetLineControl(UART1, UART_ENABLE | UART_ENABLE_PINS_TX_RX_ONLY | UART_8_BITS_NO_PARITY | UART_STOP_BITS_1)
	--------------------------------------------------------------------------*/

void SerialSetLineControl(u8 port, u32 config)
{
	switch (port)
	{
		case UART1: U1MODESET = config; break;
		case UART2: U2MODESET = config;	break;
#ifdef ENABLE_UART3
		case UART3: U2AMODESET = config; break;
#endif
#ifdef ENABLE_UART4
		case UART4: U1BMODESET = config; break;
#endif
#ifdef ENABLE_UART5
		case UART5: U3BMODESET = config; break;
#endif
#ifdef ENABLE_UART6
		case UART6: U2BMODESET = config; break;
#endif
	}
}

/*	----------------------------------------------------------------------------
	SerialFlush : clear SerialBuffer
	--------------------------------------------------------------------------*/

void SerialFlush(u8 port)
{
	switch (port)
	{
		case UART1:
			UART1wpointer = 1;
			UART1rpointer = 1;
			break;
		case UART2:
			UART2wpointer = 1;
			UART2rpointer = 1;
			break;
#ifdef ENABLE_UART3
		case UART3:
			UART3wpointer = 1;
			UART3rpointer = 1;
			break;
#endif
#ifdef ENABLE_UART4
		case UART4:
			UART4wpointer = 1;
			UART4rpointer = 1;
			break;
#endif
#ifdef ENABLE_UART5
		case UART5:
			UART5wpointer = 1;
			UART5rpointer = 1;
			break;
#endif
#ifdef ENABLE_UART6
		case UART6:
			UART6wpointer = 1;
			UART6rpointer = 1;
			break;
#endif
	}
}

/*	----------------------------------------------------------------------------
	SerialPinConfigure : UART I/O pins control
	--------------------------------------------------------------------------*/

void SerialPinConfigure(u8 port)
{
	switch (port)
	{
		case UART1:
#ifdef __32MX460F512L__
			TRISFbits.TRISF8 = OUTPUT;	// RF8 / U1TX output
			TRISFbits.TRISF2 = INPUT;	// RF2 / U1RX input
#endif
#ifdef __32MX440F256H__
			TRISDbits.TRISD3 = OUTPUT;	// RD3 / U1TX output
			TRISDbits.TRISD2 = INPUT;	// RD2 / U1RX input
#endif
#ifdef PIC32_PINGUINO_220
			TRISBbits.TRISB4 = OUTPUT;	// RB4 / U1TX output
			TRISAbits.TRISA4 = INPUT;	// RA4 / U1RX input
#endif			
#if defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
			TRISBbits.TRISB2=INPUT;		// RB2 is input ( RX )
			TRISBbits.TRISB3=OUTPUT;	// RB3 is output ( TX )
#endif
			break;
		case UART2:
#ifdef PIC32_PINGUINO_220
			TRISCbits.TRISC9 = OUTPUT;	// RC9 / U2TX output
			TRISCbits.TRISC8 = INPUT;	// RC8 / U2RX input				
#else
	#if defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		TRISBbits.TRISB1=INPUT;		// RB1 is input ( RX )
		TRISBbits.TRISB0=OUTPUT;	// RB0 is output ( TX )
	#else
		TRISFbits.TRISF5 = OUTPUT;	// RF5 / U2TX output
		TRISFbits.TRISF4 = INPUT;	// RF4 / U2RX input
	#endif
#endif
			break;
//32MX4xx not have UART3,4,5 AND 6
#ifdef ENABLE_UART3
		case UART3:
			TRISGbits.TRISG8 = OUTPUT;	// RG8 / U3TX output
			TRISGbits.TRISG7 = INPUT;	// RG7 / U3RX input
			break;
#endif
#ifdef ENABLE_UART4
		case UART4:
			TRISDbits.TRISD15 = OUTPUT;	// RD15 / U4TX output
			TRISDbits.TRISD14 = INPUT;	// RD14 / U4RX input
			break;
#endif
#ifdef ENABLE_UART5
#ifndef __32MX220F032D__
		case UART5:
			TRISFbits.TRISF13 = OUTPUT;	// RF13 / U5TX output
			TRISFbits.TRISF12 = INPUT;	// RF12 / U5RX input
			break;
#endif
#endif
#ifdef ENABLE_UART6
		case UART6:
			TRISGbits.TRISG6 = OUTPUT;	// RG6 / U6TX output
			TRISGbits.TRISG9 = INPUT;	// RG9 / U6RX input
			break;
#endif
	}
}

/*	----------------------------------------------------------------------------
	SerialIntConfigure() : Serial Interrupts Configuration
	----------------------------------------------------------------------------
	@param		port		1 (UART1), 2 (UART2) or 3 (UART3)
	@param		baudrate	baud rate
	@return		baudrate
	--------------------------------------------------------------------------*/

void SerialIntConfigure(u8 port, u8 priority, u8 subpriority)
{
	IntConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	
	switch (port)
	{
		case UART1:
			#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
				IPC8bits.U1IP = priority;
				IPC8bits.U1IS = subpriority;
				IEC1bits.U1RXIE=1;
			#else		
			IntSetVectorPriority(INT_UART1_VECTOR, priority, subpriority);
			IntEnable(INT_UART1_RECEIVER);
			#endif
			break;
		case UART2:
			#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
				IPC9bits.U2IP = priority;
				IPC9bits.U2IS = subpriority;
				IEC1bits.U2RXIE=1;
			#else
			IntSetVectorPriority(INT_UART2_VECTOR, priority, subpriority);
			IntEnable(INT_UART2_RECEIVER);
			#endif
			break;
#ifdef ENABLE_UART3
		case UART3:
			IntSetVectorPriority(INT_UART3_VECTOR, priority, subpriority);
			IntEnable(INT_UART3_RECEIVER);
			break;
#endif
#ifdef ENABLE_UART4
		case UART4:	
			IntSetVectorPriority(INT_UART4_VECTOR, priority, subpriority);
			IntEnable(INT_UART4_RECEIVER);
			break;
#endif
#ifdef ENABLE_UART5
		case UART5:
			IntSetVectorPriority(INT_UART5_VECTOR, priority, subpriority);
			IntEnable(INT_UART5_RECEIVER);
			break;
#endif
#ifdef ENABLE_UART6
		case UART6:
			IntSetVectorPriority(INT_UART6_VECTOR, priority, subpriority);
			IntEnable(INT_UART6_RECEIVER);
			break;
#endif
	}
}

/*	----------------------------------------------------------------------------
	SerialConfigure()
	----------------------------------------------------------------------------
	@param		port		1 (UART1) or 2 (UART2) or 3 (UART3) ...
	@param		baudrate	baud rate
	@return		baudrate
	--------------------------------------------------------------------------*/

void SerialConfigure(u8 port, u32 config, u32 enable, u32 baudrate)
{
	SerialPinConfigure(port);
	SerialSetDataRate(port, baudrate);		// UxBRG
	SerialSetLineControl(port, config);		// UxMODE
	SerialEnable(port, enable);				// UxSTA
	SerialIntConfigure(port, INT_PRIORITY_3, INT_SUBPRIORITY_3);
	SerialFlush(port);
}

/*	----------------------------------------------------------------------------
	SerialWriteChar1 : write data bits 0-8 on the UART1
	--------------------------------------------------------------------------*/

void SerialUART1WriteChar(char c)
{
	while (!U1STAbits.TRMT);				// wait transmitter is ready
	U1TXREG = c;
}

/*	----------------------------------------------------------------------------
	SerialWriteChar2 : write data bits 0-8 on the UART2
	--------------------------------------------------------------------------*/

void SerialUART2WriteChar(char c)
{
	while (!U2STAbits.TRMT);				// wait transmission has completed
	U2TXREG = c;
}

/*	----------------------------------------------------------------------------
	SerialWriteChar3 : write data bits 0-8 on the UART3
	--------------------------------------------------------------------------*/
#ifdef ENABLE_UART3
void SerialUART3WriteChar(char c)
{
	while (!U2ASTAbits.TRMT);				// wait transmission has completed	
	U2ATXREG = c;
}
#endif

/*	----------------------------------------------------------------------------
	SerialWriteChar4 : write data bits 0-8 on the UART3
	--------------------------------------------------------------------------*/
#ifdef ENABLE_UART4
void SerialUART4WriteChar(char c)
{
	while (!U1BSTAbits.TRMT);				// wait transmission has completed	
	U1BTXREG = c;
}
#endif

/*	----------------------------------------------------------------------------
	SerialWriteChar5 : write data bits 0-8 on the UART3
	--------------------------------------------------------------------------*/
#ifdef ENABLE_UART5
void SerialUART5WriteChar(char c)
{
	while (!U3BSTAbits.TRMT);				// wait transmission has completed	
	U3BTXREG = c;
}
#endif

/*	----------------------------------------------------------------------------
	SerialWriteChar6 : write data bits 0-8 on the UART3
	--------------------------------------------------------------------------*/
#ifdef ENABLE_UART6
void SerialUART6WriteChar(char c)
{
	while (!U2BSTAbits.TRMT);				// wait transmission has completed	
	U2BTXREG = c;
}
#endif
/*	----------------------------------------------------------------------------
	SerialPrintf : write formated string on the serial port
	--------------------------------------------------------------------------*/

//void SerialPrintf(u8 port, const char *fmt, ...)
void SerialPrintf(u8 port, unsigned char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	switch (port)
	{
		case UART1: pprintf(SerialUART1WriteChar, fmt, args); break;
		case UART2: pprintf(SerialUART2WriteChar, fmt, args); break;
#ifdef ENABLE_UART3
		case UART3: pprintf(SerialUART3WriteChar, fmt, args); break;
#endif
#ifdef ENABLE_UART4
		case UART4: pprintf(SerialUART4WriteChar, fmt, args); break;
#endif
#ifdef ENABLE_UART5
		case UART5: pprintf(SerialUART5WriteChar, fmt, args); break;
#endif
#ifdef ENABLE_UART6
		case UART6: pprintf(SerialUART6WriteChar, fmt, args); break;
#endif
	}
	va_end(args);
}

/*	----------------------------------------------------------------------------
	SerialAvailable
	--------------------------------------------------------------------------*/

char SerialAvailable(u8 port)
{
	switch (port)
	{
		case UART1: return (UART1wpointer != UART1rpointer); break;
		case UART2:	return (UART2wpointer != UART2rpointer); break;
#ifdef ENABLE_UART3
		case UART3:	return (UART3wpointer != UART3rpointer); break;
#endif
#ifdef ENABLE_UART4
		case UART4:	return (UART4wpointer != UART4rpointer); break;
#endif
#ifdef ENABLE_UART5
		case UART5:	return (UART5wpointer != UART5rpointer); break;
#endif
#ifdef ENABLE_UART6
		case UART6:	return (UART6wpointer != UART6rpointer); break;
#endif
	}
}

/*	----------------------------------------------------------------------------
	SerialRead : Get char
	--------------------------------------------------------------------------*/

char SerialRead(u8 port)
{
	char c = 0;

	if (SerialAvailable(port))
	{
		switch (port)
		{
			case UART1:
				c = UART1SerialBuffer[UART1rpointer++];
				if (UART1rpointer == SERIAL_BUFFERLENGTH)
					UART1rpointer=1;
				return(c);
				break;
			case UART2:
				c = UART2SerialBuffer[UART2rpointer++];
				if (UART2rpointer == SERIAL_BUFFERLENGTH)
					UART2rpointer=1;
				return(c);
				break;
#ifdef ENABLE_UART3
			case UART3:
				c = UART3SerialBuffer[UART3rpointer++];
				if (UART3rpointer == SERIAL_BUFFERLENGTH)
					UART3rpointer=1;
				return(c);
				break;
#endif
#ifdef ENABLE_UART4
			case UART4:				
				c = UART4SerialBuffer[UART4rpointer++];
				if (UART4rpointer == SERIAL_BUFFERLENGTH)
					UART4rpointer=1;
				return(c);
				break;
#endif
#ifdef ENABLE_UART5
			case UART5:
				c = UART5SerialBuffer[UART5rpointer++];
				if (UART5rpointer == SERIAL_BUFFERLENGTH)
					UART5rpointer=1;
				return(c);
				break;
#endif
#ifdef ENABLE_UART6
			case UART6:
				c = UART6SerialBuffer[UART6rpointer++];
				if (UART6rpointer == SERIAL_BUFFERLENGTH)
					UART6rpointer=1;
				return(c);
				break;
#endif
		}
	}
	return(-1);
}

/*	----------------------------------------------------------------------------
	SerialGetKey
	--------------------------------------------------------------------------*/

char SerialGetKey(u8 port)
{
	char c;

	while (!(SerialAvailable(port)));
	c = SerialRead(port);
	SerialFlush(port);
	return (c);
}

/*	----------------------------------------------------------------------------
	SerialGetString
	error: cannot convert 'char (*)[80]' to 'char*' in return
	--------------------------------------------------------------------------*/

char * SerialGetString(u8 port)
{
	u8 i = 0;
	char c;
	static char buffer[80];
	
	do {
		c = SerialGetKey(port);
		SerialPrintf(port, "%c", c);
		buffer[i++] = c;
	} while (c != '\r');
	buffer[i] = '\0';

	return (buffer);
}

/*	----------------------------------------------------------------------------
	SerialGetDataBuffer
	--------------------------------------------------------------------------*/

void SerialGetDataBuffer(u8 port)
{
	char caractere;
	char newwp;

	switch (port)
	{
		case UART1:
			caractere = U1RXREG;							// read received char
			if (UART1wpointer != SERIAL_BUFFERLENGTH - 1)	// if not last place in buffer
				newwp = UART1wpointer + 1;					// place=place+1
			else
				newwp = 1;									// else place=1

			if (UART1rpointer != newwp)						// if read pointer!=write pointer
				UART1SerialBuffer[UART1wpointer++] = caractere;	// store received char

			if (UART1wpointer == SERIAL_BUFFERLENGTH)		// if write pointer=length buffer
				UART1wpointer = 1;							// write pointer = 1

			//return UART1SerialBuffer;
			break;

		case UART2:
			caractere = U2RXREG;							// read received char
			if (UART2wpointer != SERIAL_BUFFERLENGTH - 1)	// if not last place in buffer
				newwp = UART2wpointer + 1;					// place=place+1
			else
				newwp = 1;									// else place=1

			if (UART2rpointer != newwp)						// if read pointer!=write pointer
				UART2SerialBuffer[UART2wpointer++] = caractere;	// store received char

			if (UART2wpointer == SERIAL_BUFFERLENGTH)		// if write pointer=length buffer
				UART2wpointer = 1;								// write pointer = 1

			//return UART2SerialBuffer;
			break;
#ifdef ENABLE_UART3
		case UART3:
			caractere = U2ARXREG;							// read received char
			if (UART3wpointer != SERIAL_BUFFERLENGTH - 1)	// if not last place in buffer
				newwp = UART3wpointer + 1;					// place=place+1
			else
				newwp = 1;									// else place=1

			if (UART3rpointer != newwp)						// if read pointer!=write pointer
				UART3SerialBuffer[UART3wpointer++] = caractere;	// store received char

			if (UART3wpointer == SERIAL_BUFFERLENGTH)		// if write pointer=length buffer
				UART3wpointer = 1;							// write pointer = 1

			//return UART3SerialBuffer;
			break;
#endif
#ifdef ENABLE_UART4
		case UART4:
			caractere = U1BRXREG;							// read received char
			if (UART4wpointer != SERIAL_BUFFERLENGTH - 1)	// if not last place in buffer
				newwp = UART4wpointer + 1;					// place=place+1
			else
				newwp = 1;									// else place=1

			if (UART4rpointer != newwp)						// if read pointer!=write pointer
				UART4SerialBuffer[UART4wpointer++] = caractere;	// store received char

			if (UART4wpointer == SERIAL_BUFFERLENGTH)		// if write pointer=length buffer
				UART4wpointer = 1;							// write pointer = 1

			//return UART4SerialBuffer;
			break;
#endif
#ifdef ENABLE_UART5
		case UART5:
			caractere = U3BRXREG;							// read received char
			if (UART5wpointer != SERIAL_BUFFERLENGTH - 1)	// if not last place in buffer
				newwp = UART5wpointer + 1;					// place=place+1
			else
				newwp = 1;									// else place=1

			if (UART5rpointer != newwp)						// if read pointer!=write pointer
				UART5SerialBuffer[UART5wpointer++] = caractere;	// store received char

			if (UART5wpointer == SERIAL_BUFFERLENGTH)		// if write pointer=length buffer
				UART5wpointer = 1;							// write pointer = 1

			//return UART5SerialBuffer;
			break;
#endif
#ifdef ENABLE_UART6
		case UART6:
			caractere = U2BRXREG;							// read received char
			if (UART6wpointer != SERIAL_BUFFERLENGTH - 1)	// if not last place in buffer
				newwp = UART6wpointer + 1;					// place=place+1
			else
				newwp = 1;									// else place=1

			if (UART6rpointer != newwp)						// if read pointer!=write pointer
				UART6SerialBuffer[UART6wpointer++] = caractere;	// store received char

			if (UART6wpointer == SERIAL_BUFFERLENGTH)		// if write pointer=length buffer
				UART6wpointer = 1;							// write pointer = 1

			//return UART3SerialBuffer;
			break;
#endif
	}
}


/*	----------------------------------------------------------------------------
	SerialInterrupt
	TODO: move this to interrupt library and add it to main32.c
	--------------------------------------------------------------------------*/

// vector 24 or 32 (PIC32_PINGUINO_220)
void Serial1Interrupt(void)
{
	char    Dummy;

	// Is this an RX interrupt from UART1 ?
	#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
	if (IFS1bits.U1RXIF)
	#else	
	if (IntGetFlag(INT_UART1_RECEIVER))
	#endif
	{
   if (U1STAbits.OERR != 0)
    U1STAbits.OERR = 0;
   else
 		 do
     {
      if ((U1STAbits.FERR != 0) || (U1STAbits.PERR != 0))
       Dummy = U1RXREG;
      else
       SerialGetDataBuffer(UART1);
     }
    while (U1STAbits.URXDA != 0);
		#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		IFS1bits.U1RXIF=0;
		#else	
		IntClearFlag(INT_UART1_RECEIVER);
		#endif
	}

	// Is this an TX interrupt from UART1 ?
	#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
	if (IFS1bits.U1TXIF)
	#else		
	if (IntGetFlag(INT_UART1_TRANSMITTER))
	#endif
	{
		#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		IFS1bits.U1TXIF=0;
		#else			
		IntClearFlag(INT_UART1_TRANSMITTER);
		#endif
	}

	// Is this an ERROR interrupt from UART1 ?
	#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
	if (IFS1bits.U1EIF)
	#else	
	if (IntGetFlag(INT_UART1_ERROR))
	#endif
	{
		#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		IFS1bits.U1EIF=0;
		#else	
		IntClearFlag(INT_UART1_ERROR);
		#endif
	}		

}

// vector 32 or 37 (PIC32_PINGUINO_220)
void Serial2Interrupt(void)
{
	char    Dummy;

	// Is this an RX interrupt from UART2 ?
	#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
	if (IFS1bits.U2RXIF)
	#else
	if (IntGetFlag(INT_UART2_RECEIVER))
	#endif
	
	{
   if (U2STAbits.OERR != 0)
    U2STAbits.OERR = 0;
   else
 		 do
     {
      if ((U2STAbits.FERR != 0) || (U2STAbits.PERR != 0))
       Dummy = U2RXREG;
      else
       SerialGetDataBuffer(UART2);
     }
    while (U2STAbits.URXDA != 0);
		//Toggle(REDLED);			// Toggle LED to indicate UART activity
		#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		IFS1bits.U2RXIF=0;
		#else		
		IntClearFlag(INT_UART2_RECEIVER);
		#endif
	}
	// Is this an TX interrupt from UART2 ?
	#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
	if (IFS1bits.U2TXIF)
	#else	
	if (IntGetFlag(INT_UART2_TRANSMITTER))
	#endif
	{
		#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		IFS1bits.U2TXIF=0;
		#else	
		IntClearFlag(INT_UART2_TRANSMITTER);
		#endif
	}
	// Is this an ERROR interrupt from UART2 ?
	#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
	if (IFS1bits.U2EIF)
	#else	
	if (IntGetFlag(INT_UART2_ERROR))
	#endif
	{
		#if defined(PIC32_PINGUINO_220)||defined(GENERIC32MX250F128)||defined(GENERIC32MX220F032)
		IFS1bits.U2EIF=0;
		#else	
		IntClearFlag(INT_UART2_ERROR);
		#endif
	}	
}

#ifdef ENABLE_UART3
// vector 31 is shared with SPI2
void Serial3Interrupt(void)
{	
	// Is this an RX interrupt from UART3 ?
	if (IntGetFlag(INT_UART3_RECEIVER))
	{
   if (U3STAbits.OERR != 0)
    U3STAbits.OERR = 0;
   else
 		 do
     {
      if ((U3STAbits.FERR != 0) || (U3STAbits.PERR != 0))
       Dummy = U3RXREG;
      else
       SerialGetDataBuffer(UART3);
     }
    while (U3STAbits.URXDA != 0);
		//Toggle(REDLED);			// Toggle LED to indicate UART activity
		IntClearFlag(INT_UART3_RECEIVER);
	}
	// Is this an TX interrupt from UART3 ?
	if (IntGetFlag(INT_UART3_TRANSMITTER))
	{
		IntClearFlag(INT_UART3_TRANSMITTER);
	}
}
#endif

#ifdef ENABLE_UART4
// vector 49
void Serial4Interrupt(void)
{	
	//toggle(REDLED);
	// Is this an RX interrupt from UART4 ?
	if (IntGetFlag(INT_UART4_RECEIVER))
	{
   if (U4STAbits.OERR != 0)
    U4STAbits.OERR = 0;
   else
 		 do
     {
      if ((U4STAbits.FERR != 0) || (U4STAbits.PERR != 0))
       Dummy = U4RXREG;
      else
       SerialGetDataBuffer(UART4);
     }
    while (U4STAbits.URXDA != 0);
		//toggle(REDLED);			// Toggle LED to indicate UART activity
		IntClearFlag(INT_UART4_RECEIVER);
	}
	// Is this an TX interrupt from UART4 ?
	if (IntGetFlag(INT_UART4_TRANSMITTER))
	{
		IntClearFlag(INT_UART4_TRANSMITTER);
	}
}
#endif

#ifdef ENABLE_UART5
// vector 51
void Serial5Interrupt(void)
{
	// Is this an RX interrupt from UART5 ?
	if (IntGetFlag(INT_UART5_RECEIVER))
	{
   if (U5STAbits.OERR != 0)
    U5STAbits.OERR = 0;
   else
 		 do
     {
      if ((U5STAbits.FERR != 0) || (U5STAbits.PERR != 0))
       Dummy = U5RXREG;
      else
       SerialGetDataBuffer(UART5);
     }
    while (U5STAbits.URXDA != 0);
		//Toggle(REDLED);			// Toggle LED to indicate UART activity
		IntClearFlag(INT_UART5_RECEIVER);
	}
	// Is this an TX interrupt from UART5 ?
	if (IntGetFlag(INT_UART5_TRANSMITTER))
	{
		IntClearFlag(INT_UART5_TRANSMITTER);
	}
}
#endif

#ifdef ENABLE_UART6
// vector 50
void Serial6Interrupt(void)
{
	// Is this an RX interrupt from UART6 ?
	if (IntGetFlag(INT_UART6_RECEIVER))
	{
   if (U6STAbits.OERR != 0)
    U6STAbits.OERR = 0;
   else
 		 do
     {
      if ((U6STAbits.FERR != 0) || (U6STAbits.PERR != 0))
       Dummy = U6RXREG;
      else
       SerialGetDataBuffer(UART6);
     }
    while (U6STAbits.URXDA != 0);
		//Toggle(REDLED);			// Toggle LED to indicate UART activity
		IntClearFlag(INT_UART6_RECEIVER);
	}
	// Is this an TX interrupt from UART6 ?
	if (IntGetFlag(INT_UART6_TRANSMITTER))
	{
		IntClearFlag(INT_UART6_TRANSMITTER);
	}
}
#endif

/*	----------------------------------------------------------------------------
	ClearRxError
	--------------------------------------------------------------------------*/

BOOL ClearRxError(u8 port)
{

	switch (port)
	{
		case UART1:
    if (U1STAbits.OERR != 0)
     {
      U1STAbits.OERR = 0;
      return(FALSE);
     }
    break;

		case UART2:
    if (U2STAbits.OERR != 0)
     {
      U2STAbits.OERR = 0;
      return(FALSE);
     }
		break;

#ifdef ENABLE_UART3
		case UART3:
    if (U3STAbits.OERR != 0)
     {
      U3STAbits.OERR = 0;
      return(FALSE);
     }
		break;
#endif

#ifdef ENABLE_UART4
		case UART4:
    if (U4STAbits.OERR != 0)
     {
      U4STAbits.OERR = 0;
      return(FALSE);
     }
		break;
#endif

#ifdef ENABLE_UART5
		case UART5:
    if (U5STAbits.OERR != 0)
     {
      U5STAbits.OERR = 0;
      return(FALSE);
     }
		break;
#endif

#ifdef ENABLE_UART6
		case UART6:
    if (U6STAbits.OERR != 0)
     {
      U6STAbits.OERR = 0;
      return(FALSE);
     }
		break;
#endif
	}
 return(TRUE);
}




//IFS0CLR = UART1_ALL_INTERRUPT;			// clear any existing event

#endif	/* __SERIAL__ */
