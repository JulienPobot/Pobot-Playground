#include "Articulation.h"

void Articulation::init(int pin, int valsmin, int valsmax, int valpmin, int valpmax, int valcmin, int valcmax, boolean valsquare) {
  servo.attach(pin);
  smin = valsmin;
  pmin = valpmin;
  cmin = valcmin;
  cmax = valcmax;
  square = valsquare;

  pcoeff = (valsmax - valsmin) / (float) (valpmax - valpmin);
  ccoeff = (valsmax - valsmin) / (float) (valcmax - valcmin);
}

void Articulation::updateChuck(int value) {    
  //
  if (square) {
    value = value*value;
  }
  // first filter to not take into account values out of range  
  if (value < cmin)
  {
    value = cmin;
  }
  if (value > cmax) {
    value = cmax;
  }
  svalue = smin + (value - cmin) * ccoeff;
  servo.write(svalue);  
}

void Articulation::updatePosition(int value) {    
  svalue = smin + (value - pmin) * pcoeff;
  servo.write(svalue);  
}

int Articulation::readPosition() {
  return pmin + (svalue-smin) / pcoeff;  

}

