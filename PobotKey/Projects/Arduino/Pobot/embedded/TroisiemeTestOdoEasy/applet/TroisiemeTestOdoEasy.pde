#include <Servo.h>

Servo servL;
Servo servR;

int consigneLeft = 0;
long counterLeft = 0;

int consigneRight = 0;
long counterRight = 0;

long temps = 0;

void setup() {
  // init serial communication
  Serial.begin(115200); 
  //  
  servL.attach(10);
  servR.attach(9);
  //
  pinMode(3,INPUT);  
  pinMode(4,INPUT);  

  attachInterrupt(0, interruptLeft, CHANGE);
  attachInterrupt(1, interruptRight, CHANGE);
  //
  consigneLeft = 1;
  consigneRight = 1;
}

void loop() {
  //
  servL.write(counterLeft%255); 
  servR.write(counterRight%255);
  //  
  Serial.print((int) counterLeft);
  Serial.print(" ");
  Serial.print((int) counterRight);
  Serial.print(" ");
  Serial.print((consigneLeft+consigneRight)*500);
  Serial.print(" ");
  Serial.println();
  delay(50);

}

void interruptLeft()
{
  if (consigneLeft > 0)
  {
    counterLeft++;
  } 
  else {
    counterLeft--;    
  }
}

void interruptRight()
{
  if (consigneRight > 0)
  {
    counterRight++;
  } 
  else {
    counterRight--;    
  }
}
