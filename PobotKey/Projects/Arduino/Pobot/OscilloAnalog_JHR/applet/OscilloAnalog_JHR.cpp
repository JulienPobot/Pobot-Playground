// The Arduino code.


#include "WProgram.h"
void setup();
void loop();
void setup() {
  Serial.begin(38400); 
}

void loop() {
  int val = analogRead(0);
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

