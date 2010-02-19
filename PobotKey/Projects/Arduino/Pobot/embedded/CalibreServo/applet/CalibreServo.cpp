/**
 * @author Julien Holtzer - julien.holtzer@gmail.com
 * @date 23-05-2009
 *
 * This code is intended to test the "zero" position of a 360\u00c3\u00af\u00c2\u00bf\u00c2\u00bd servomotor.
 * Modified servomotor Futaba S3003 used on the Pobot Easy mobile robot 
 * is turning continuously in one direction or another depending on the 
 * standard servo command.
 * 
 * Considering that a Servo (in standard Arduino 0013 library) can receive 
 * an angle in degree as command between 0 and 180, the zero should be 90,
 * but due to the physical modification, it isn't that simple. 
 * 
 * This code will help the user to test several values sent via a serial terminal
 * for tuning the control software of any robot using this kind of modified servo. 
 */

#include <Servo.h>

// servo object representing the motor
#include "WProgram.h"
void setup();
void loop();
Servo serv;

// servo connection to the Arduino (digital output 9 or 10)
int servPin = 9;

// default value that will be updated when the user sends a new value 
int valeur = 90;

void setup()
{
  // configure your serial terminal to 9600, N, 8, 1 (standard configuration for Arduino)
  Serial.begin(9600);
  // attach the servo with standard values
  serv.attach(servPin);
}

void loop()
{
  // test if a new value has been received
  if (Serial.available())
  {
    // update without any control
    valeur = Serial.read();  
  }

  // update the command of the servo
  serv.write(valeur);
    
  // confirm the current value (equivalent to a servo position but continously rotating)
  Serial.print("> position: ");
  Serial.print(valeur);
  Serial.println();
  
  // 1/2 second delay to avoid saturation (don't update values more than twice per second)
  delay(500); 
  
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

