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

#ifndef _EASY_ODO_H
#define _EASY_ODO_H

// conversion from degrees to radians and conversely
#define RAD_TO_DEG(r)  ((float)r*180.0/PI)
#define DEG_TO_RAD(d)  ((int)d*PI/180.0)



#include "WProgram.h"

#include "EasyRobot.h"

#define pinOdoLeft  2
#define pinOdoRight  3

// Class EasyOdo
class EasyOdo{

public:

  // true if we are tracking changes
  boolean flagZero;

  double x;
  double y;
  double theta;

  int counterLeft;
  int counterRight;
  
  int totalLeft;
  int totalRight;

  EasyOdo() {
    x = 0;
    y = 0;
    theta = 0;

    //
    pinMode(pinOdoLeft,INPUT);  
    pinMode(pinOdoRight,INPUT);    
  }

  void update(void);
  
  void attachRobot(EasyRobot *rob) {
      robot = rob;
  }
  
  void checkZero(void);

  void incrementLeft(void);

  void incrementRight(void);
  
private:

  EasyRobot *robot;

  double dTheta;
  double dDist;

};
#endif



