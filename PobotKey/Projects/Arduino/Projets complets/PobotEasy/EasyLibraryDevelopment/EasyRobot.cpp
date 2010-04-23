/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyRobot.h"


void EasyRobot::updateSpeeds(int left, int right) {
  consLeft = left;
  consRight = right;
  servoLeft->write(VG(speedLeft));
  servoRight->write(VD(speedRight));
}

void EasyRobot::directSpeeds(int left, int right) {
  servoLeft->write(VG(left));
  servoRight->write(VD(right));  
}

void EasyRobot::update(void)
{
  if (speedLeft < left);


}

