/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyRobot.h"

void EasyRobot::updateSpeeds(void) {  
  //
  if (speedLeft < consLeft) 
  {
   speedLeft++; 
  }
  //
  if (speedLeft > consLeft) 
  {
   speedLeft--; 
  }
  //
  if (speedRight < consRight) 
  {
   speedRight++; 
  }
  //
  if (speedRight > consRight) 
  {
   speedRight--; 
  }
  // réactiver les servos si besoin
  if (speedLeft != 0 || speedRight != 0)
  {
   enableServos(); 
  }  
  // 
  servoLeft->write(VG(speedLeft));
  servoRight->write(VD(speedRight));   
}

void EasyRobot::penteSpeeds(int left, int right) {
  consLeft = left;
  consRight = right; 
}

void EasyRobot::directSpeeds(int left, int right) {
  consLeft = left;
  consRight = right;
  speedLeft = left;
  speedRight = right;
  updateSpeeds();
}


