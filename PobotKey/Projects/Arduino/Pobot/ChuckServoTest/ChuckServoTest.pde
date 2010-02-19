#include "Servo.h"
#include "SoftwareServo.h"
#include "Wire.h"
#include "WiiChuck.h"

Servo servGauche;
Servo servDroite;

SoftwareServo servBras;
SoftwareServo servPince;

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
  Serial.begin(19200);
  chuck.begin();
  chuck.update();
  //
  servBras.attach(11);
  servPince.attach(12);
  servBras.write(90);
  servPince.write(90);
  //

}

void loop() {
  chuck.update(); 

  // lecture des valeurs du joystick 2D
  valX = chuck.readJoyX();
  valY = chuck.readJoyY();
  
  servBras.write(90+valX/2);
  servPince.write(90+valY/2);

  // affichage
  Serial.print(valX);
  Serial.print(" ");
  Serial.print(valY);
  Serial.println();
  
  //  
  delay(50);
  SoftwareServo::refresh();
}

