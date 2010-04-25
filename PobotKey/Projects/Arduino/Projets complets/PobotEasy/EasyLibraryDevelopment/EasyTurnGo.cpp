
#include "EasyTurnGo.h"


void EasyTurnGo::avance(int value, int vitesse)
{
  tgodo->tgreset();

  tgrobot->penteSpeeds(vitesse,vitesse);

  delay(value*10);

  tgrobot->penteSpeeds(0,0);

  delay(200);

  tgrobot->stopRobot();

  return;

  /*
  while (tgodo->tgmoyenne < value) {  
   // 
   tgrobot->directSpeeds(vitesse+tgodo->tgvar,vitesse-tgodo->tgvar);
   // pour éviter d'avoir une boucle vide    
   count++;
   // affichage de la variable
   int val = 500+tgodo->tgvar;
   Serial.print( 0xff, BYTE);
   Serial.print( (val >> 8) & 0xff, BYTE);
   Serial.print( val & 0xff, BYTE);
   }
   
   tgrobot->stopRobot();
   
   */
}

void EasyTurnGo::tourneDroite(int angle, int vitesse)
{

  tgodo->tgreset();
  
  tgrobot->penteSpeeds(vitesse,-1*vitesse);

  delay(angle*10);

  tgrobot->penteSpeeds(0,0);

  delay(200);

  tgrobot->stopRobot();

  return;
}

void EasyTurnGo::recule(int value, int vitesse)
{
  tgodo->tgreset();

  tgrobot->penteSpeeds(-1*vitesse,-1*vitesse);

  while (tgodo->tgmoyenne > value) {    
    tgrobot->penteSpeeds(-1*vitesse+2*tgodo->tgvar,-1*vitesse-2*tgodo->tgvar);
    count++;
  }


}









