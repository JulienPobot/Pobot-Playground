#include <Wire.h>
#include <MegaServo.h>

#include "Marionnette.h"
#include "WiiChuck.h"

// la marionnette connect\u00e9e \u00e0 la carte Arduino
#include "WProgram.h"
void setup();
void loop();
Marionnette mario;

// le nunchuck connect\u00e9 \u00e0 la carte Arduino
WiiChuck chuck = WiiChuck();

enum State { 
  INIT, CONTROL, IDLE };

State state = INIT;

// initialisation du mat\u00e9riel
void setup() 
{
  //
  mario.init();
  //
  chuck.begin();
  chuck.update();
  state = IDLE;
  //
  Serial.begin(9600);  

}


void loop()
{
  delay(20);
  chuck.update();
  //
  switch (state)
  {
  case IDLE:
    if (chuck.cPressed())
    {
      state = CONTROL;
    }
    break;
  case CONTROL:
    if (chuck.cPressed())
    {
      state = IDLE;
      return;
    }
    mario.updateChuck(chuck);
    mario.writeFigure();
    break;
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

