#include "danger.h"

int i = 0;

Danger d;

void setup()
{
  ds_init();
}

void loop()
{
  i++;
  if (i>255) { 
    i = 0;
  }
  analogWrite(3,i*10);
  delay(100);

}
