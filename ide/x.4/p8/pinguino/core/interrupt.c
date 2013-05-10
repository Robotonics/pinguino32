/*	----------------------------------------------------------------------------
    FILE:			interrupt.c
    PROJECT:		pinguino
    PURPOSE:		interrupt routines
    PROGRAMER:		regis blanchot <rblanchot@gmail.com>
    FIRST RELEASE:	24-12-2010
    LAST RELEASE:	24-01-2013
    ----------------------------------------------------------------------------
    CHANGELOG :
    23-11-2012		rblanchot	added __18f1220,1320,14k22,2455,4455,46j50 support
    24-01-2013		rblanchot	added IntEnable and IntDisable functions
                                added partial support to RTCC interrupt
    ----------------------------------------------------------------------------
    TODO :
    * x7j53 family support
    * INT3
    * INTEDG0, INTEDG1, INTEDG2, INTEDG3
    * BCL1, BCL2
    * CM1, CM2
    * RX1, RX2
    * TX1, TX2
    * SSP1, SSP2
    * CTMU
    * RTCC
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

#ifndef __INTERRUPT_C
	#define __INTERRUPT_C

	#ifndef ON_EVENT
		#define ON_EVENT
	#endif

    #if !defined(__18f1220) && !defined(__18f1320) && \
        !defined(__18f14k22) && !defined(__18lf14k22) && \
        !defined (__18f2455) && !defined (__18f4455) && \
        !defined (__18f2550) && !defined (__18f4550) && \
        !defined (__18f25k50) && !defined (__18f45k50) && \
        !defined(__18f26j50) && !defined(__18f46j50)
        #error "Error : this library is intended to be used only with 8-bit Pinguino" 
    #endif
    
	#include <pic18fregs.h>
	#include <typedef.h>
	#include <interrupt.h>
	#include <macro.h>
    #include <oscillator.c>

	typedef void (*callback) (void);				// type of: void callback()

	static callback intFunction[INT_NUM];
	u8  intUsed[INT_NUM];
	volatile u16 intCount[4];
	volatile u16 intCountLimit[4];
	volatile u8 preloadH[4];
	volatile u8 preloadL[4];

/*	----------------------------------------------------------------------------
	---------- Attach / Detach Interrupt
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Enable / Disable the interrupt
	@param		inter:		interrupt number
	--------------------------------------------------------------------------*/

void IntSetEnable(u8 inter, u8 enable)
{
    if (enable == INT_DISABLE)
        intUsed[inter] = INT_NOT_USED;	// This interrupt is no longer used
    else
        intUsed[inter] = INT_USED;	    // This interrupt is used
        
    switch(inter)
	{
		case INT_INT0:
			INTCONbits.INT0IE = enable;
			break;
		case INT_INT1:
			INTCON3bits.INT1IE = enable;
			break;
		case INT_INT2:
			INTCON3bits.INT2IE = enable;
			break;
		case INT_TMR0:
			INTCONbits.TMR0IE = enable;
			break;
		case INT_TMR1:
			PIE1bits.TMR1IE = enable;
			break;
		case INT_TMR2:
			PIE1bits.TMR2IE = enable;
			break;
		case INT_TMR3:
			PIE2bits.TMR3IE = enable;
			break;
        #if defined(__18f26j50) || defined(__18f46j50)
		case INT_TMR4:
			PIE3bits.TMR4IE = enable;
			break;
        #endif
		case INT_RB:
		#if defined(__18f25k50) || defined(__18f45k50)
			INTCONbits.IOCIE = enable;
        #else
			INTCONbits.RBIE = enable;
        #endif
			break;
		#if !defined(__18f1220) && !defined(__18f1320) && \
            !defined(__18f14k22) && !defined(__18lf14k22)
		case INT_USB:
		#if defined(__18f25k50) || defined(__18f45k50)
			PIE3bits.USBIE = enable;
        #else
			PIE2bits.USBIE = enable;
        #endif
			break;
        #endif
		case INT_AD:
			PIE1bits.ADIE = enable;
			break;
		case INT_RC:
			PIE1bits.RCIE = enable;
			break;
		case INT_TX:
			PIE1bits.TXIE = enable;
			break;
		case INT_CCP1:
			PIE1bits.CCP1IE = enable;
			break;
		#if !defined(__18f1220) && !defined(__18f1320) && \
            !defined(__18f14k22) && !defined(__18lf14k22)
		case INT_CCP2:
			PIE2bits.CCP2IE = enable;
			break;
        #endif
		case INT_OSCF:
			PIE2bits.OSCFIE = enable;
			break;
		case INT_CM:
		#if defined(__18f4550) || defined(__18f2550)
			PIE2bits.CMIE = enable;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
			PIE2bits.CM1IE = enable;
		#endif
			break;
		#if defined(__18f4550) || defined(__18f2550)
		case INT_EE:
			PIE2bits.EEIE = enable;
			break;
		#endif
		case INT_BCL:
		#if defined(__18f4550) || defined(__18f2550)
			PIE2bits.BCLIE = enable;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
			PIE2bits.BCL1IE = enable;
		#endif
			break;
		case INT_HLVD:
		#if defined(__18f4550) || defined(__18f2550)
			PIE2bits.HLVDIE = enable;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
			PIE2bits.LVDIE = enable;
		#endif
			break;
		#if defined(__18f4550)
		case INT_SSP:
			PIE1bits.SSPIE = enable;
			break;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
		case INT_RTCC:
			PIE3bits.RTCCIE = enable;
			break;
		#endif
	}
}

void IntDisable(u8 inter)
{
    IntSetEnable(inter, INT_DISABLE);
}

void IntEnable(u8 inter)
{
    IntSetEnable(inter, INT_ENABLE);
    INTCONbits.GIEH = 1;   // Enable global HP interrupts
    INTCONbits.GIEL = 1;   // Enable global LP interrupts
}

/*	----------------------------------------------------------------------------
	---------- IntClearFlag
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Clear interrupt flag to allow interrupt again
	@param		inter:		interrupt number
	--------------------------------------------------------------------------*/

void IntClearFlag(u8 inter)
{
	switch(inter)
	{
		case INT_INT0:
			INTCONbits.INT0IF = 0;
			break;
		case INT_INT1:
			INTCON3bits.INT1IF = 0;
			break;
		case INT_INT2:
			INTCON3bits.INT2IF = 0;
			break;
		case INT_TMR0:
			INTCONbits.TMR0IF = 0;
			break;
		case INT_TMR1:
			PIR1bits.TMR1IF = 0;
			break;
		case INT_TMR2:
			PIR1bits.TMR2IF = 0;
			break;
		case INT_TMR3:
			PIR2bits.TMR3IF = 0;
			break;
        #if defined(__18f26j50) || defined(__18f46j50)
		case INT_TMR4:
			PIR3bits.TMR4IF = 0;
			break;
        #endif
		case INT_RB:
		#if defined(__18f25k50) || defined(__18f45k50)
			INTCONbits.IOCIF = 0;
        #else
			INTCONbits.RBIF = 0;
        #endif
			break;
		#if !defined(__18f1220) && !defined(__18f1320) && \
            !defined(__18f14k22) && !defined(__18lf14k22)
		case INT_USB:
		#if defined(__18f25k50) || defined(__18f45k50)
			PIR3bits.USBIF = 0;
        #else
			PIR2bits.USBIF = 0;
        #endif
			break;
        #endif
		case INT_AD:
			PIR1bits.ADIF = 0;
			break;
		case INT_RC:
			PIR1bits.RCIF = 0;
			break;
		case INT_TX:
			PIR1bits.TXIF = 0;
			break;
		case INT_CCP1:
			PIR1bits.CCP1IF = 0;
			break;
		#if !defined(__18f1220) && !defined(__18f1320) && \
            !defined(__18f14k22) && !defined(__18lf14k22)
		case INT_CCP2:
			PIR2bits.CCP2IF = 0;
			break;
        #endif
		case INT_OSCF:
			PIR2bits.OSCFIF = 0;
			break;
		case INT_CM:
		#if defined(__18f4550) || defined(__18f2550)
			PIR2bits.CMIF = 0;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
			PIR2bits.CM1IF = 0;
		#endif
			break;
		#if defined(__18f4550) || defined(__18f2550)
		case INT_EE:
			PIR2bits.EEIF = 0;
			break;
		#endif
		case INT_BCL:
		#if defined(__18f4550) || defined(__18f2550)
			PIR2bits.BCLIF = 0;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
			PIR2bits.BCL1IF = 0;
		#endif
			break;
		case INT_HLVD:
		#if defined(__18f4550) || defined(__18f2550)
			PIR2bits.HLVDIF = 0;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
			PIR2bits.LVDIF = 0;
		#endif
			break;
		#if defined(__18f4550)
		case INT_SSP:
			PIR1bits.SSPIF = 0;
			break;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
		case INT_RTCC:
			PIR3bits.RTCCIF = 0;
			break;
		#endif
	}
}

/*	----------------------------------------------------------------------------
	---------- IntIsFlagSet
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Read if interrupt flag is set
	@param		inter:		interrupt number
    @return     1 if set, 0 if not set
	--------------------------------------------------------------------------*/

u8 IntIsFlagSet(u8 inter)
{
	switch(inter)
	{
		case INT_INT0:  return INTCONbits.INT0IF;
		case INT_INT1:	return INTCON3bits.INT1IF;
		case INT_INT2:	return INTCON3bits.INT2IF;
		case INT_TMR0:	return INTCONbits.TMR0IF;
		case INT_TMR1:	return PIR1bits.TMR1IF;
		case INT_TMR2:	return PIR1bits.TMR2IF;
		case INT_TMR3:	return PIR2bits.TMR3IF;
        #if defined(__18f26j50) || defined(__18f46j50)
		case INT_TMR4:	return PIR3bits.TMR4IF;
        #endif
		case INT_RB:
		#if defined(__18f25k50) || defined(__18f45k50)
        	return INTCONbits.IOCIF;
        #else
			return INTCONbits.RBIF;
        #endif
    #if !defined(__18f1220) && !defined(__18f1320) && \
        !defined(__18f14k22) && !defined(__18lf14k22)
		case INT_USB:
		#if defined(__18f25k50) || defined(__18f45k50)
			return  PIR3bits.USBIF;
        #else
			return  PIR2bits.USBIF;
        #endif
    #endif
		case INT_AD:	return PIR1bits.ADIF;
		case INT_RC:	return PIR1bits.RCIF;
		case INT_TX:	return PIR1bits.TXIF;
		case INT_CCP1:	return PIR1bits.CCP1IF;
    #if !defined(__18f1220) && !defined(__18f1320) && \
        !defined(__18f14k22) && !defined(__18lf14k22)
		case INT_CCP2:	return PIR2bits.CCP2IF;
    #endif
		case INT_OSCF:	return PIR2bits.OSCFIF;
		case INT_CM:
		#if defined(__18f4550) || defined(__18f2550)
        	return PIR2bits.CMIF;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
        	return PIR2bits.CM1IF;
		#endif
		#if defined(__18f4550) || defined(__18f2550)
		case INT_EE:	return PIR2bits.EEIF;
		#endif
		case INT_BCL:
		#if defined(__18f4550) || defined(__18f2550)
        	return PIR2bits.BCLIF;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
        	return PIR2bits.BCL1IF;
		#endif
		case INT_HLVD:
		#if defined(__18f4550) || defined(__18f2550)
        	return PIR2bits.HLVDIF;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
        	return PIR2bits.LVDIF;
		#endif
		#if defined(__18f4550)
		case INT_SSP:	return PIR1bits.SSPIF;
		#endif
		#if defined(__18f26j50) || defined(__18f46j50)
		case INT_RTCC:	return PIR3bits.RTCCIF;
		#endif
	}
    return 0;
}

/*	----------------------------------------------------------------------------
	---------- int_init
	----------------------------------------------------------------------------
	@author		Régis Blanchot <rblanchot@gmail.com>
	@descr		Disable all the interrupt
	--------------------------------------------------------------------------*/

void IntInit()
{
	u8 i;

	RCONbits.IPEN = 1;					// Enable HP/LP interrupts

    INTCONbits.GIEH = 0;                // Disable global HP interrupts
    INTCONbits.GIEL = 0;                // Disable global LP interrupts

	for (i = 0; i < INT_NUM; i++)		// Disable all interrupts
		IntSetEnable(i, INT_DISABLE);

	INTCONbits.GIEH = 1;				// Enable HP interrupts
	INTCONbits.GIEL = 1;				// Enable LP interrupts
}

/*	----------------------------------------------------------------------------
	---------- int_start
	----------------------------------------------------------------------------
	@author		Régis Blanchot <rblanchot@gmail.com>
	@descr		Start all timers together
	--------------------------------------------------------------------------*/

void IntTimerStart()
{
	#ifdef TMR0INT
		T0CONbits.TMR0ON = ON;
	#endif

	#ifdef TMR1INT
		T1CONbits.TMR1ON = ON;
	#endif

	#ifdef TMR2INT
		T2CONbits.TMR2ON = ON;
	#endif

	#ifdef TMR3INT
		T3CONbits.TMR3ON = ON;
	#endif

	#ifdef TMR4INT
	#if defined(__18f26j50) || defined(__18f46j50)
		T4CONbits.TMR4ON = ON;
	#endif
	#endif
}

/*	----------------------------------------------------------------------------
	---------- int_stop
	----------------------------------------------------------------------------
	@author		Régis Blanchot <rblanchot@gmail.com>
	@descr		Stop all timers together
	--------------------------------------------------------------------------*/

void IntTimerStop()
{
	#ifdef TMR0INT
		T0CONbits.TMR0ON = OFF;
	#endif

	#ifdef TMR1INT
		T1CONbits.TMR1ON = OFF;
	#endif

	#ifdef TMR2INT
		T2CONbits.TMR2ON = OFF;
	#endif

	#ifdef TMR3INT
		T3CONbits.TMR3ON = OFF;
	#endif

	#ifdef TMR4INT
		#if defined(__18f26j50) || defined(__18f46j50)
		T4CONbits.TMR4ON = OFF;
        #endif
	#endif
}

/*	----------------------------------------------------------------------------
	---------- OnTimerX
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Configure timers to execute function func every delay ms, us or sec
	@param		timediv:	INT_MICROSEC, INT_MILLISEC, INT_SEC
				func:		function called when interrupt occures
				delay:		delay before overload
	--------------------------------------------------------------------------*/

#ifdef TMR0INT
u8 OnTimer0(callback func, u8 timediv, u16 delay)
{
	u8 _t0con = 0;
    u16 _cycles_;
    
	if (intUsed[INT_TMR0] == INT_NOT_USED)
	{
		intUsed[INT_TMR0] = INT_USED;
		intCount[INT_TMR0] = 0;
		intCountLimit[INT_TMR0] = delay;
		intFunction[INT_TMR0] = func;

		switch(timediv)
		{
			case INT_MICROSEC:
				// 1 us = 1.000 ns = 12 cy
                _cycles_ = SystemGetInstructionClock() / 1000 / 1000;
				preloadH[INT_TMR0] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR0] =  low8(0xFFFF - _cycles_);
				_t0con = T0_OFF | T0_16BIT | T0_SOURCE_INT | T0_PS_OFF;
				break;
			case INT_MILLISEC:
				// 1 ms = 1.000.000 ns = 12.000 cy
                _cycles_ = SystemGetInstructionClock() / 1000 ;
				preloadH[INT_TMR0] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR0] =  low8(0xFFFF - _cycles_);
				_t0con = T0_OFF | T0_16BIT | T0_SOURCE_INT | T0_PS_OFF;
				break;
			case INT_SEC:
				// 1 sec = 1.000.000.000 ns = 12.000.000 cy
				// 12.000.000 / 256 = 46875
                _cycles_ = SystemGetInstructionClock() >> 8;
				preloadH[INT_TMR0] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR0] =  low8(0xFFFF - _cycles_);
				_t0con = T0_OFF | T0_16BIT | T0_SOURCE_INT | T0_PS_ON | T0_PS_1_256;
				break;
		}

		T0CON = _t0con;
		INTCON2bits.TMR0IP = INT_LOW_PRIORITY;
		TMR0H = preloadH[INT_TMR0];
		TMR0L = preloadL[INT_TMR0];
		INTCONbits.TMR0IF = 0;
		T0CONbits.TMR0ON = 1;
		INTCONbits.TMR0IE = INT_ENABLE;
		return INT_TMR0;
	}
	else
	{
	#ifdef DEBUG
		debug("Error : interrupt TIMER0 is already used !");
	#endif
		return INT_USED;
	}
}
#endif

#ifdef TMR1INT
u8 OnTimer1(callback func, u8 timediv, u16 delay)
{
	u8 _t1con = 0;
    u16 _cycles_;

	if (intUsed[INT_TMR1] == INT_NOT_USED)
	{
		intUsed[INT_TMR1] = INT_USED;
		intCount[INT_TMR1] = 0;
		intCountLimit[INT_TMR1] = delay;
		intFunction[INT_TMR1] = func;

		switch(timediv)
		{
			case INT_MICROSEC:
				// 1us = 1.000 ns = 12 cy
                _cycles_ = SystemGetInstructionClock() / 1000 / 1000;
				preloadH[INT_TMR1] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR1] =  low8(0xFFFF - _cycles_);
                #if defined(__18f26j50) || defined(__18f46j50)
				_t1con = T1_OFF | T1_16BIT | T1_PS_1_1 | T1_OSC_OFF | T1_SYNC_EXT_OFF | T1_SOURCE_INT;
                #else
				_t1con = T1_OFF | T1_16BIT | T1_PS_1_1 | T1_RUN_FROM_ANOTHER | T1_OSC_OFF | T1_SYNC_EXT_OFF | T1_SOURCE_INT;
                #endif
				break;
			case INT_MILLISEC:
				// 1ms = 1.000.000ns = 12.000 cy
                _cycles_ = SystemGetInstructionClock() / 1000;
				preloadH[INT_TMR1] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR1] =  low8(0xFFFF - _cycles_);
                #if defined(__18f26j50) || defined(__18f46j50)
				_t1con = T1_OFF | T1_16BIT | T1_PS_1_1 | T1_OSC_OFF | T1_SYNC_EXT_OFF | T1_SOURCE_INT;
                #else
				_t1con = T1_OFF | T1_16BIT | T1_PS_1_1 | T1_RUN_FROM_ANOTHER | T1_OSC_OFF | T1_SYNC_EXT_OFF | T1_SOURCE_INT;
                #endif
				break;
			case INT_SEC:
				// 1 sec = 1.000.000.000 ns = 12.000.000 cy
				// 12.000.000 / 8 = 1.500.000
				// 1.500.000 / 25 = 60000
                _cycles_ = SystemGetInstructionClock() / 8 / 25;
				preloadH[INT_TMR1] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR1] =  low8(0xFFFF - _cycles_);
				intCountLimit[INT_TMR1] = delay * 25;
                #if defined(__18f26j50) || defined(__18f46j50)
				_t1con = T1_OFF | T1_16BIT | T1_PS_1_8 | T1_OSC_OFF | T1_SYNC_EXT_OFF | T1_SOURCE_INT;
                #else
				_t1con = T1_OFF | T1_16BIT | T1_PS_1_8 | T1_RUN_FROM_ANOTHER | T1_OSC_OFF | T1_SOURCE_INT;
                #endif
				break;
		}
		#if defined(__18f26j50) || defined(__18f46j50)
		T1GCONbits.TMR1GE = 0;				/* First Ignore T1DIG effection */ 
	    #endif
		T1CON = _t1con;
		IPR1bits.TMR1IP = INT_LOW_PRIORITY;
		TMR1H = preloadH[INT_TMR1];
		TMR1L = preloadL[INT_TMR1];
		PIR1bits.TMR1IF = 0;
		T1CONbits.TMR1ON=1;					/* Explicitly timer start */
		PIE1bits.TMR1IE = INT_ENABLE;
		return INT_TMR1;
	}
	else
	{
	#ifdef DEBUG
		debug("Error : interrupt TIMER1 is already used !");
	#endif
		return INT_USED;
	}
}

void OnRTCC(callback func, u16 delay)
{
#if defined(__18f2550) || defined(__18f4550)
	u8 _t1con = 0;

	if (intUsed[INT_TMR1] == INT_NOT_USED)
	{
		intUsed[INT_TMR1] = INT_USED;
		intCount[INT_TMR1] = 0;
		intCountLimit[INT_TMR1] = delay;
		intFunction[INT_TMR1] = func;

		// Preload TMR1 register pair for 1 mn (60 s = 60 x 1000 x 4 x 250 us) overflow
		preloadH[INT_TMR1] = 0x80;			// (0xFFFF - 0xFF0F) = 240
		preloadL[INT_TMR1] = 0x00;			// TMR1L is never preloaded or altered

		// RD16    = 1	Enables register read/write of Timer1 in one 16-bit operation
		// T1RUN   = 0	Device clock is derived from another source (le quartz 20MHz)
		// T1CKPS1 = 0	00 = 1:1 Prescale value
		// T1CKPS0 = 0
		// T1OSCEN = 1	Timer1 oscillator is on
		// T1SYNC  = 0	When TMR1CS = 1, Synchronize external clock input
		// TMR1CS  = 1	External clock from RC0/T1OSO/T13CKI pin (on the rising edge)
		// TMR1ON  = 0	Stops Timer1
		_t1con = T1_OFF | T1_16BIT | T1_PS_1_1 | T1_RUN_FROM_ANOTHER | T1_OSC_ON | T1_SYNC_EXT_ON | T1_SOURCE_EXT;
		// 18F26J50 -> _t1con = T1_OFF | T1_16BIT | T1_PS_1_1 | T1_OSC_ON | T1_SYNC_EXT_ON | T1_SOURCE_EXT;

		IPR1bits.TMR1IP = INT_LOW_PRIORITY;
		PIE1bits.TMR1IE = INT_ENABLE;
		PIR1bits.TMR1IF = 0;
		TMR1H = preloadH[INT_TMR1];
		TMR1L = preloadL[INT_TMR1];
		T1CON = _t1con;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt TIMER1 is already used !");
	}
	#endif
#endif

#if defined(__18f26j50) || defined(__18f46j50)
// TODO
#endif
}

#endif

#ifdef TMR2INT
u8 OnTimer2(callback func, u8 timediv, u16 delay)
{
	u8 _t2con = 0;
	u8 _pr2 = 0;

	if (intUsed[INT_TMR2] == INT_NOT_USED)
	{
		intUsed[INT_TMR2] = INT_USED;
		intCount[INT_TMR2] = 0;
		intCountLimit[INT_TMR2] = delay;
		intFunction[INT_TMR2] = func;

		// time = Tcy*postscaler*pr2*prescaler
		switch(timediv)
		{
			case INT_MICROSEC:
				// 1us = 12 cy
				_pr2 = SystemGetInstructionClock() / 1000 / 1000;
				_t2con = T2_OFF | T2_PS_1_1 | T2_POST_1_1;
				break;
			case INT_MILLISEC:
				// 1ms = 12.000 cy
				// 12.000 / 15 / 16 = 50
				_pr2 = SystemGetInstructionClock() / 1000 / 240;
				_t2con = T2_OFF | T2_POST_1_15 | T2_PS_1_16;
				break;
			case INT_SEC:
				// 1sec = 12.000.000 cy
				// 12.000.000 / 15 / 16 = 50.000 = 200 * 250
				_pr2 = SystemGetInstructionClock() / 240 / 200;
				intCountLimit[INT_TMR2] = delay * 200;
				_t2con = T2_OFF | T2_POST_1_15 | T2_PS_1_16;
				break;
		}

		T2CON = _t2con;
		PR2 = _pr2;	// Timer2 Match value
		IPR1bits.TMR2IP = INT_LOW_PRIORITY;
		PIR1bits.TMR2IF = 0;
		T2CONbits.TMR2ON = 1;
		PIE1bits.TMR2IE = INT_ENABLE;
		return INT_TMR2;
	}
	else
	{
	#ifdef DEBUG
		debug("Error : interrupt TIMER2 is already used !");
	#endif
		return INT_USED;
	}
}
#endif

#ifdef TMR3INT
/* Note: This function needs T1OSC as its clock source */
u8 OnTimer3(callback func, u8 timediv, u16 delay)
{
	u8 _t3con = 0;
    u16 _cycles_;

	if (intUsed[INT_TMR3] == INT_NOT_USED)
	{
		intUsed[INT_TMR3] = INT_USED;
		intCount[INT_TMR3] = 0;
		intCountLimit[INT_TMR3] = delay;
		intFunction[INT_TMR3] = func;

		switch(timediv)
		{
            // nb : T3_SOURCE_INT => Fosc/4
            // 1 cy = 1/(Fosc/4)=4/Fosc=4/48MHz=83ns
			case INT_MICROSEC:
				// 1us = 1.000 ns = 12 cy
                _cycles_ = SystemGetInstructionClock() / 1000 / 1000;
				preloadH[INT_TMR3] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR3] =  low8(0xFFFF - _cycles_);
                #if defined(__18f26j50) || defined(__18f46j50)
				_t3con = T3_OFF | T3_16BIT | T3_PS_1_1 | T3_SOURCE_INT | T3_SOURCE_T1OSC;
                #else
				_t3con = T3_OFF | T3_16BIT | T3_PS_1_1 | T3_SOURCE_INT;
                #endif
				break;
			case INT_MILLISEC:
				// 1 ms = 1.000.000 ns = 12.000 cy at Fosc/4
                _cycles_ = SystemGetInstructionClock() / 1000;
				preloadH[INT_TMR3] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR3] =  low8(0xFFFF - _cycles_);
                #if defined(__18f26j50) || defined(__18f46j50)
				_t3con = T3_OFF | T3_16BIT | T3_PS_1_1 | T3_SOURCE_INT | T3_SOURCE_T1OSC;
                #else
				_t3con = T3_OFF | T3_16BIT | T3_PS_1_1 | T3_SOURCE_INT;
                #endif
				break;
			case INT_SEC:
				// 1 sec = 1.000.000.000 ns = 12.000.000 cy
                _cycles_ = SystemGetInstructionClock() / 8 / 25;
				preloadH[INT_TMR0] = high8(0xFFFF - _cycles_);
				preloadL[INT_TMR0] =  low8(0xFFFF - _cycles_);
				intCountLimit[INT_TMR3] = delay * 25;
                #if defined(__18f26j50) || defined(__18f46j50)
				_t3con = T3_OFF | T3_16BIT | T3_PS_1_8 | T3_SOURCE_INT | T3_SOURCE_T1OSC;
                #else
				_t3con = T3_OFF | T3_16BIT | T3_PS_1_8 | T3_SOURCE_INT;
                #endif
				break;
		}

		T3CON = _t3con;
		IPR2bits.TMR3IP = INT_LOW_PRIORITY;
		TMR3H = preloadH[INT_TMR3];
		TMR3L = preloadL[INT_TMR3];
		PIR2bits.TMR3IF = 0;
		T3CONbits.TMR3ON = 1;
		PIE2bits.TMR3IE = INT_ENABLE;
		return INT_TMR3;
	}
	else
	{
	#ifdef DEBUG
		debug("Error : interrupt TIMER3 is already used !");
	#endif
		return INT_USED;
	}
}
#endif

#ifdef TMR4INT
#if defined(__18f26j50) || defined(__18f46j50)
u8 OnTimer4(callback func, u8 timediv, u16 delay)
{
	u8 _t4con = 0;
	u8 _pr4 = 0;

	if (intUsed[INT_TMR4] == INT_NOT_USED)
	{
		intUsed[INT_TMR4] = INT_USED;
		intCount[INT_TMR4] = 0;
		intCountLimit[INT_TMR4] = delay;
		intFunction[INT_TMR4] = func;

		// time = Tcy*postscaler*pr2*prescaler
		switch(timediv)
		{
			case INT_MICROSEC:
				// 1us = 12 cy
				_pr4 = SystemGetInstructionClock() / 1000 / 1000;
				_t4con = T4_OFF | T4_PS_1_1 | T4_POST_1_1;
				break;
			case INT_MILLISEC:
				// 1ms = 12.000 cy
				// 12.000 / 15 / 16 = 50
				_pr4 = SystemGetInstructionClock() / 1000 / 240;
				_t4con = T4_OFF | T4_POST_1_15 | T4_PS_1_16;
				break;
			case INT_SEC:
				// 1sec = 12.000.000 cy
				// 12.000.000 / 15 / 16 = 50.000 = 200 * 25
				_pr4 = SystemGetInstructionClock() / 240 / 200;
				intCountLimit[INT_TMR4] = delay * 200;
				_t4con = T4_OFF | T4_POST_1_15 | T4_PS_1_16;
				break;
		}

		T4CON = _t4con;
		IPR3bits.TMR4IP = INT_LOW_PRIORITY;
		PR4 = _pr4;	// Timer2 Match value
		PIR3bits.TMR4IF = 0;
		T4CONbits.TMR4ON = 1;
		PIE3bits.TMR4IE = INT_ENABLE;
		return INT_TMR4;
	}
	else
	{
	#ifdef DEBUG
		debug("Error : interrupt TIMER4 is already used !");
	#endif /* DEBUG */
		return INT_USED;
	}
}
#else
#error "Your processor don't have any Timer4."
#endif /* defined(__18f26j50) || defined(__18f46j50) */
#endif /* TMR4INT */

/*	----------------------------------------------------------------------------
	---------- OnCounter
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Configure counters
	@param		inter:		interrupt number (INT_TMR0, INT_TMR1, or INT_TMR3)
				func:		function called when interrupt comes
	--------------------------------------------------------------------------*/

#ifdef CNTR0INT
// Not available on Pinguino with 2 buttons (RA4 = Run Led)
void OnCounter0(callback func, u8 config)
{
	if (intUsed[INT_TMR0] == INT_NOT_USED)
	{
		intUsed[INT_TMR0] = INT_USED;
		intFunction[INT_TMR0] = func;
		intCount[INT_TMR0] = 0;
		intCountLimit[INT_TMR0] = 0;
		INTCON2bits.TMR0IP = INT_LOW_PRIORITY;
		INTCONbits.TMR0IE = INT_ENABLE;
		preloadH[INT_TMR0] = 0;
		preloadL[INT_TMR0] = 0;
		TMR0H = 0;
		TMR0L = 0;
		T0CON = T0_ON | T0_16BIT | T0_SOURCE_EXT | T0_L2H | T0_PS_ON | T0_PS_1_2;
		T0CON |= config;
		INTCONbits.TMR0IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt TMR0 is already used !");
	}
	#endif
}
#endif

#ifdef CNTR1INT
void OnCounter1(callback func, u8 config)
{
	if (intUsed[INT_TMR1] == INT_NOT_USED)
	{
		intUsed[INT_TMR1] = INT_USED;
		intFunction[INT_TMR1] = func;
		intCount[INT_TMR1] = 0;
		intCountLimit[INT_TMR1] = 0;
		IPR1bits.TMR1IP = INT_LOW_PRIORITY;
		PIE1bits.TMR1IE = INT_ENABLE;
		preloadH[INT_TMR1] = 0;
		preloadL[INT_TMR1] = 0;
		TMR1H = 0;
		TMR1L = 0;
		T1CON = T1_ON | T1_16BIT | T1_PS_1_8 | T1_RUN_FROM_ANOTHER | T1_OSC_OFF | T1_SYNC_EXT_ON | T1_SOURCE_EXT;
		T1CON |= config;
		PIR1bits.TMR1IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt TMR1 is already used !");
	}
	#endif
}
#endif

#ifdef CNTR3INT
void OnCounter3(callback func, u8 config)
{
	if (intUsed[INT_TMR3] == INT_NOT_USED)
	{
		intUsed[INT_TMR3] = INT_USED;
		intFunction[INT_TMR3] = func;
		intCount[INT_TMR3] = 0;
		intCountLimit[INT_TMR3] = 0;
		IPR2bits.TMR3IP = INT_LOW_PRIORITY;
		PIE2bits.TMR3IE = INT_ENABLE;
		preloadH[INT_TMR3] = 0;
		preloadL[INT_TMR3] = 0;
		TMR3H = 0;
		TMR3L = 0;
		T3CON = T3_ON | T3_16BIT | T3_PS_1_8 | T3_SOURCE_EXT; // default
		T3CON |= config;
		PIR2bits.TMR3IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt TMR3 is already used !");
	}
	#endif
}
#endif

/*	----------------------------------------------------------------------------
	---------- OnChange
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Configure interrupt-on-change
	@param		inter:		interrupt number
				config:		INT_RISING_EDGE or INT_FALLING_EDGE
				func:		function called when interrupt comes
	--------------------------------------------------------------------------*/

#ifdef INT0INT
void OnChangePin0(callback func, u8 config)
{
	if (intUsed[INT_INT0] == INT_NOT_USED)
	{
		intUsed[INT_INT0] = INT_USED;
		intFunction[INT_INT0] = func;
		INTCON2bits.INTEDG0 = config;
		INTCON2bits.RBPU = 0;						// PORTB pull-ups are enabled
		TRISBbits.TRISB0 = INPUT;
		//INTCON3bits.INT0IP = INT_LOW_PRIORITY;
		INTCONbits.INT0IE = INT_ENABLE;
		INTCONbits.INT0IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt INT0 is already used !");
	}
	#endif
}
#endif

#ifdef INT1INT
void OnChangePin1(callback func, u8 config)
{
	if (intUsed[INT_INT1] == INT_NOT_USED)
	{
		intUsed[INT_INT1] = INT_USED;
		intFunction[INT_INT1] = func;
		INTCON2bits.INTEDG1 = config;
		INTCON2bits.RBPU = 0;						// PORTB pull-ups are enabled
		TRISBbits.TRISB1 = INPUT;
		INTCON3bits.INT1IP = INT_LOW_PRIORITY;
		INTCON3bits.INT1IE = INT_ENABLE;
		INTCON3bits.INT1IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt INT1 is already used !");
	}
	#endif
}
#endif

#ifdef INT2INT
void OnChangePin2(callback func, u8 config)
{
	if (intUsed[INT_INT2] == INT_NOT_USED)
	{
		intUsed[INT_INT2] = INT_USED;
		intFunction[INT_INT2] = func;
		INTCON2bits.INTEDG2 = config;
		INTCON2bits.RBPU = 0;						// PORTB pull-ups are enabled
		TRISBbits.TRISB2 = INPUT;
		INTCON3bits.INT2IP = INT_LOW_PRIORITY;
		INTCON3bits.INT2IE = INT_ENABLE;
		INTCON3bits.INT2IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt INT2 is already used !");
	}
	#endif
}
#endif

#ifdef RBINT
/*
PORTB<7:4> Interrupt-on-Change
Only pins configured as inputs can
cause this interrupt to occur. Any RB7:RB4 pin
configured as an output is excluded from the interrupt-
on-change comparison. The pins are compared with
the old value latched on the last read of PORTB.
*/

void OnChangePin4to7(callback func, u8 pin, u8 config)
{
	if (intUsed[INT_RB] == INT_NOT_USED)
	{
		intUsed[INT_RB] = INT_USED;
		intFunction[INT_RB] = func;
		INTCON2bits.INTEDG2 = config;
		TRISB |= (1 << pin);	// pin as an INPUT
		INTCON2bits.RBIP = INT_LOW_PRIORITY;
		INTCONbits.RBIE = INT_ENABLE;
		INTCONbits.RBIF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt RB is already used !");
	}
	#endif
}
#endif

/*	----------------------------------------------------------------------------
	---------- OnCompare
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Configure compare interrupt mode
	@param		inter:		interrupt number
				func:		function called when interrupt occured
	--------------------------------------------------------------------------*/

#ifdef CCP1INT
void OnCompare1(callback func, u8 config)
{
	if (intUsed[INT_CCP1] == INT_NOT_USED)
	{
		intUsed[INT_CCP1] = INT_USED;
		intFunction[INT_CCP1] = func;
		IPR1bits.CCP1IP = INT_LOW_PRIORITY;
		PIE1bits.CCP1IE = INT_ENABLE;
		CCP1CON &= config;
		PIR1bits.CCP1IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt CCP1 is already used !");
	}
	#endif
}
#endif

#ifdef CCP2INT
void OnCompare2(callback func, u8 config)
{
	if (intUsed[INT_CCP2] == INT_NOT_USED)
	{
		intUsed[INT_CCP2] = INT_USED;
		intFunction[INT_CCP2] = func;
		IPR2bits.CCP2IP = INT_LOW_PRIORITY;
		PIE2bits.CCP2IE = INT_ENABLE;
		CCP2CON &= config;
		PIR2bits.CCP2IF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt CCP2 is already used !");
	}
	#endif
}
#endif

#ifdef CMINT
void OnCompareAll(callback func, u8 config)
{
	if (intUsed[INT_CM] == INT_NOT_USED)
	{
		intUsed[INT_CM] = INT_USED;
		intFunction[INT_CM] = func;
		IPR2bits.CMIP = INT_LOW_PRIORITY;
		PIE2bits.CMIE = INT_ENABLE;
		PIR2bits.CMIF = 0;
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt CM is already used !");
	}
	#endif
}
#endif

/*	----------------------------------------------------------------------------
	---------- OnEvent
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		Configure interrupt event
	@param		inter:		interrupt number
				func:		function called when interrupt occured
	--------------------------------------------------------------------------*/

#if defined (RCINT) || defined(TXINT) || defined(ADINT) || defined(OSCFINT) || defined(EEINT) || defined(HLVDINT) || defined(BCLINT) || defined(USBINT) || defined(SSPINT)
void OnEvent(u8 inter, callback func)
{
	if (intUsed[inter] == INT_NOT_USED)
	{
		intUsed[inter] = INT_USED;
		intFunction[inter] = func;

		switch(inter)
		{
			case INT_RC:					// serial rx
				PIR1bits.RCIF = 0;
				break;
			case INT_TX:					// serial tx
				PIR1bits.TXIF = 0;
				break;
			case INT_AD:					// ad conversion complete
				PIR1bits.ADIF = 0;
				break;
			case INT_OSCF:					// oscillator failed
				PIR2bits.OSCFIF = 0;
				break;
			case INT_EE:					// eeprom write operation
				PIR2bits.EEIF = 0;
				break;
			case INT_HLVD:					// high/low voltage detect
				PIR2bits.HLVDIF = 0;
				break;
			case INT_BCL:					// bus collision
				PIR2bits.BCLIF = 0;
				break;
			case INT_USB:					// usb
            #if defined(__18f25k50) || defined(__18f45k50)
                PIR3bits.USBIF = 0;
            #else
				PIR2bits.USBIF = 0;
            #endif
				break;
			case INT_SSP:
				PIR1bits.SSPIF = 0;
				break;
		}
	}
	#ifdef DEBUG
	else
	{
		debug("Error : interrupt #%u is already used !", inter);
	}
	#endif
}
#endif

#ifdef RCINT
void OnSerialRX(callback func)	{	OnEvent(INT_RC, func);	}
#endif

#ifdef TXINT
void OnSerialTX(callback func)	{	OnEvent(INT_TX, func);	}
#endif

#ifdef ADINT
void OnADC(callback func)		{	OnEvent(INT_AD, func);	}
#endif

#ifdef OSCFINT
void OnOscFailed(callback func)	{	OnEvent(INT_OSCF, func);}
#endif

#ifdef EEINT
void OnEeprom(callback func)	{	OnEvent(INT_EE, func);	}
#endif

#ifdef HLVDINT
void OnLowVoltage(callback func){	OnEvent(INT_HLVD, func);}
#endif

#ifdef BCLINT
void OnBusCol(callback func)	{	OnEvent(INT_BCL, func);	}
#endif

#ifdef USBINT
void OnUSB(callback func)		{	OnEvent(INT_USB, func);	}
#endif

#ifdef SSPINT
void OnParallel(callback func)	    {	OnEvent(INT_SSP, func);	}
void OnI2CRequest(callback func)	{	OnEvent(INT_SSP, func);	}
void OnI2CReceive(callback func)	{	OnEvent(INT_SSP, func);	}
#endif

/*	----------------------------------------------------------------------------
	---------- userhighinterrupt
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		function called by high_priority_isr
	@param		none
	--------------------------------------------------------------------------*/

void userhighinterrupt()
{
	#ifdef INT0INT
	if (INTCONbits.INT0IE && INTCONbits.INT0IF)
	{
		intFunction[INT_INT0]();
		INTCONbits.INT0IF = 0;
	}
	#endif
}

/*	----------------------------------------------------------------------------
	---------- userlowinterrupt
	----------------------------------------------------------------------------
	@author		Regis Blanchot <rblanchot@gmail.com>
	@descr		function called by low_priority_isr
	@param		none
	--------------------------------------------------------------------------*/

void userlowinterrupt()
{
	#ifdef INT1INT
	if (INTCON3bits.INT1IE && INTCON3bits.INT1IF)
	{
		INTCON3bits.INT1IF = 0;
		intFunction[INT_INT1]();
	}
	#endif
	#ifdef INT2INT
	if (INTCON3bits.INT2IE && INTCON3bits.INT2IF)
	{
		INTCON3bits.INT2IF = 0;
		intFunction[INT_INT2]();
	}
	#endif
	#ifdef RBINT
	if (INTCONbits.RBIE && INTCONbits.RBIF)
	{
		INTCONbits.RBIF = 0;
		intFunction[INT_RB]();
	}
	#endif
	#if defined (TMR0INT) || defined(CNTR0INT)
	if (INTCONbits.TMR0IE && INTCONbits.TMR0IF)
	{
		//T0CONbits.TMR0ON = OFF;
		TMR0H = preloadH[INT_TMR0];
		TMR0L = preloadL[INT_TMR0];
		INTCONbits.TMR0IF = 0;
		if (intCount[INT_TMR0]++ >= intCountLimit[INT_TMR0])
		{
			intCount[INT_TMR0] = 0;
			intFunction[INT_TMR0]();
		}
	}
	#endif
	#if defined (TMR1INT) || defined(CNTR1INT)
	if (PIE1bits.TMR1IE && PIR1bits.TMR1IF)
	{
		//T1CONbits.TMR1ON = OFF;
		TMR1H = preloadH[INT_TMR1];
		TMR1L = preloadL[INT_TMR1];
		PIR1bits.TMR1IF = 0;
		if (intCount[INT_TMR1]++ >= intCountLimit[INT_TMR1])
		{
			intCount[INT_TMR1] = 0;
			intFunction[INT_TMR1]();
		}
	}
	#endif
	#ifdef TMR2INT
	if (PIE1bits.TMR2IE && PIR1bits.TMR2IF)
	{
		//T2CONbits.TMR2ON = OFF;
		PIR1bits.TMR2IF = 0;
		if (intCount[INT_TMR2]++ >= intCountLimit[INT_TMR2])
		{
			intCount[INT_TMR2] = 0;
			intFunction[INT_TMR2]();
		}
		// NB : no need to reload PR2
	}
	#endif
	#if defined (TMR3INT) || defined(CNTR3INT)
	if (PIE2bits.TMR3IE && PIR2bits.TMR3IF)
	{
		//T3CONbits.TMR3ON = OFF;
		TMR3H = preloadH[INT_TMR3];
		TMR3L = preloadL[INT_TMR3];
		PIR2bits.TMR3IF = 0;
		if (intCount[INT_TMR3]++ >= intCountLimit[INT_TMR3])
		{
			intCount[INT_TMR3] = 0;
			intFunction[INT_TMR3]();
		}
	}
	#endif
	#ifdef TMR4INT
	if (PIE3bits.TMR4IE && PIR3bits.TMR4IF)
	{
		//T4CONbits.TMR4ON = OFF;
		PIR3bits.TMR4IF = 0;
		if (intCount[INT_TMR4]++ >= intCountLimit[INT_TMR4])
		{
			intCount[INT_TMR4] = 0;
			intFunction[INT_TMR4]();
		}
		// NB : no need to reload PR4
	}
	#endif
	#ifdef ADINT
	if (PIE1bits.ADIE && PIR1bits.ADIF)
	{
		PIR1bits.ADIF = 0;
		intFunction[INT_AD]();
	}
	#endif
	#ifdef RCINT
	if (PIE1bits.RCIE && PIR1bits.RCIF)
	{
		PIR1bits.RCIF = 0;
		intFunction[INT_RC]();
	}
	#endif
	#ifdef TXINT
	if (PIE1bits.TXIE && PIR1bits.TXIF)
	{
		PIR1bits.TXIF = 0;
		intFunction[INT_TX]();
	}
	#endif
	#ifdef CCP1INT
	if (PIE1bits.CCP1IE && PIR1bits.CCP1IF)
	{
		PIR1bits.CCP1IF = 0;
		intFunction[INT_CCP1]();
	}
	#endif
	#ifdef CCP2INT
	if (PIE2bits.CCP2IE && PIR2bits.CCP2IF)
	{
		PIR2bits.CCP2IF = 0;
		intFunction[INT_CCP2]();
	}
	#endif
	#ifdef EEINT
	if (PIE2bits.EEIE && PIR2bits.EEIF)
	{
		PIR2bits.EEIF = 0;
		intFunction[INT_EE]();
	}
	#endif
	#ifdef USBINT
	if (PIE2bits.USBIE && PIR2bits.USBIF)
	{
		PIR2bits.USBIF = 0;
		intFunction[INT_USB]();
	}
	#endif
	#ifdef OSCFINT
	if (PIE2bits.OSCFIE && PIR2bits.OSCFIF)
	{
		PIR2bits.OSCFIF = 0;
		intFunction[INT_OSCF]();
	}
	#endif
	#ifdef CMINT
	if (PIE2bits.CMIE && PIR2bits.CMIF)
	{
		PIR2bits.CMIF = 0;
		intFunction[INT_CM]();
	}
	#endif
	#ifdef BCLINT
	if (PIE2bits.BCLIE && PIR2bits.BCLIF)
	{
		PIR2bits.BCLIF = 0;
		intFunction[INT_BCL]();
	}
	#endif
	#ifdef HLVDINT
	if (PIE2bits.HLVDIE && PIR2bits.HLVDIF)
	{
		PIR2bits.HLVDIF = 0;
		intFunction[INT_HLVD]();
	}
	#endif
	#ifdef SSPINT
	if (PIE1bits.SSPIE && PIR1bits.SSPIF)
	{
		PIR1bits.SSPIF = 0;
		intFunction[INT_SSP]();
	}
	#endif

	//int_start();
}

#endif /* __INTERRUPT_C */
