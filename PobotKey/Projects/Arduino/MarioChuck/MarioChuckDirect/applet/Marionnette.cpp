#include "Marionnette.h"

void Marionnette::init()
{ 
  aNeckP.init(PIN_PAN,SMIN_PAN,SMAX_PAN,PMIN_PAN,PMAX_PAN,CMIN_PAN,CMAX_PAN,SQUARE_PAN); 
  aNeckT.init(PIN_TILT,SMIN_TILT,SMAX_TILT,PMIN_TILT,PMAX_TILT,CMIN_TILT,CMAX_TILT,SQUARE_TILT); 
  aEyesH.init(PIN_EYESH,SMIN_EYESH,SMAX_EYESH,PMIN_EYESH,PMAX_EYESH,CMIN_EYESH,CMAX_EYESH,SQUARE_EYESH); 
  aEyesV.init(PIN_EYESV,SMIN_EYESV,SMAX_EYESV,PMIN_EYESV,PMAX_EYESV,CMIN_EYESV,CMAX_EYESV,SQUARE_EYESV); 
  aMouth.init(PIN_MOUTH,SMIN_MOUTH,SMAX_MOUTH,PMIN_MOUTH,PMAX_MOUTH,CMIN_MOUTH,CMAX_MOUTH,SQUARE_MOUTH); 
}

void Marionnette::updateChuck(WiiChuck chuck) 
{  
  aNeckP.updateChuck(chuck.readRoll());
  aNeckT.updateChuck(chuck.readPitch());  
  aEyesH.updateChuck(chuck.readJoyX());
  aEyesV.updateChuck(chuck.readJoyY());  
  aMouth.updateChuck((chuck.buttonZ?1:0));
}

void Marionnette::writeFigure()
{
  // Send all information through serial connection
  Serial.print(aNeckP.readPosition());
  Serial.print(" ");
  Serial.print(aNeckT.readPosition());
  Serial.print(" ");
  Serial.print(aEyesH.readPosition());
  Serial.print(" ");  
  Serial.print(aEyesV.readPosition());
  Serial.print(" ");  
  Serial.print(aMouth.readPosition());
  Serial.print(" ");  
  Serial.println();
}
