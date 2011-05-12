#include <Servo.h>

double torse = 12;
double epaule = 11;
double coude = 10;

Servo sTorse;
Servo sEpaule;
Servo sCoude;

int i = 0;
int sens = 1;

void setup() {
  sTorse.attach(torse);
  sEpaule.attach(epaule);
  sCoude.attach(coude);
}



void loop() {
  //
  sTorse.write(30+2*i);
  sEpaule.write(30+1*i);
  sCoude.write(30+3*i);
  delay(100);
  //
  if (i > 80) {
     sens = 0;
  }
  if (i < 10) {
     sens = 1; 
  }
  
  if (sens == 0) {
     i-=1; 
  } else {
     i+=1; 
  }
}






