#include "MicroTimer2.h"

#include "WProgram.h"
void setup();
void loop();
void flash();
void setup()
{  
  MicroTimer2::set(500, flash); // 500ms period
  MicroTimer2::start();
}


void loop()
{
  
}


void flash() {
  static boolean output = HIGH;
  
  digitalWrite(13, output);
  output = !output;
}
int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

