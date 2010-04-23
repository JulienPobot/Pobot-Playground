/**
 * This source code is release under the MIT License.
 *
 * # Copyright (c) 2009 Julien Holtzer
 * #
 * # Permission is hereby granted, free of charge, to any person obtaining a copy
 * # of this software and associated documentation files (the "Software"), to deal
 * # in the Software without restriction, including without limitation the rights
 * # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * # copies of the Software, and to permit persons to whom the Software is
 * # furnished to do so, subject to the following conditions:
 * #
 * # The above copyright notice and this permission notice shall be included in
 * # all copies or substantial portions of the Software.
 * #
 * # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * # THE SOFTWARE.
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
  
  // normal control of the robot using speed for each motor
  void updateSpeeds(int left, int right);  
  
  // direct access (no effect on speeds)
  void directSpeeds(int left, int right);

private:
  Servo *servoLeft;
  Servo *servoRight;


};

#endif


