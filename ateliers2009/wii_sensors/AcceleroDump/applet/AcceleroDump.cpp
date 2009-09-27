#include <Servo.h>
#include <Wire.h>
#include "WiiMPlus.h"

#include "WProgram.h"
void setup();
void loop();
Servo gauche;
Servo droite;

WiiMPlus wmp;

long time;

void setup()
{
  // power WiiMPlus
  DDRC |= _BV(PORTC3) | _BV(2);  // 
  PORTC &=~ _BV(2);           // Analog 2 - 0v
  PORTC |=  _BV(3);           // Analog 3 - 5v

  //
  Serial.begin(115200);
  time = millis();

  //
  Wire.begin();
  wmp.wmpOn(); //turn WM+ on
  wmp.calibrateZeroes(false); //calibrate zeroes
  delay(1000);
}

void loop()
{
  wmp.receiveData();
  Serial.print(millis());
  Serial.print(",");
  Serial.print(wmp.getYaw()); //for info on which axis is which
  Serial.print(",");
  Serial.print(wmp.getPitch());
  Serial.print(",");
  Serial.print(wmp.getRoll());
  Serial.print(",");
  Serial.print(analogRead(0));
  Serial.print(",");
  Serial.print(analogRead(1));
  Serial.print(",");
  Serial.println();
  delay(10);

  // 
  if (Serial.available())
  {
    gauche.attach(9);
    droite.attach(10);

    gauche.write(0);
    droite.write(180);
  }  
}






int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

