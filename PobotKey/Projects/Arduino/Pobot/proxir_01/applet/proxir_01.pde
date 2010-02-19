/*
 * ProxIR connections to Arduino
 * -----------------------------
 * G - GND -- Gnd
 * T - TRM -- Digital Out 12
 * R - RCV -- Digital In  11
 * V - PWR -- 5V
 */
 
// la valeur précédente de la détection
int memoire = HIGH;  // HIGH = inactif pour le PROXIR

// nombre de ms au début de la détection
unsigned long compte = 0; 


void setup()
{
   pinMode(11, INPUT);   
   pinMode(12, OUTPUT);         
   
   digitalWrite(11,HIGH);       // pull-up sur l'entrée
   digitalWrite(12,HIGH);       // activer la led émettrice du capteur
  
   Serial.begin(9600);          // communiquer avec le PC
   
   memoire = digitalRead(11);
   
}

void loop()
{
  // ce qui va être envoyé à l'oscillo
  int val = 0;
  
  // la valeur courante du capteur
  int courant = digitalRead(11);
  
  // agir au début de la détection (front descendant) !! attention inversion
  if (memoire == HIGH && courant == LOW)
  {
    // remise à zéro du compteur
    compte = millis();

  } else
  
  // agir pendant la détection
  if (memoire == LOW && courant == LOW)
  {
    // on va envoyer la valeur du compteur (ms courantes - ms mémorisées)
     val = (int) millis()-compte;       
  } else
  
  // agir à la fin de la détection
  if (memoire == LOW && courant == HIGH)
  {
    val = (int) millis()-compte;
    // s'agissant de la fin, on pourrait ici lancer un traitement particulier
    
  }
  
  // préparer l'étape suivante en transférant l'état courant dans la mémoire
  memoire = courant;

  Serial.print(0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);  
 
}
