#include <Servo.h>

Servo tourelle;
Servo bras1;
Servo bras2;


void setup() {
   tourelle.attach(3);
   bras1.attach(8);
   bras2.attach(9);
   tourelle.write(90); 
   Serial.begin(9600);
}

void loop() {
  tourelle.write(70);
  bras1.write(60);
  bras2.write(60);
  delay(4000);
  tourelle.write(120);
  bras1.write(120);
  bras2.write(120);
  delay(4000);
  Serial.println(".");
  
}  


