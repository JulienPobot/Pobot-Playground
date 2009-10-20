/*
 * Change the color according to a received string on the serial link
 *
 * Speed : 9600
 *
 * r : blink to red
 * o : blink to orange
 * g : blink to green
 * b : blink to blue
 */

#include <Wire.h>

void setup()
{    
  // alimentation sur la carte
  DDRC |= _BV(PC3) | _BV(PC2);
  PORTC &=~ _BV(PC2);
  PORTC |=  _BV(PC3);
  // wait for things to stabilize
  delay(100);  
  Wire.begin(); 
  //
  Serial.begin(9600);   
  //
  stopScript();
  //
  setColor(0,0,0);
}

void loop()
{
  while (Serial.available())
  {
    switch (Serial.read())
    {
    case 'r': 
      setColor(255,0,0);      
      break;      
    case 'b': 
      setColor(0,255,0);      
      break;      
    case 'o': 
      setColor(255,0,255);      
      break;      
    case 'g':
      setColor(0,0,255);
      break;
    default:
      setColor(255,255,255);
      break;
    }
  }
}   

void stopScript()
{
  Wire.beginTransmission(0x00);
  Wire.send('o');
  Wire.endTransmission();
}

void setColor(int R, int G, int B)
{ 
  Wire.beginTransmission(0x00);
  // join I2C, talk to BlinkM 0x09 
  Wire.send('c'); // fade command to color ...
  Wire.send(R); // red
  Wire.send(B); // blue
  Wire.send(G); // green
  Wire.endTransmission();      
  // leave I2C bus 
}


