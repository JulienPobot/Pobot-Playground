/**
 * This source code is release under the MIT License.
 *
 */
 
#ifndef _EASY_TURNGO_H
#define _EASY_TURNGO_H

#include "WProgram.h"
#include "EasyRobot.h"


class EasyTurnGo
{
public:
  
  void attachRobot(EasyRobot *rob) {
    robot = rob;
  }
    
private:
  EasyRobot *tgrobot;  

};
#endif

