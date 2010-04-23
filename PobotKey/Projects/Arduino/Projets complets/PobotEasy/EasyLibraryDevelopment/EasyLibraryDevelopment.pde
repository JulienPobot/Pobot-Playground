/**
 * This source code is release under the MIT License.
 *
 */

/** Inspired by : 
 * - http://www.arduino.cc/en/Hacking/LibraryTutorial
 */

#include <MsTimer2.h>
#include <Servo.h>

#include "EasyOdo.h"
#include "EasyRobot.h"
#include "EasyPince.h"
#include "EasySquare.h"

Servo servo1;
Servo servo2;

Servo servo3;
Servo servo4;

EasyRobot robot;
EasyOdo odo;
EasySquare movesquare;
EasyPince pince;

// two external interrupt functions
void interruptLeft(void);
void interruptRight(void);

//
void setup() {
  // init serial communication
  Serial.begin(38400); 
  Serial.print("Init odometry test");
  // init timer for regular scheduled operations
  MsTimer2::set(250, triggerTimer);
  MsTimer2::start();

  //
  attachInterrupt(0, interruptLeft, CHANGE);
  attachInterrupt(1, interruptRight, CHANGE);  

  // the base components : a robot with odometry
  odo.attachRobot(&robot);
  robot.attachServo(&servo1, &servo2);

  // the movement control
  movesquare.attachRobot(&robot);
  movesquare.attachOdo(&odo);

  // la pince
  pince.attachServo(&servo3, &servo4);

  // stop the robot
  robot.updateSpeeds(0,0);    

  // here, select the strategy
  //movesquare.start();
}

void loop() {

  // movesquare.update();  
  odo.checkZero();
  
  // pince.testPince();


  /*
  if (millis()%1000 == 0) {
  /*
   Serial.print("G ");
   Serial.print(odo.counterLeft);
   Serial.print(" D ");
   Serial.print(odo.counterRight);
   */
  /*
    Serial.print(" X ");
   Serial.print(odo.x);
   Serial.print(" Y ");
   Serial.print(odo.y);
   Serial.print(" A ");
   Serial.print(odo.theta/PI*180.0);  
   Serial.print(" L ");
   Serial.print(robot.speedLeft);
   Serial.print(" R ");
   Serial.print(robot.speedRight);
   Serial.print(" S ");
   Serial.print(movesquare.side);
   Serial.println();
   }
   */
}


void triggerTimer(void)
{
  odo.update();    
}

void interruptLeft(void)
{
  odo.incrementLeft();
}

void interruptRight(void)
{
  odo.incrementRight();  
}


