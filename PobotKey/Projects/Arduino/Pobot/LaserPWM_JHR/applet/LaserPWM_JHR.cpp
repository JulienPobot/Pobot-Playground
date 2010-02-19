#include "WProgram.h"


void setup();
void loop();
void setup()
{
  Serial.begin(9600);          // start serial communication at 9600bps
    
}


void loop()
{
  analogWrite(9,250);
}
int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

