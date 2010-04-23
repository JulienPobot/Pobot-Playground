/**
 * This source code is release under the MIT License.
 *
 */

#ifndef _EASY_ROBOT_H
#define _EASY_ROBOT_H

#include "WProgram.h"
#include "Servo.h"

// mechanical values
#define ENTRAXE_MM  145
#define WHEEL_MM  64


// use "Calibre Servo" Arduino program to know the value of immobile position for each servo
// #define POS_NEUTRAL_RIGHT  77
// P#define POS_NEUTRAL_LEFT   72

// servomotors pins
#define pinLeft  9
#define pinRight  10

// ajout de deux macros qui calculent la consigne en adaptant 
// - la consigne constatée pour vitesse de rotation minimale (si possible nulle)
// - le signe qui rétablira l'inversion des deux moteurs face à face
#define VG(val)  (val+72)
#define VD(val)  ((val<0)?76-val:77-val)


// Class EasyRobot
class EasyRobot{

public:

  int speedLeft; 
  int speedRight;
  
  int consLeft;
  int consRight;

  EasyRobot() {
    //
    speedLeft = 0;
    speedRight = 0;
  }

  void attachServo(Servo *s1, Servo *s2) {  
    servoLeft = s1;
    servoRight = s2;
    servoLeft->attach(pinLeft);
    servoRight->attach(pinRight);
  }
  
  // pente
  void update(void);
  
  // normal control of the robot using speed for each motor
  void updateSpeeds(int left, int right);  
  
  // direct access (no effect on speeds)
  void directSpeeds(int left, int right);

private:
  Servo *servoLeft;
  Servo *servoRight;


};

#endif


