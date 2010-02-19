#include "WProgram.h"
void setup();
void loop ();
int LED = 13;   // select the pin for the LED
int RESET = 7;

void setup() {
  pinMode(LED,OUTPUT);   // declare the LED's pin as output
  pinMode(RESET,OUTPUT);   // declare the reset pin as output
  Serial.begin(115200);        // connect to the serial port
  digitalWrite(RESET, HIGH);
  delay(10);
  digitalWrite(RESET, LOW);
  delay(2000);
}

void loop () {
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  Serial.println("ciao");
  delay(1000);
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

