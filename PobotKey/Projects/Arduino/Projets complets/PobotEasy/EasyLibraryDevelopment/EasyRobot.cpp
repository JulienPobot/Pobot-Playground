/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyRobot.h"


void EasyRobot::updateSpeeds(int left, int right) {
  speedLeft = left;
  speedRight = right;
  servoLeft->write(VG(speedLeft));
  servoRight->write(VD(speedRight));
}

void EasyRobot::directSpeeds(int left, int right) {
  servoLeft->write(VG(left));
  servoRight->write(VD(right));  
}

