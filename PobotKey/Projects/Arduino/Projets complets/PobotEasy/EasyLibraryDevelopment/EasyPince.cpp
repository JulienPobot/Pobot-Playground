/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyPince.h"


void EasyPince::ouvrePince(void)
{
  enablePince();
  servoPince->write(PINCE_OUVERTE);
}

void EasyPince::fermePince(void)
{
  enablePince();
  servoPince->write(PINCE_FERMEE);
}

boolean EasyPince::isPinceOuverte(void)
{
  return false;
}

boolean EasyPince::isPinceFermee(void)
{
  return false;
}

boolean EasyPince::isPinceBloquee(void)
{
  delay(50);
  return !isPinceFermee();
}


void EasyPince::levePince(void)
{
  enablePince();
  servoBras->write(BRAS_HAUT);
}

void EasyPince::baissePince(void)
{
  enablePince();
  servoBras->write(BRAS_BAS);
}

void EasyPince::anglePince(int val)
{
  enablePince();
  servoBras->write(val);
}

void EasyPince::testPince(void)
{ 
  //
  enablePince();
  // fermer la pince  
  fermePince();
  Serial.println("On ferme la pince.");
  delay(500);

  // lever la pince
  levePince();
  Serial.println("On leve la pince.");
  delay(500);

  // ouvrir la pince
  ouvrePince();
  Serial.println("On ouvre la pince.");
  delay(500);

  // baisser la pince
  baissePince();
  Serial.println("On ferme la pince.");
  delay(500);

  //
  disablePince();
}




