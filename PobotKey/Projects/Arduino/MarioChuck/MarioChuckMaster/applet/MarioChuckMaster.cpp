/**
 * Partie maitre de la marionnette t\u00e9l\u00e9command\u00e9e
 *
 * @copyright Julien Holtzer (2009)
 * @mail julien.holtzer@gmail.com
 *
 */

#include "Wire.h"
#include "WiiChuck.h"

#define SEUIL_SPEED1  2000

#include "WProgram.h"
void setup();
void loop();
WiiChuck chuck = WiiChuck();  

boolean open = false;
int lastRoll = 0;
int lastPitch = 0;

void setup()
{ 
  // alimentation du nunchuck (pas n\u00e9cessaire de comprendre)  
  DDRC |= _BV(PC3) | _BV(PC2);  // pattes Analog 2 et 3 en sortie  
  PORTC &=~ _BV(PC2);           // patte Analog 2 \u00e0 0v  
  PORTC |=  _BV(PC3);           // patte Analog 3 \u00e0 5v  
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

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

