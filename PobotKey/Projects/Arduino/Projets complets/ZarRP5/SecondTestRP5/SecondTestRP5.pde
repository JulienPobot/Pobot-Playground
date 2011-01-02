/**
 * Test de pilotage du RP5 à chenilles
 *
 * matériel : une Romeo (driver L298)
 */

#include "ZarRP5.h"

ZarRP5 robot;

void setup()
{
  robot.init();
  setPwmFrequency(1024);
}

void loop()
{
  robot.avance(50);
  delay(120);
  robot.arrete();
  delay(300);     
}



/**
 * Fonction de modification du timer pour les pattes 5 et 6
 */
void setPwmFrequency(int divisor) {
  byte mode;
  switch(divisor) {
  case 1: 
    mode = 0x01; 
    break;
  case 8: 
    mode = 0x02; 
    break;
  case 64: 
    mode = 0x03; 
    break;
  case 256: 
    mode = 0x04; 
    break;
  case 1024: 
    mode = 0x05; 
    break;
  default: 
    return;
  }
  // le code important : changement du prescaler du comparateur B du timer 0
  TCCR0B = TCCR0B & 0b11111000 | mode;    
}




