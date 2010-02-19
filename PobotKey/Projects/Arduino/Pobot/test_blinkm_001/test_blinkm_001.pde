
#include <Wire.h>
#include "BlinkM_funcs.h"

void setup()
{
  //
  BlinkM_beginWithPower();
  BlinkM_setAddress(0x09);
  BlinkM_setStartupParams(0x00,1,13,10,0x20,0);
  
}

void loop()
{
  BlinkM_playScript(0x09,13,0x20,0); 
  
  
}
