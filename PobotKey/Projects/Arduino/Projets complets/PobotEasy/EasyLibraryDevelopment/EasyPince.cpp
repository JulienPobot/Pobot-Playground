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




