#class auto

#use "I2C_DEVICES.LIB"

#define I2CSCLBit 3
#define I2CSDABit	2

#define ADDR	0xFF

char msg[4];

void main()
{
	int i;
	i2c_init();

   msg[0] = 'h';
   msg[1] = 128;
   msg[2] = 255;
   msg[3] = 255;

   for(i = 0;i < 30;i++)
   {
      msg[1] = i*10;
      I2CWrite(ADDR,0,msg,4);
      costate {
         printf("Test %d\n",i);
 	      waitfor(DelayMs(1000));
      }
   }
}