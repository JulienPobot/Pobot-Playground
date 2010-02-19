
#include "WProgram.h"
void setup();
void loop();
int xbee_id = 0x01;

int count1 = 0;
int count2 = 0;
int count3 = 0;


void setup()
{
  Serial.begin(9600);
}

void loop()
{
  while (Serial.available() > 0)
  {    
    int reception = Serial.read();
    switch(reception) {
      case '?':
        Serial.print("(");
        Serial.print(count1);
        Serial.print(",");
        Serial.print(count2);
        Serial.print(",");
        Serial.print(count3);
        Serial.println(")");
        break;
      case 0x01:
        count1++;
        break;
      case 0x02:
        count2++;
        break;
      case 0x03:
        count3++;
        break;
      default:
        Serial.print("Bien re\u00e7u ");
        Serial.println(reception);
        break;
    }
  }
  // toutes les secondes, envoie mon code
  Serial.write(xbee_id);
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

