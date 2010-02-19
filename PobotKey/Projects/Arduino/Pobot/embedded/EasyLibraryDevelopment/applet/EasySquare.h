/**
 * This source code is release under the MIT License.
 *
 */
 
#ifndef _EASY_SQUARE_H
#define _EASY_SQUARE_H

/**
 * Robot move control for a square test
 */

// dimension of the square in mm
#define SQUARE_SIZE  400

#define MAX_SPEED  50
#define MIN_SPEED  10

#include "WProgram.h"
#include "EasyRobot.h"
#include "EasyOdo.h"

class EasySquare
{

public:
  // what side of the square is currently done
  int side;

  EasySquare() {
    side = 0;
  }

  void attachRobot(EasyRobot *rob) {
    robot = rob;
  }

  void attachOdo(EasyOdo *od) {
    odo = od;
  }

  void start(void);

  void update(void);

private:


  //
  EasyRobot *robot;
  EasyOdo *odo;

};

#endif
