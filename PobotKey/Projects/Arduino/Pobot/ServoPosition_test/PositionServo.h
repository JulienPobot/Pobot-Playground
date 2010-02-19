#ifndef PositionServo_h
#define PositionServo_h

#include "WProgram.h"

class PositionServo : 
public Servo
{
private:
  int pinAnalog = 0;

  int valLow = 0;
  int valHigh = 1024;

public:
  PositionServo() : 
  Servo() {
  };    

  uint8_t attach(int pinArg) {  
    this->Servo::attach(pinArg); 
  };

  void calibrate(int level) { 
    if (level == LOW) {
      valLow = analogRead(pinAnalog);
    } 
    else {
      valHigh = analogRead(pinAnalog);
    }
  };

  int position() { 
    return analogRead(pinAnalog)*; 
  };

};

#endif
