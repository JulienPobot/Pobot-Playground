

#include "WProgram.h"
void setup(void);
void loop(void);
void setup(void)
{
   Serial.begin(9600);  
   byte val2 = -5; 
   
   Serial.print("> -5 : ");
   Serial.println(val2,BIN);
   
   Serial.print("> -5*2 : ");
   Serial.println(2*val2,BIN);
   

}

void loop(void)
{


}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

