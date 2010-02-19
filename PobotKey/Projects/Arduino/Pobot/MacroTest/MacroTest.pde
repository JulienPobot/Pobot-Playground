#define SEUIL    10

#define VAL_1    930
#define VAL_2    960

int val;

#define  PRESS_1(pin)  ( (val = analogRead(pin)) && val > VAL_1 - SEUIL && val < VAL_1+SEUIL)

#define  PRESS_2(pin)  (analogRead(pin) > 512)



void setup()
{
  Serial.begin(9600); 
  
}


void loop()
{
  if (PRESS_1(1)) {
     Serial.println("Choix 1 !");
  } 
  
  if (PRESS_2(1)) {
     Serial.println("Choix 2 !"); 
  }
  
  delay(500);
}
