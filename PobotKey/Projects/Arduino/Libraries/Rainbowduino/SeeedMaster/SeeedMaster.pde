#include <Wire.h>

unsigned char RainbowCMD[5];
unsigned char State = 0;  
unsigned long timeout;


void setup()
{
  Wire.begin(); // join i2c bus (address optional for master) 
}

void loop()
{ int test;
  for(test='0';test<'9';test++)
  {
    ShowChar(4,test,15,0,0,0);
    delay(200);
  }
  
    for(test='A';test<'Z';test++)
  {
    ShowChar(4,test,0,15,0,0);
    delay(200);
  }
  
  for(test='a';test<'z';test++)
  {
    ShowChar(4,test,0,0,15,0);
    delay(200);
  }

}





//--------------------------------------------------------------------------
//Name:ShowColor
//function: Send a conmand to Rainbowduino for showing a color
//parameter: Address: rainbowduino IIC address
//                 red,green,blue:  the color RGB    
//----------------------------------------------------------------------------
void ShowColor(int Address,unsigned char red , unsigned char green, unsigned char blue)
{
  RainbowCMD[0]='R';
  RainbowCMD[1]=0x03;
  RainbowCMD[2]=red;
  RainbowCMD[3]=((green<<4)|(blue));

   SentCMD(Address);
}


//--------------------------------------------------------------------------
//Name:ShowImage
//function: Send a conmand to Rainbowduino for showing a picture which was pre-set in Rainbowduino Flash
//parameter: Address: rainbowduino IIC address
//                 number:  the pre-set picture position
//                 shift: the picture  shift bit for display
//----------------------------------------------------------------------------
void ShowImage(int Address, unsigned char number,unsigned char shift)
{
    RainbowCMD[0]='R';
    RainbowCMD[1]=0x01;
    RainbowCMD[2]=(shift<<4);
    RainbowCMD[4]=number;
 
     SentCMD(Address);
}



//--------------------------------------------------------------------------
//Name:ShowColor
//function: Send a conmand to Rainbowduino for showing a color
//parameter: Address: rainbowduino IIC address
//                 red,green,blue:  the color RGB  
//                 shift: the picture  shift bit for display
//                 ASCII:the char or Number want to show
//----------------------------------------------------------------------------
void ShowChar(int  Address,unsigned char ASCII,unsigned char red, unsigned char blue ,unsigned char green,unsigned char shift)
{
    RainbowCMD[0]='R';
    RainbowCMD[1]=0x02;
    RainbowCMD[2]=((shift<<4)|(red));
    RainbowCMD[3]=((green<<4)|(blue));
    RainbowCMD[4]=ASCII;
	 
     SentCMD(Address);
}




//--------------------------------------------------------------------------
//Name:SentCMD
//function: Send a 5 byet Rainbow conmand out 
//parameter: NC  
//----------------------------------------------------------------------------
void SentCMD(int  Add)
{   unsigned char OK=0;
     unsigned char i,temp;
 
    while(!OK)
  {                          
    switch (State)
    { 	

    case 0:                          
      Wire.beginTransmission(Add);
      for (i=0;i<5;i++) Wire.send(RainbowCMD[i]);
      Wire.endTransmission();    
      delay(5);   
      State=1;                      
      break;

    case 1:

      Wire.requestFrom(Add,1);   
      if (Wire.available()>0) 
        temp=Wire.receive();    
      else {
        temp =0xFF;
        timeout++;
      }

      if ((temp==1)||(temp==2)) State=2;
      else if (temp==0) State=0;

      if (timeout>5000)
      {
        timeout=0;
        State=0;
      }

      delay(5);
      break;

    case 2:
      OK=1;
      State=0;
      break;

    default:
      State=0;
      break;

    }
  }
}
