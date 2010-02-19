#include "WProgram.h"
/*
 * ProxIR connections to Arduino
 * -----------------------------
 * G - GND -- Gnd
 * T - TRM -- Digital Out 12
 * R - RCV -- Digital In  11
 * V - PWR -- 5V
 */
 
// la valeur pr\u00e9c\u00e9dente de la d\u00e9tection
void setup();
void loop();
int memoire = HIGH;  // HIGH = inactif pour le PROXIR

// nombre de ms au d\u00e9but de la d\u00e9tection
unsigned long compte = 0; 


void setup()
{
   pinMode(11, INPUT);   
   pinMode(12, OUTPUT);         
   
   digitalWrite(11,HIGH);       // pull-up sur l'entr\u00e9e
   digitalWrite(12,HIGH);       // activer la led \u00e9mettrice du capteur
  
   Serial.begin(9600);          // communiquer avec le PC
   
   memoire = digitalRead(11);
   
}

void loop()
{
  // ce qui va \u00eatre envoy\u00e9 \u00e0 l'oscillo
  int val = 0;
  
  // la valeur courante du capteur
  int courant = digitalRead(11);
  
  // agir au d\u00e9but de la d\u00e9tection (front descendant) !! attention inversion
  if (memoire == HIGH && courant == LOW)
  {
    // remise \u00e0 z\u00e9ro du compteur
    compte = millis();

  } else
  
  // agir pendant la d\u00e9tection
  if (memoire == LOW && courant == LOW)
  {
    // on va envoyer la valeur du compteur (ms courantes - ms m\u00e9moris\u00e9es)
     val = (int) millis()-compte;       
  } else
  
  // agir \u00e0 la fin de la d\u00e9tection
  if (memoire == LOW && courant == HIGH)
  {
    val = (int) millis()-compte;
    // s'agissant de la fin, on pourrait ici lancer un traitement particulier
    
  }
  
  // pr\u00e9parer l'\u00e9tape suivante en transf\u00e9rant l'\u00e9tat courant dans la m\u00e9moire
  memoire = courant;

  Serial.print(0xff, BYTE);
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

