#include "Wire.h"
#include "WiiChuck.h"

#include "WProgram.h"
void setup();
void loop();
WiiChuck chuck = WiiChuck();

int valX = 0;
int valY = 0;

void setup() {
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

void loop() {
  chuck.update(); 
  
  int val = chuck.readRoll();
  
  Serial.print(val);
  Serial.print(" ");
  
  val = chuck.readAccelX();
  
  Serial.print(val);
  Serial.print(" ");
  Serial.println();
 

  /*
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);
  */
  //  
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

