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

#define MAX_SPEED  15

// use "Calibre Servo" Arduino program to know the value of immobile position for each servo
// #define POS_NEUTRAL_RIGHT  77
// P#define POS_NEUTRAL_LEFT   72

// servomotors pins
#define pinLeft  9
#define pinRight  10

// ajout de deux macros qui calculent la consigne en adaptant 
// - la consigne constatée pour vitesse de rotation minimale (si possible nulle)
// - le signe qui rétablira l'inversion des deux moteurs face à face
#define VG(val)  ((val<0)?72+val:71+val)
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
    consLeft = 0;
    consRight = 0;
  }

  void enableServos(void) {    
    servoLeft->attach(pinLeft);
    servoRight->attach(pinRight);
  }

  void disableServos(void) {
    servoLeft->detach();
    servoRight->detach(); 
  }

  void attachServo(Servo *s1, Servo *s2) {  
    servoLeft = s1;
    servoRight = s2;
    enableServos();
  }

  // mise à jour des vitesses selon les consignes
  void updateSpeeds(void);  

  // mise à jour des consignes de vitesse
  void penteSpeeds(int left, int right);

  // modification directe des vitesses (annule les consignes)
  void directSpeeds(int left, int right);
  
  // arrêt complet du robot
  void stopRobot(void) {
    penteSpeeds(0,0);
    delay(200);
    directSpeeds(0,0);
    disableServos();    
  }

private:
  Servo *servoLeft;
  Servo *servoRight;


};

#endif





