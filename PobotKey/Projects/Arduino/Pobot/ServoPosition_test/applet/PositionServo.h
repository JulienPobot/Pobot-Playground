#ifndef PositionServo_h
#define PositionServo_h

#include "WProgram.h"

class PositionServo : public Servo
{
  private:
    int pinAnalog;
  
  public:
    PositionServo() : Servo() {};    

    uint8_t attach(int pinArg) {  this->Servo::attach(pinArg); };
    
    int position() { return analogRead(pinAnalog); };
    
};

#endif
