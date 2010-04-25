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
#include "EasyTurnGo.h"

Servo servo1;
Servo servo2;

Servo servo3;
Servo servo4;

// le compteur des 90 secondes
long start;

int triggerCount;

EasyRobot robot;
EasyOdo odo;
EasyPince pince;
//EasyAsserv asserv;
EasyTurnGo tg;

// two external interrupt functions
void interruptLeft(void);
void interruptRight(void);

//
void setup() {
  // init serial communication
  Serial.begin(38400); 
  Serial.print("Init odometry test");
  // init timer for regular scheduled operations
  MsTimer2::set(100, triggerTimer);
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

  // le turn & go
  tg.attachRobot(&robot);
  tg.attachOdo(&odo);

  // stop the robot
  robot.directSpeeds(0,0);    

  pince.fermePince();  
  pince.levePince();
  delay(200);
  pince.baissePince(); 
  delay(200);
  pince.levePince();

  robot.stopRobot();

  while (analogRead(1) > 250) {
    delay(50); 

    start = 0;

  }

}

void loop() {

  tg.avance(500,10);

  robot.stopRobot();

  pince.baissePince();

  delay(1000);  
  
  tg.avance(500,10);
  
  pince.ouvrePince();
  delay(500);
  pince.fermePince();
    
  tg.avance(700,10);

  robot.stopRobot();

  delay(2000);

}


void triggerTimer(void)
{  
  //triggerCount++;
  odo.update();    
  robot.updateSpeeds();
  //asserv.checkAsserv();

  if (millis() - start > 90000)
  {
     robot.stopRobot();
     // boucle infinie
     while (1) {
      start++; 
     }
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









