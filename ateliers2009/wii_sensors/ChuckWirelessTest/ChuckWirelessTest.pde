#include "Wire.h"
#include "WiiChuck.h"

WiiChuck chuck = WiiChuck();

int valX = 0;
int valY = 0;

void setup() {
  // alimentation du nunchuck (pas nécessaire de comprendre)
  DDRC |= _BV(PC3) | _BV(PC2);  // pattes Analog 2 et 3 en sortie
  PORTC &=~ _BV(PC2);           // patte Analog 2 à 0v
  PORTC |=  _BV(PC3);           // patte Analog 3 à 5v
  delay(100);  // une petite attente
  //
  Serial.begin(38400);
  chuck.begin();
  chuck.update();
}

void loop() {
  chuck.update(); 

  // lecture des valeurs du joystick 2D
  valX = chuck.readJoyX();
  valY = chuck.readJoyY();

  // affichage
  
  Serial.println();
  
  //  
}

