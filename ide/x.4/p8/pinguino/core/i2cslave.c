/*	----------------------------------------------------------------------------
	FILE:			i2cslave.c
	PROJECT:		pinguino
	PURPOSE:		Include functions to handle I2C communication by Slave
	PROGRAMMER:      A. GENTRIC		
	FIRST RELEASE:	abr. 2013
	LAST RELEASE:	
	----------------------------------------------------------------------------

Adapted from Sebastien Lelong, Joep Suijs, Albert Faber's programs

addresses coded with 10-bits are not allowed
very important : interrupts (global, peripherals and i2c) are enable
	---------------------------------------------------------------------------- */
#define I2C_BUFFER_SIZE 32
#define LA7 LATBbits.LATB7
#include <delay.h>
#include <serial.c>


u8  i2c_datapresent;   // datapresent flag


extern void receiveEvent(u8);

void i2c_hw_slave_init(u8 height_bits_icaddress)
{
u8 i;
   // only slave 7bit address
   SSPCON1 = 0b00110110;
   SSPCON1 = SSPCON1 | 0b00001000;   // enable start/stop interrupts
    // I2C slave hardware
	// This address must not be one bit left-shifted when used for R/W operations
    SSPADD = height_bits_icaddress;


    // init SSPSTAT
    SSPSTATbits.BF = 0;
    SSPCON1bits.WCOL = 0;
    SSPCON1bits.SSPOV = 0;
    PIR1bits.SSPIF = 0;
    // enable interrupts
    PIE1bits.SSPIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
	
	_i2c_rxBufferIndex = 0;
	_i2c_txBufferIndex = 0;
	i2c_datapresent=0; //received data
    for (i=0;i< _I2CBUFFERLENGTH_;i++) {_i2c_rxBuffer[i]=0; _i2c_txBuffer[i]=0;}
}


// read a byte from i2c buffer and returns it
u8 i2c_hw_slave_read_i2c()
{    u8 tmpbuf;
    tmpbuf = SSPBUF;
    return tmpbuf;
}


// write a byte to i2c bus
void i2c_hw_slave_write_i2c(u8 what)
    // wait 'til buffer is empty
{  u8 dosend; 
   while ( SSPSTATbits.BF);
    dosend = 1;
    while (dosend) {
        SSPCON1bits.WCOL = 0;
        // try to write into buffer, checking collision
        SSPBUF = what;
        if (! SSPCON1bits.WCOL)
            dosend = 0;
    }
    SSPCON1bits.CKP = 1;
}

void i2c_call_process_message()
{
   // let user process buffer
   if (i2c_datapresent)
   {
      _i2c_rxBufferIndex=0;
      _i2c_onReceive_function(_i2c_rxBufferLength);  
      i2c_datapresent = 0;
      _i2c_rxBufferLength = 0;
      _i2c_rxBufferIndex=0;
  }

}   

// this callback is used when something wrong happened 
// during communication between master and us
void i2c_hw_slave_on_error()
{
while(1) {LA7=!LA7; delay10ktcy(500);}
   // Just tell user user something's got wrong
}
void i2c_hw_slave_on_start()
{
}

void i2c_hw_slave_on_stop()
{   // let user process buffer
   i2c_call_process_message();
}                                
// this callback is used when master wants to talk to us
// and our i2c address has been recognized
void i2c_hw_slave_on_state_1(u8 _trash)
{
   // let user process buffer (there should not be one if the proper procedure is executed)
   i2c_call_process_message();
}

// This callback is used when master sends a data byte
void i2c_hw_slave_on_state_2(u8 rcv)
{
   i2c_datapresent   = 1;      // Indicate we received data
//serial_printf("i%2X ",i2c_rxindex);
   // store data
   _i2c_rxBuffer[_i2c_rxBufferIndex] = rcv; 

   if (_i2c_rxBufferIndex < I2C_BUFFER_SIZE)
      _i2c_rxBufferIndex++;   // point to next position
   _i2c_rxBufferLength = _i2c_rxBufferIndex;
//serial_printf(":l%2X ",i2c_rxlength);

}

// this callback is used when master wants to read something
// from us. It should use i2c_hw_slave_write() to send something
void i2c_hw_slave_on_state_3()
{   // let user process buffer
   i2c_call_process_message();
 
   // send first byte to master
   i2c_hw_slave_write_i2c(_i2c_txBuffer[0]);  // send first databyte
   _i2c_txBufferIndex = 1;                          // point to next position
}


// this callback is used when master, after having read something,
// still wants to read and get data from us.
void i2c_hw_slave_on_state_4()
{
   // This shouldn't occur in our i2c echo example

   i2c_hw_slave_write_i2c(_i2c_txBuffer[_i2c_txBufferIndex]);    // send data 

   if (_i2c_txBufferIndex < _I2CBUFFERLENGTH_)
      _i2c_txBufferIndex++;   // point to next position

//   i2c_hw_slave_on_error();
}

// this callback is used when master does not want to talk
// with us anymore... This is an appropriate place to reset
// data for instance
void i2c_hw_slave_on_state_5()
{   // data = 0 
}

#ifdef I2CINT
void I2C_interrupt()
{
u8 tmpstat;
u8 _trash;
u8 rcv;
LA7=1;
   if (! PIR1bits.SSPIF)
      return;

   PIR1bits.SSPIF = 0;
   tmpstat = SSPSTAT;

   // check for overflow
   if (SSPCON1bits.SSPOV)
   {
      _trash = i2c_hw_slave_read_i2c();
      _trash = i2c_hw_slave_read_i2c();
      SSPCON1bits.SSPOV = 0;
   }

   // start & stop handlers are necessary

         if ((tmpstat & 0b00001111) == 0b00001000)
         {
            i2c_hw_slave_on_start();
            return;
         }
         if ((tmpstat & 0b00110000) == 0b00110000)
		 {
            i2c_hw_slave_on_stop();
            return;
         }

   // mask out unimportant bit
   tmpstat = tmpstat & 0b00101101;

   // State machine for PIC18f

      // slightly modified statemachine for PIC18F devices
      // see AN734 revision B for additional details

      // state 1: write operation, last byte is address, buffer full
      // byte is an address, it we get here, we just know master
      // wants to talk to us, in a new write operation...
      // and we also know address is recognized (BF is set, see spec)
      // anyway, we must read buffer to reset BF bit
      // (buffer actually contains previously detected address, that's
      // why it's usually ignored)
      //
      //     => call i2c_hw_slave_on_state_1(u8)
      if (!(tmpstat ^ 0b00001001))
      {
          _trash = i2c_hw_slave_read_i2c();
          i2c_hw_slave_on_state_1(_trash);
      }
      // state 2: write operation, last byte is data, buffer full
      // master wants to write, it sends a byte
      //
      //     => call i2c_hw_slave_on_state_2(u8)
      //
      // (note: at a write-only sequence, fast masters give a stop (clearing bit3)
      //  before we can read the register. The or statment is handles this)
      else if (!((tmpstat | 0b00001000) ^ 0b00101001))
      {
        rcv = i2c_hw_slave_read_i2c();
//serial_printf("\r\nI/R%2X", rcv);
        i2c_hw_slave_on_state_2(rcv);
//        _i2c_onReceive_function();
      }
      // state 3: read operation, last byte is address, buffer empty
      // master wants to get a value from us
      //
      //     => call i2c_hw_slave_on_state_3()
      // Mask BF flag for newer statemachines
      else if (!((tmpstat & 0b11111110) ^ 0b00001100))
      {
         // clear BF flag by clearing SSPBUF, required for PIC18f devices
         // with a new version of the i2c statemachine (see also AN734)
         _trash = i2c_hw_slave_read_i2c();
         i2c_hw_slave_on_state_3();
//         _i2c_onRequest_function();

      }
      else if (!((tmpstat & 0b11111011) ^ 0b00101000))
      {

         // check CKP bit to distinguish between state 4 and 5
         if ( SSPCON1bits.CKP == 0 )

            // state 4: read operation, last byte is data, buffer empty
            // master still wants to get a value from us
            //
            //     => call i2c_hw_slave_on_state_4()
            // Don't check R/W flag, use CKP bit instead
           i2c_hw_slave_on_state_4();

         else

            // state 5: nack
            // master doesn't want to talk with us anymore
            //
            //     => call i2c_hw_slave_on_state_5()
            // AN734 does not talk about setting CKP, whereas spec says
            // it must be set. Some people say it can be error prone.
            // SSPCON1bits.CKP = 1
            i2c_hw_slave_on_state_5();
         
      }
      // state not recognized... Error !
      else
        // something went wrong, that is, XOR operations did not match
        // SSPSTAT bits
        i2c_hw_slave_on_error();

      // always clear CKP so clock line will be released (HW will set CKP
      // bit when clock stretching feature is enabled
      SSPCON1bits.CKP = 1;
}
#endif