#include "Servo.h"
#include "PositionServo.h"

PositionServo serv;

int consigne;

void setup()
{
  pinMode(13,OUTPUT);
  serv.attach(10);
  //
  Serial.begin(38400);
  //
  consigne = 0;
  serv.write(consigne);

}

void loop()
{    
  while (Serial.available() > 0)
  {
    switch (Serial.read())
    {
    case '+':
      consigne += 10;
      serv.write(consigne);
      break;
    case '-':
      consigne -= 10;
      serv.write(consigne);
      break;
    case '0':
      serv.calibrate(LOW);
      break;
    case '1':
      serv.calibrate(HIGH);
      break;
    default:
      Serial.println("> unrecognized command");
    }
  }


  //
  Serial.println(serv.position());
  // 
  delay(333);

}
