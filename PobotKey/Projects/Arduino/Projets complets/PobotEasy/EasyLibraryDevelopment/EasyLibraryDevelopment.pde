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
//#include "EasyAsserv.h"

Servo servo1;
Servo servo2;

Servo servo3;
Servo servo4;

int triggerCount;

EasyRobot robot;
EasyOdo odo;
EasyPince pince;
//EasyAsserv asserv;

// two external interrupt functions
void interruptLeft(void);
void interruptRight(void);

//
void setup() {
  // init serial communication
  Serial.begin(38400); 
  Serial.print("Init odometry test");
  // init timer for regular scheduled operations
  MsTimer2::set(200, triggerTimer);
  MsTimer2::start();

  //
  attachInterrupt(0, interruptLeft, CHANGE);
  attachInterrupt(1, interruptRight, CHANGE);  

  // the base components : a robot with odometry
  odo.attachRobot(&robot);
  robot.attachServo(&servo1, &servo2);

  // la pince
  pince.attachServo(&servo3, &servo4);

  // l'asservissement
  //asserv.attachRobot(&robot);
  //asserv.attachOdo(&odo);

  // stop the robot
  robot.directSpeeds(0,0);    

  pince.fermePince();  
  pince.levePince();


}

void loop() {


  //odo.checkZero();

  //pince.testPince();

  /*
  robot.penteSpeeds(15,15);
   
   delay(5000);
   
   robot.penteSpeeds(0,0);
   
   delay(5000);
   
   robot.penteSpeeds(-15,-15);
   
   delay(5000);
   
   robot.stopRobot();
   
   delay(5000);
   delay(5000);
   */

  //pince.testPince();



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
   Serial.print(" S ");
   Serial.print(movesquare.side);
   Serial.println();
   }
   */

  delay(100);
}


void triggerTimer(void)
{  
  triggerCount++;
  odo.update();    
  robot.updateSpeeds();
  //asserv.checkAsserv();
  if (triggerCount >= 5) 
  {

    triggerCount = 0;
  }
}

void interruptLeft(void)
{
  odo.incrementLeft();
}

void interruptRight(void)
{
  odo.incrementRight();  
}





