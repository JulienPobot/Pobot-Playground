/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasySquare.h"

void EasySquare::start()
{
  side = 0;

  odo->x = 0;
  odo->y = 0;
  odo->theta = 0;   
  robot->updateSpeeds(MAX_SPEED,MAX_SPEED);  
}


void EasySquare::update()
{
  switch (side)
  {
  case 0:
    if (odo->x >= SQUARE_SIZE) {
      // first side is finished, turn
      robot->updateSpeeds(-MAX_SPEED,MAX_SPEED);
    }
    if (odo->theta >= PI/2) {
      // first rotation is finished, side is finished, next
      side++;
      robot->updateSpeeds(MAX_SPEED,MAX_SPEED);
    }
    break;
  case 1:
    if (odo->y >= SQUARE_SIZE) {
      // second side is finished, turn
      robot->updateSpeeds(-MAX_SPEED,MAX_SPEED);
    }
    if (odo->theta >= PI || odo->theta < DEG_TO_RAD(-150)) { // second test is very large but sufficient
      // second rotation is finished, side is finished, next
      side++;
      robot->updateSpeeds(MAX_SPEED,MAX_SPEED);
    }
    break;
  case 2:
    if (odo->x <= 0) {
      // third side is finished, turn
      robot->updateSpeeds(-MAX_SPEED,MAX_SPEED);
    }
    if (odo->theta >= -PI/2) {
      // third rotation is finished, side is finished, next
      side++;
      robot->updateSpeeds(MAX_SPEED,MAX_SPEED);
    }
    break;
  case 3:
    if (odo->y <= 0) {
      // fourth side is finished, turn
      robot->updateSpeeds(-MAX_SPEED,MAX_SPEED);
    }
    if (odo->theta >= 0) {
      // first rotation is finished, side is finished, next
      side++;
      robot->updateSpeeds(0,0);
    }
    break;
  default:
    // error
    digitalWrite(13,HIGH);
  }

}
