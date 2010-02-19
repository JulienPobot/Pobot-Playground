// The Arduino code.

#define ANALOG_IN 0

#include "WProgram.h"
void setup();
void loop();
void setup() {
  Serial.begin(38400); 
}

void loop() {
  int val = analogRead(ANALOG_IN);
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

