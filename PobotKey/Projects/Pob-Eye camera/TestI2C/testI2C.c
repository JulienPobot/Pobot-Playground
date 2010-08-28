#include <pob-eye.h>

int main(void)
{
  UInt8 *rgbFrame;
  UInt32 i;
  UInt32 red = 0, green = 0, blue = 0;


  UInt8 blinkm_addr = 0x12; // The slave address (sur 8 bits)
  UInt8 buffer;
  UInt8 data[4];

  InitPobeye2();
  InitI2C(I2C_100_KHZ);
  InitCameraPobeye2();
  InitUART0(115200);

  SendString("Test I2C !");

  // arrêt de la playlist interne
  SendSlaveAddress( blinkm_addr );         
  buffer = 'o';
  WriteOnI2C(buffer);
  StopI2C();

  rgbFrame = GetRGBFrame();
  while(1) {
    GrabRGBFrame();

    for (i=0; i<  100; i++) // 100 valeurs
    {
       red   += GetRed(rgbFrame[i*100]); // tous les 100 pixels
       green += GetGreen(rgbFrame[i*100]);
       blue  += GetBlue(rgbFrame[i*100]);  
    }
    
    red = red / 100;
    green = green / 100;
    blue = blue / 100;

    PrintToUart0("%d %d %d\n",red,green,blue);

    data[0] = 'n';
    data[1] = red;
    data[2] = green;
    data[3] = blue;

    SendSlaveAddress( blinkm_addr );    
    WriteBufferOnI2C(data,4);
    StopI2C();

    WaitUs(500);
  }    
  return 0;
}
