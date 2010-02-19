#include <Wire.h>
#include <MegaServo.h>

#include "Marionnette.h"
#include "WiiChuck.h"

// la marionnette connect�e � la carte Arduino
Marionnette mario;

// le nunchuck connect� � la carte Arduino
WiiChuck chuck = WiiChuck();

enum State { 
  INIT, CONTROL, IDLE };

State state = INIT;

// initialisation du mat�riel
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
