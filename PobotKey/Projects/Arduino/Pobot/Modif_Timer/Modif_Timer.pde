#include "MicroTimer2.h"

void setup()
{  
  MicroTimer2::set(500, flash); // 500ms period
  MicroTimer2::start();
}


void loop()
{
  
}


void flash() {
  static boolean output = HIGH;
  
  digitalWrite(13, output);
  output = !output;
}
