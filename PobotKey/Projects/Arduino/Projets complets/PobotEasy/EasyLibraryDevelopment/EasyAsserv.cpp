/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyAsserv.h"

/**
 * Les consignes de vitesse
 */
void EasyAsserv::vitesses(int left, int right)
{
  asservLeft = left; 
  asservRight = right;
}

/**
 * But : modifier les vitesses des moteurs 
 */
void EasyAsserv::checkAsserv()
{
  // mettre à jour les incréments observés
  int diffLeft = odo->totalLeft - lastLeft;
  lastLeft = odo->totalLeft;
  
  Serial.print(diffLeft);
  Serial.print("\t");
  
  
  Serial.print(asservLeft);
  Serial.print("\t");
  
  
  Serial.print(robot->speedLeft);
  Serial.println();
  
  // calculer la nouvelle consigne
  // = vitesse courante - erreur constatée
  robot->speedLeft -= (diffLeft - asservLeft);
  
}

