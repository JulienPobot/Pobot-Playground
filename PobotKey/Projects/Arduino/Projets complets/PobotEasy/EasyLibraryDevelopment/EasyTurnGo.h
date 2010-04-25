/**
 * This source code is release under the MIT License.
 *
 */

#ifndef _EASY_TURNGO_H
#define _EASY_TURNGO_H

#include "WProgram.h"
#include "EasyRobot.h"
#include "EasyOdo.h"


class EasyTurnGo
{
public:

  EasyTurnGo() {
     count = 0; 
  }

  void avance(int value, int vitesse);

  void tourneDroite(int angle, int vitesse);

  void recule(int value, int vitesse);

  void attachRobot(EasyRobot *rob) {
    tgrobot = rob;
  }

  void attachOdo(EasyOdo *odo) {
    tgodo = odo;
  }

private:
  int count;

  EasyRobot *tgrobot;  
  EasyOdo *tgodo;

};
#endif


