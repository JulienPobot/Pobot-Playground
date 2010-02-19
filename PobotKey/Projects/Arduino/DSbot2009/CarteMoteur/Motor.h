#ifndef MOTOR_H
#define MOTOR_H

#include "WProgram.h"

class Motor {
public:
  // set pins and init them 
  void init(unsigned char pwm, unsigned char in1, unsigned char in2){
    pinPwm = pwm;
    pinIn1 = in1;
    pinIn2 = in2;
    pinMode(pinIn1,OUTPUT);
    pinMode(pinIn2,OUTPUT);
    pinMode(pinPwm,OUTPUT);
    set_speed(0);  
  }
  
  // set speed and rotation direction. Positive speed is forward, negative is backward.
  void set_speed(int m_speed){
    unsigned char corrected_speed;
    if(m_speed>0){ // forward  
      digitalWrite(pinIn1, LOW );
      digitalWrite(pinIn2, HIGH);
      corrected_speed = (unsigned char)constrain(m_speed,0,255);
    } else if(m_speed<0){ // backwards
      digitalWrite(pinIn1, HIGH);
      digitalWrite(pinIn2, LOW );
      corrected_speed = (unsigned char)constrain(-m_speed,0,255);
    }else {    // standby in any other case
      digitalWrite(pinIn1, HIGH);
      digitalWrite(pinIn2, HIGH);
      corrected_speed = 0;
    }
    // set speed
    analogWrite(pinPwm, corrected_speed);
  }
private:
  char pinPwm;
  char pinIn1;
  char pinIn2;  
};


#endif