#define PILOT_A 11     // entr�e A du pont en H
#define PILOT_B 12     // entr�e B du pont en H
#define PILOT_PWM 6   // entr�e "Enable" du pont en H


void direction (byte dir)
{
  if (dir == 0)
  {
    digitalWrite (PILOT_A, HIGH);
    digitalWrite (PILOT_B, LOW);
  }
  else
  {
    digitalWrite (PILOT_A, LOW);
    digitalWrite (PILOT_B, HIGH);
  }
}


void setup()
{
  pinMode(PILOT_A, OUTPUT);
  pinMode(PILOT_B, OUTPUT);
  pinMode(PILOT_PWM, OUTPUT);
  direction(0); 
  
}


void loop()
{
  
  int valeur = analogRead(0);
    
  analogWrite(PILOT_PWM, analogRead(0)*2);
}
