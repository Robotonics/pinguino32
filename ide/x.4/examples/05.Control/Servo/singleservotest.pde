// Servo library test with Pinguino
// Connect servo control lines to pinguino pins.
// Feed servos with +5 V:
//
//   +-----+
//   |servo|--------- PWM Servo control ---> to pinguino pin.
//   |     |--------- +5V
//   |  o  |--------- GND
//   |     | 
//   +-----+

#define MYSERVO 0   // servo attached to pin 0

u8 position=0;
s8 dir=1;

void setup(void)
{
    pinMode(USERLED, OUTPUT);    
    servo.attach(MYSERVO);
    servo.setMinimumPulse(MYSERVO, 1000);
    servo.setMaximumPulse(MYSERVO, 2000);
}  

void loop(void)
{
    servo.write(MYSERVO, position);
    position = position + dir;
    if (position>=180) dir = -1;
    if (position<1)   dir = 1;
    toggle(USERLED);
    delay(50);
}
