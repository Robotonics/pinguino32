// Serial.printf and sprintf Demo
// Régis Blanchot 2010

char *string = "Hello world!";
char c = 65;
int i = 333;
long l = 115200;
float f = 3.14159265; // floating point support is not yet implemented in printf
void setup()
{
	Serial.begin(9600);
}
void loop()
{
	Serial.printf("\r\n");
	Serial.printf("**************************\r\n");
	Serial.printf("*** Serial Printf Demo ***\r\n");
	Serial.printf("**************************\r\n");
	Serial.printf("\r\n");
	Serial.printf("string = %s\r\n", string);
	Serial.printf("character = \"%c\"\r\n", c);
	Serial.printf("signed char = %d / unsigned char = %u\r\n", -c, -c);
	Serial.printf("signed int = %d / unsigned int = %u\r\n", -i, -i);
	Serial.printf("signed long = %d / unsigned long = %u\r\n", -l, -l);
	Serial.printf("decimal[%d] = hexa[0x%X] = binary[0b%016b] = octal[%o]\r\n", i, i, i, i);
	Serial.printf("float = %f\r\n", f);
	Serial.printf("justif: \"%-10s\"\r\n", "left");
	Serial.printf("justif: \"%10s\"\r\n", "right");
	Serial.printf(" 3: %04d zero padded\r\n", 3);
	Serial.printf(" 3: %-4d left justif.\r\n", 3);
	Serial.printf(" 3: %4d right justif.\r\n", 3);
	Serial.printf("-3: %04d zero padded\r\n", -3);
	Serial.printf("-3: %-4d left justif.\r\n", -3);
	Serial.printf("-3: %4d right justif.\r\n", -3);
	Serial.printf("\r\n");
	Serial.printf("Press Any Key to continue ...\r\n");
	Serial.getkey();
}
