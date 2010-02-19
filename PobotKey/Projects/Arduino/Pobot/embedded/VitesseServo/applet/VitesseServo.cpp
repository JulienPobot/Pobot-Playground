/**
 * @author Julien Holtzer - julien.holtzer@gmail.com
 * @date 31-08-2009
 *
 */

#include <Servo.h>

// servo object representing the motor
#include "WProgram.h"
void setup();
void loop();
void blink();
Servo serv;

// servo connection to the Arduino (digital output 9 or 10)
int servPin = 10;
int odoPin = 0;

int counter = 0;

long timer = 0;

int vitesse = -1;


void setup()
{
  // configure your serial terminal to 9600, N, 8, 1 (standard configuration for Arduino)
  Serial.begin(9600);
  // attach the servo with standard values
  serv.attach(servPin);
  // attach the odometer interrupt
  attachInterrupt(odoPin, blink, CHANGE);
}

void loop()
{
  if (timer == 0) {
    vitesse++;
    serv.write(vitesse);
    counter = 0;
    timer = millis();    
  }
  if (millis()-timer > 5000) {
    // 10 seconds
    timer = 0;
    // send value of counter and corresponding speed
    Serial.print(vitesse);
    Serial.print("\t"); // tab for Excel csv record
    Serial.print(counter);
    Serial.println();
  }
}

void blink()
{  
  counter++;
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

