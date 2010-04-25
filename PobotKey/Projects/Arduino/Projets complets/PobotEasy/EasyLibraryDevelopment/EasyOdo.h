/**
 * This source code is release under the MIT License.
 *
 */

#ifndef _EASY_ODO_H
#define _EASY_ODO_H

// conversion from degrees to radians and conversely
#define RAD_TO_DEG(r)  ((float)r*180.0/PI)
#define DEG_TO_RAD(d)  ((int)d*PI/180.0)

#include "WProgram.h"

#include "EasyRobot.h"

#define pinOdoLeft  2
#define pinOdoRight  3

// Class EasyOdo
class EasyOdo{

public:

  // true if we are tracking changes
  boolean flagZero;

  double x;
  double y;
  double theta;

  int counterLeft;
  int counterRight;

  int totalLeft;
  int totalRight;
  
  double tgmoyenne;
  double tgvar;

  EasyOdo() {
    x = 0;
    y = 0;
    theta = 0;

    //
    pinMode(pinOdoLeft,INPUT);  
    pinMode(pinOdoRight,INPUT);    
  }

  void update(void);

  void attachRobot(EasyRobot *rob) {
    robot = rob;
  }

  void checkZero(void);

  void incrementLeft(void);

  void incrementRight(void);

  void reset(void)
  {
    counterLeft = 0;
    counterRight = 0;
    totalLeft = 0;
    totalRight = 0;
  }
  
  void tgreset(void)
  {
     tgmoyenne = 0; 
     tgvar = 0;
  }

private:

  EasyRobot *robot;

  double dTheta;
  double dDist;

};
#endif





