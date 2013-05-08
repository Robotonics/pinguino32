/*	--------------------------------------------------------------------
	FILE:			digitalw.c
	PROJECT:		pinguino
	PURPOSE:		Digital IO management
	PROGRAMER:		Jean-Pierre MANDON
	FIRST RELEASE:	2008
	LAST RELEASE:	29 Mar 2013
	----------------------------------------------------------------------------
	TODO : 
	----------------------------------------------------------------------------
    CHANGELOG :
        jean-pierre mandon : modification 2009/08/08 18F4550
        regis blanchot 2011/08/09 : FreeJALduino support
        regis blanchot 2012/02/14 : Pinguino 26J50 support
        regis blanchot 2012/09/28 : complete rewrite
        regis blanchot 2012/11/19 : Pinguino 1220 and 1320 support
        regis blanchot 2012/12/07 : Pinguino 25k50 and 45k50 support
        regis blanchot 2013/01/05 : fixed warnings about pointers in RAM
        andre gentric  2013/03/29 : fixed Pinguino4550 RA4 pin definition
	----------------------------------------------------------------------------
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
	------------------------------------------------------------------*/

#ifndef __DIGITALW__
#define __DIGITALW__

//#include <pic18fregs.h>
//#include <typedef.h>

/*
#define pB	0
#define pC	1
#define pA	2
#define pD	3
#define pE	4
#define pF	5
#define pG	6
*/

#define pA	0
#define pB	1
#define pC	2
#define pD	3
#define pE	4
#define pF	5
#define pG	6

#define _0	0x01    // 1<<0
#define _1	0x02    // 1<<1 
#define _2	0x04    // 1<<2
#define _3	0x08    // 1<<3 
#define _4	0x10    // 1<<4 
#define _5	0x20    // 1<<5 
#define _6	0x40    // 1<<6 
#define _7	0x80    // 1<<7 

/**********************************************************************/
#if defined(PINGUINO1220) || defined(PINGUINO1320)
/**********************************************************************/
                                                    // Pinguino pin number
const unsigned char mask[19]={
                    _0,_1,_2,_3,_4,_5,_6,_7,        // 0 - 7
                    _0,_1,_2,_3,_4,_5};             // 8 - 13

const unsigned char port[19]={
                    pB, pB, pB, pB, pB, pB, pB, pB, // 0 - 7
                    pA, pA, pA, pA, pA, pA};        // 8 - 13

/**********************************************************************/
#elif defined(__18f14k22)
/**********************************************************************/
                                                    // Pinguino pin number
const unsigned char mask[19]={
                    _5,_4,_3,_5,_4,_3,_6,_7,_7,     // 0 - 8
                    _0,_1,_2,_0,_1,_2,_4,_5,_6};    // 9 - 17

const unsigned char port[19]={
                    pA, pA, pA, pC, pC, pC, pC, pC, pB, // 0 - 8
                    pA, pA, pA, pC, pC, pC, pB, pB, pB }; // 9 - 17

/**********************************************************************/
#elif defined(PINGUINO2550) || defined(PINGUINO25K50) || defined(CHRP3)
/**********************************************************************/
                                                    // Pinguino pin number
const unsigned char mask[19]={
                    _0,_1,_2,_3,_4,_5,_6,_7,        // 0 - 7
                    _6,_7,_0,_1,_2,                 // 8 - 12
                    _0,_1,_2,_3,_5,_4};             // 13 - 18

const unsigned char port[19]={
                    pB, pB, pB, pB, pB, pB, pB, pB, // 0 - 7
                    pC, pC, pC, pC, pC,             // 8 - 12
                    pA, pA, pA, pA, pA, pA};        // 13 - 18

/**********************************************************************/
#elif defined(PINGUINO26J50)
/**********************************************************************/
                                                    // Pinguino pin number
const unsigned char mask[18]={
                    _0,_1,_2,_3,_4,_5,_6,_7,        // 0 - 7
                    _6,_7,_0,_1,_2,                 // 8 - 12
                    _0,_1,_2,_3,_5};                // 13 -17

const unsigned char port[18]={
                    pB, pB, pB, pB, pB, pB, pB, pB, // 0 -7
                    pC, pC, pC, pC, pC,             // 8 - 12
                    pA, pA, pA, pA, pA};            // 13 - 17

/**********************************************************************/
#elif defined(PINGUINO4550) || defined(PINGUINO45K50)
/**********************************************************************/
                                                    // Pinguino pin number
const unsigned char mask[30]={
                    _0,_1,_2,_3,_4,_5,_6,_7,		// 0 - 7
                    _6,_7,_0,_1,_2,					// 8 - 12
                    _0,_1,_2,_3,_5,				    // 13 - 17
                    _0,_1,_2,						// 18 - 20
                    _0,_1,_2,_3,_4,_5,_6,_7,  		// 21 - 28
                    _4                              // 29
                    };
const unsigned char port[30]={
                    pB, pB, pB, pB, pB, pB, pB, pB,
                    pC, pC, pC, pC, pC,
                    pA, pA, pA, pA, pA,
                    pE, pE, pE,
                    pD, pD, pD, pD, pD, pD, pD, pD,
                    pA};

/**********************************************************************/
#elif defined(FREEJALDUINO)
/**********************************************************************/

const unsigned char mask[19]={
                    _7,_6,_4,_0,_1,_2,_3,_4,_5,_6,_7,_0,_1,_2,_0,_1,_2,_3, _5};

const unsigned char port[19]={
                    1,1,2,0,0,0,0,0,0,0,0,1,1,1,2,2,2,2,2};

/**********************************************************************/
#elif defined(PICUNO_EQUO)
/**********************************************************************/

const unsigned char mask[14]={
                    _7,_6,_2,_3,_0,_2,_1,_1,_2,_3,_4,_5,_6,_7};
                    
const unsigned char port[14]={
                    1,1,0,0,3,1,1,3,3,3,3,3,3,3};

#else
    #error "Processor Not Yet Supported. Please, Take Contact with Developpers."
#endif

void digitalwrite(unsigned char pin, unsigned char state)
{
    switch (port[pin])
    {
        case pA: if (state) LATA=LATA | mask[pin];
                else LATA=LATA & (255-mask[pin]);
                break;
        case pB: if (state) LATB=LATB | mask[pin]; 
                else LATB=LATB & (255-mask[pin]);
                break;
        case pC: if (state) LATC=LATC | mask[pin];
                else LATC=LATC & (255-mask[pin]);
                break;
        #if defined(PINGUINO4550) || defined(PICUNO_EQUO) 
        case pD: if (state) LATD=LATD | mask[pin]; 
                else LATD=LATD & (255-mask[pin]);
                break;
        case pE: if (state) LATE=LATE | mask[pin]; 
                else LATE=LATE & (255-mask[pin]);
                break;
        #endif
    }
/*
    unsigned char  b = mask[pin];          // 1<<bit

    // A variable should be declared volatile
    // whenever its value could change unexpectedly.
    // For ex. with Memory-mapped peripheral registers.
    
    __data unsigned char volatile * pLAT; // pointer in RAM space
    
    pLAT = (__data unsigned char volatile *) (port[pin] + 0x0F89); // PORTx
    
    if (state)
        *pLAT |= b;            // set bit
    else
        *pLAT &= (255-b);      // clear bit
*/
}

unsigned char digitalread(unsigned char pin)
{
    switch (port[pin])
    {
        case pA: if ((PORTA & mask[pin])!=0) return (1);
            else return (0);
            break;
        case pB: if ((PORTB & mask[pin])!=0) return (1);
            else return (0);
            break;
        case pC: if ((PORTC & mask[pin])!=0) return (1);
            else return (0);
            break;
        #if defined(PINGUINO4550) || defined(PICUNO_EQUO) 
        case pD: if ((PORTD & mask[pin])!=0) return (1);
            else return (0);
            break;
        case pE: if ((PORTE & mask[pin])!=0) return (1);
            else return (0);
            break;
        #endif
    }
    return (0);
/*
    unsigned char  b = mask[pin];          // 1<<bit

    // A variable should be declared volatile
    // whenever its value could change unexpectedly.
    // For ex. with Memory-mapped peripheral registers.
    
    __data unsigned char volatile * pLAT; // pointer in RAM space
    
    pLAT = (__data unsigned char volatile *) (port[pin] + 0x0F89); // PORTx
    
    if ( (*pLAT & b) != 0)
        return 1;               // bit is set
    else
        return 0;               // bit is not set
*/
}

void pinmode(unsigned char pin, unsigned char state)
{
    switch (port[pin])
    {
        case pA: if (state) TRISA=TRISA | mask[pin];
            else TRISA=TRISA & (255-mask[pin]);
            break;
        case pB: if (state) TRISB=TRISB | mask[pin];
            else TRISB=TRISB & (255-mask[pin]);
            break;
        case pC: if (state) TRISC=TRISC | mask[pin];
            else TRISC=TRISC & (255-mask[pin]);
            break;
        #if defined(PINGUINO4550) || defined(PICUNO_EQUO) 
        case pD: if (state) TRISD=TRISD | mask[pin];
            else TRISD=TRISD & (255-mask[pin]);
            break;
        case pE: if (state) TRISE=TRISE | mask[pin];
            else TRISE=TRISE & (255-mask[pin]);
            break;
        #endif
    }
/*
    unsigned char  b = mask[pin];          // 1<<bit

    // A variable should be declared volatile
    // whenever its value could change unexpectedly.
    // For ex. with Memory-mapped peripheral registers.
    
    __data unsigned char volatile * pLAT; // pointer in RAM space

    pLAT = (__data unsigned char volatile *) (port[pin] + 0x0F92); // PORTx
    
    if ( state )                // 1 ?
        *pLAT |= b;            // set bit
    else                        // 0 ?
        *pLAT &= (255-b);      // clear bit
*/
}

//  pLAT = address of PORTx
// *pLAT = content of PORTx
void toggle(unsigned char pin)
{
    unsigned char state;
    state = digitalread(pin);
    digitalwrite(pin, state^1);
/*
    if (state)
        digitalwrite(pin, 0);
    else
        digitalwrite(pin, 1);
*/
    /*
    unsigned char  b = mask[pin];          // 1<<bit

    // A variable should be declared volatile
    // whenever its value could change unexpectedly.
    // For ex. with Memory-mapped peripheral registers.
    
    __data unsigned char volatile * pLAT; // pointer in RAM space

    pLAT = (__data unsigned char volatile *) (port[pin] + 0x0F89); // PORTx
    
    if ( *pLAT & b )           // bit is set ?
        *pLAT &= (255-b);      // clear bit
    else                        // bit is not set ?
        *pLAT |= b;            // set bit
    */
}

#endif /* __DIGITALW__ */
