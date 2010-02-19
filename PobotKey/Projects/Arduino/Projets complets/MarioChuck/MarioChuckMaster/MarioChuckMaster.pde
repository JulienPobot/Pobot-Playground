/**
 * Partie maitre de la marionnette télécommandée
 *
 * @copyright Julien Holtzer (2009)
 * @mail julien.holtzer@gmail.com
 *
 */

#include "Wire.h"
#include "WiiChuck.h"

#define SEUIL_SPEED1  2000

WiiChuck chuck = WiiChuck();  

boolean open = false;
int lastRoll = 0;
int lastPitch = 0;

void setup()
{ 
  // alimentation du nunchuck (pas nécessaire de comprendre)  
  DDRC |= _BV(PC3) | _BV(PC2);  // pattes Analog 2 et 3 en sortie  
  PORTC &=~ _BV(PC2);           // patte Analog 2 à 0v  
  PORTC |=  _BV(PC3);           // patte Analog 3 à 5v  
  delay(100);  // une petite attente  
  //  
  Serial.begin(9600);  
  chuck.begin();  
  chuck.update();  
}

void loop()
{
  //
  chuck.update();  
  
  // bouton z
  if (chuck.zPressed())
  {
    Serial.print('z');    
  }
  
  // bouton c  
  if (chuck.cPressed())
  {
    Serial.print('c');
  }
  
  int val = abs(chuck.readJoyX()) + abs(chuck.readJoyY());
  
  if (open && val < 50)
  {
    open = false;
    Serial.print('b');
  }
  
  if (!open && val > 50)
  {
    open = true;
    Serial.print('d');   
  }    
  
  int roll = (90+chuck.readRoll())/10;
  if (lastRoll != roll)
  {
    lastRoll = roll;
    Serial.print('r');
    Serial.print(roll,BYTE);    
  }
  
  int pitch = (chuck.readPitch())/10;
  if (lastPitch != pitch)
  {
    lastPitch = pitch;
    Serial.print('p');
    Serial.print(pitch,BYTE);    
  }

 
  delay(20);
      
}
