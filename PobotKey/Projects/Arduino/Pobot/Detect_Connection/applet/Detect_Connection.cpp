
#include "WProgram.h"
void setup();
void loop();
boolean isConnected(void);
void setup()
{
  Serial.begin(115200);   
  pinMode(14,INPUT); 
  digitalWrite(14,HIGH);  
}

void loop()
{  
  Serial.print(analogRead(0));
  Serial.print(" ");
  Serial.print(isConnected()?500:0);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.println();
  delay(50);  
}

boolean isConnected(void) 
{
  int count = 0;
  // mettre \u00e0 z\u00e9ro
  pinMode(14,OUTPUT);
  digitalWrite(14,LOW);
  pinMode(14,INPUT);  
  digitalWrite(14,HIGH);
  while (digitalRead(14) != HIGH) {
    count++;
    if (count > 10) return true;
  }
  return analogRead(0) != 1023 || count != 0;
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

