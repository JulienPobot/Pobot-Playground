#include "SoftwareServo.h"

SoftwareServo servo1;

void setup()
{
  pinMode(13,OUTPUT);
  servo1.attach(5);
  servo1.setMaximumPulse(2200);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Ready");
}

void loop()
{
  static int value = 0;

  if ( Serial.available() > 0) {
    char ch = Serial.read();
    switch(ch) {
    case '0' ... '9':
      value=(ch-'0')*20;
      servo1.write(value);
      Serial.println();

      break;
    default:
      Serial.print("Undefined command : ");
      Serial.print(ch);
      Serial.println();
      break;       
    }
  }
  delay(20);
  SoftwareServo::refresh();
}
