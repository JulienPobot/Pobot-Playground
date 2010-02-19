
#include "WProgram.h"
void setup();
void version1();
void version2();
void loop();
void setup()
{
  Serial.begin(9600);  
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  
}

void version1()
{
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH); 
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);
  digitalWrite(3,LOW);
}

void version2()
{
  int i;
  for (i=3;i<=6;i++)
  {
    digitalWrite(i,HIGH);    
  }
  for (i=6;i>=3;i--)
  {
    digitalWrite(i,LOW);    
  }
}

void loop()
{
  if (Serial.available())
  {
    switch (Serial.read())
    {
    case '1':
      digitalWrite(2,HIGH);
      version1();
      digitalWrite(2,LOW);
      break;
    case '2':
      digitalWrite(2,HIGH);
      version2();
      digitalWrite(2,LOW);
      break;
    default:
      Serial.println("Unrecognized command.");        
    }    
  } 
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

