/**
 * This source code is release under the MIT License.
 *
 */

#include "WProgram.h"
#include "EasyOdo.h"


/* 1 wheel rotation = PI radians = 40 tics = wheel diameter * PI mm
 * 1 tic = wheel diameter * PI / 40 mm
 */
#define TIC_MM  (WHEEL_MM * PI / 40)

/* 1 demi robot rotation with one wheel moving = robot diameter * PI mm = PI radians
 */
#define TIC_RAD  (TIC_MM / ENTRAXE_MM)

void EasyOdo::update()
{  
  // conversion from encoder counts into delta angle and delta distance
  // + conversion in mm and radians
  dTheta = (counterRight - counterLeft) * TIC_RAD;
  dDist = (counterRight + counterLeft) / 2 * TIC_MM;

  // reset counters (as soon as possible due to external interrupt routine)
  counterLeft = 0;
  counterRight = 0;

  // increment theta
  theta += dTheta;

  // control theta to always consider value between -PI and PI
  if (theta > PI) {
    theta -= 2*PI;
  }
  if (theta < -PI) {
    theta += 2*PI;
  }

  // trigonometric computation of new X,Y
  x += (dDist * cos(theta));
  y += (dDist * sin(theta));


}

void EasyOdo::incrementLeft(void) {
  if (robot->speedLeft > 0)
  {
    counterLeft++;
    totalLeft++;
  } 
  else {
    counterLeft--;    
    totalLeft--;
  }  
}

void EasyOdo::incrementRight(void) {
  if (robot->speedRight > 0)
  {
    counterRight++;
    totalRight++;
  } 
  else {
    counterRight--;    
    totalRight--;
  }  
}

boolean isCheckingZero = false;

double czTotalLeft;
double czTotalRight;

void EasyOdo::checkZero(void) {
  // output of the checking zero
  if (robot->speedRight != 0 || robot->speedLeft !=0) {
    isCheckingZero = false;
    return; 
  }
  // detect the start of checking zero
  if (! isCheckingZero) {
    if (robot->speedRight ==0 && robot->speedLeft == 0) {
      // start
      isCheckingZero = true;
      // remember current values
      czTotalLeft = totalLeft;
    }
  } 
  //
  if (isCheckingZero) {
    // normal behaviour : null speeds 
    robot->directSpeeds(0,0);
    // but can be changed by one or several of these :
    if (totalLeft < czTotalLeft) {
      robot->directSpeeds(1,0);  
    }

  }


}
