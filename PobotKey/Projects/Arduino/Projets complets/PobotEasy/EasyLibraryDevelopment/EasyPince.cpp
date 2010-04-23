/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyPince.h"


void EasyPince::ouvrePince(void)
{
  servoPince->write(PINCE_OUVERTE);
}

void EasyPince::fermePince(void)
{
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
  servoBras->write(BRAS_HAUT);
}

void EasyPince::baissePince(void)
{
  servoBras->write(BRAS_BAS);
}

void EasyPince::anglePince(int val)
{
  servoBras->write(val);
}

void EasyPince::testPince(void)
{ 
  // fermer la pince  
  fermePince();
  Serial.println("On ferme la pince.");
  delay(2000);

  // lever la pince
  levePince();
  Serial.println("On leve la pince.");
  delay(2000);

  // ouvrir la pince
  ouvrePince();
  Serial.println("On ouvre la pince.");
  delay(2000);

  // baisser la pince
  baissePince();
  Serial.println("On ferme la pince.");
  delay(2000);
}



