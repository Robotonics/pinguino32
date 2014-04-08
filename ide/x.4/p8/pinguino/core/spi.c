#ifndef __SPI_C__
#define __SPI_C__

#include <spi.h>
#include <delay.c>

/* Port Controls  (Platform dependent) */
#define SELECT()	SD_CS = 0 // device selection
#define DESELECT()	SD_CS = 1 // it stops device selection
#define LA7 LATBbits.LATB5
#define LA6 LATBbits.LATB4

u8 this_mode = SPI_MODE0;
u8 this_clock = SPI_MASTER_FOSC_64;
u8 this_role = SPI_CLOCK_DIV64;
u8 this_bitorder = SPI_MSBFIRST;
u8 this_phase = SPI_SMPEND;

/**
 * This function initializes the SPI hardware configuring polarity and edge
 * of transition using Standard SPI Mode Terminology (datasheet Table 19-1 p206)
 *
 *    Mode0,Mode1 CKP   CKE
 *    0,0         0     1
 *    0,1         0     0
 *    1,0         1     1
 *    1,1         1     0
 *
 * Also is possible to use LOW or HIGH for mode0 to indicate the idle state
 * clock level, and RISING or FALLING to indicate when the transmit should
 * take place.
 *
 * The sync_mode param could be any of SPI_MASTER, MASTER_FOSC_4, MASTER_FOSC_16,
 * MASTER_FOSC_64, SLAVE_SS or SLAVE, where MASTER_FOSC_X indicates the SPI
 * clock speed used. If you want to use the /SS pin in slave mode you should
 * initialize the SPI using SPI_SLAVE_SS.
 sync_mode   description
   0       SPI_MASTER mode, clock = FOSC_4
   1       SPI_MASTER, clock = FOSC_16
   2       SPI_MASTER, clock = FOSC_64
   3       SPI_MASTER, clock = TMR2 output/2
   4       SPI_SLAVE mode, /SS pin enabled
   5       SPI_SLAVE, /SS pin disabled
 * smp_phase = Sample bit
   0  in slave mode
   0  in master mode : Input data sampled at middle of data output time
   1  in master mode : Input data sampled at end of data output time
   Nota bene : enter 128 as decimal value or 0x80 as hexadecimal value for the below smp_phase parameter
 */

void SPI_init()
{
    DESELECT();
    ENABLE = 0;
    STATUS = 0x3F;                 // power on state (SMP=0) 
    CONFIG = 0x00;                 // power on state
    CONFIG |= this_clock;           // select serial mode

    if (this_phase == SPI_SMPEND)
        SMP = 1;                   // input data sampled at end of data output time

    switch( this_mode )
    {
        case SPI_MODE0:              // SPI bus mode 0,0
          CKP = 0;
          CKE = 1;                   // data transmitted on falling edge
          break;    
        case SPI_MODE1:              // default SPI bus mode 0,1
          CKP = 0;
          CKE = 0;                   // data transmitted on falling edge
          break;
        case SPI_MODE2:              // SPI bus mode 1,0
          CKP = 1;                   // clock idle state high
          CKE = 1;                   // data transmitted on rising edge
          break;
        case SPI_MODE3:              // SPI bus mode 1,1
          CKP = 1;                   // clock idle state high
          CKE = 0;                   // data transmitted on falling edge
          break;
    }

    switch( this_role )
    {
        case SPI_SLAVE_SS:           // slave mode w /SS enable
            SSPIN = 1;               // define /SS pin as input
            SCKPIN = 1;              // define clock pin as input
            break;

        case SPI_SLAVE:              // slave mode w/o /SS enable
            SCKPIN = 1;              // define clock pin as input
            break;

        default:                     // master mode, define clock pin as output
            SCKPIN = 0;              // define clock pin as input
            break;
    }

    SSPIN  = 0;                    // define SS  pin as output
    SDIPIN = 1;                    // define SDI pin as input
    SDOPIN = 0;                    // define SDO pin as output
    ENABLE = 1;
    Delayms(30);

    #ifdef SPIINT
        SSPIE = 1;
        INTCONbits.GIEH = 1;
        INTCONbits.GIEL = 1;
        INTCONbits.PEIE = 1;
        //SSPHPE = 1;
        FLAG = 0;
    #endif
}

/**
 * This function sets the order of the bits shifted out of and into the SPI bus,
 * either LSBFIRST (least-significant bit first) or MSBFIRST (most-significant bit first). 
 */

void SPI_setBitOrder(u8 bitorder)
{
	this_bitorder = bitorder;
}

/**
 *	This function sets the SPI data mode (clock polarity and phase)
 *	mode is SPI_MODE0, SPI_MODE1, SPI_MODE2, or SPI_MODE3
 */
 
void SPI_setDataMode(u8 mode)
{
	this_mode = mode;
}

void SPI_setMode(u8 mode)
{
	this_role  = mode;
	this_clock = mode;
}

/**
 * This function sets the SPI clock divider relative to the system clock.
 * The dividers available are 4, 8, 16, 64.
 * The default setting is SPI_CLOCK_DIV4, which sets the SPI clock to one-quarter
 * the frequency of the system clock. 
 */

void SPI_setClockDivider(u8 clock)
{
	this_clock = clock;
	this_role  = clock;
}

u8 SPI_write(u8 datax) {
    u8 clear;
    clear = BUFFER;        // clear BF
    FLAG = 0;              // enable SPI2 interrupt
    WCOL = 0;
    BUFFER = datax;        // send data

    if (WCOL)
        return (1);
    else
	    while (!FLAG);
    return(0);
}

u8 SPI_read(void) {
    u8 clear;
    clear = BUFFER; //clear BF
    FLAG = 0;
    BUFFER = 0xFF; // Initiate bus cycle
    while (!FLAG);
    return(BUFFER);
}

void SPI_interrupt()
{
u8 c;
   if (! FLAG)
      return;
   c = BUFFER;
   FLAG = 0;
   BUFFER = SPI_onEvent_func(c);
}  // end of interrupt routine SPI_STC_vect

static void SPI_onEvent(u8(*func)(u8))
{
  SPI_onEvent_func = func;
}
#endif
