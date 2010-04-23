/**
 * This source code is release under the MIT License.
 *
 */

#ifndef _EASY_ASSERV_H
#define _EASY_ASSERV_H

#include "WProgram.h"

#include "EasyOdo.h"
#include "EasyRobot.h"

class EasyAsserv
{
public:  

  void vitesses(int, int);
  
  void checkAsserv(void);
  
  void attachRobot(EasyRobot *rob) {
    robot = rob;
  }
  
  void attachOdo(EasyOdo *od) {
    odo = od;
  }
  
private:
  int asservLeft;
  int asservRight;
  
  int lastLeft;
  int lastRight;

  EasyRobot *robot;
  EasyOdo *odo;

};
#endif



