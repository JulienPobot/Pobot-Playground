/**
 * Test simple d'un LM335Z
 * 
 * 
 */
 
#include "WProgram.h"
void setup();
void loop();
int tempPin = 0;


void setup()
{
  Serial.begin(38400);

}


void loop()
{
  int val = analogRead(tempPin);
  
  // 1 K = 10 mV
  // 1024 = 5000 mV
  // 0 = 0 K
  // 1024 = 500 K
  double kelvin = ((double) val * 500.0) / 1024.0;
  
  // 0 Kelvin = -273,15\u00b0C
  double degres = kelvin - 273.15;
  
  Serial.print("> ");
  Serial.print(val);
  Serial.print(" ");
  Serial.print(degres);
  
  Serial.println();  
  
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

