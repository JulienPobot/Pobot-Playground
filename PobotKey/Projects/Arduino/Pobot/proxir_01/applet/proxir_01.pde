/*
 * ProxIR connections to Arduino
 * -----------------------------
 * G - GND -- Gnd
 * T - TRM -- Digital Out 12
 * R - RCV -- Digital In  11
 * V - PWR -- 5V
 */
 
// la valeur pr�c�dente de la d�tection
int memoire = HIGH;  // HIGH = inactif pour le PROXIR

// nombre de ms au d�but de la d�tection
unsigned long compte = 0; 


void setup()
{
   pinMode(11, INPUT);   
   pinMode(12, OUTPUT);         
   
   digitalWrite(11,HIGH);       // pull-up sur l'entr�e
   digitalWrite(12,HIGH);       // activer la led �mettrice du capteur
  
   Serial.begin(9600);          // communiquer avec le PC
   
   memoire = digitalRead(11);
   
}

void loop()
{
  // ce qui va �tre envoy� � l'oscillo
  int val = 0;
  
  // la valeur courante du capteur
  int courant = digitalRead(11);
  
  // agir au d�but de la d�tection (front descendant) !! attention inversion
  if (memoire == HIGH && courant == LOW)
  {
    // remise � z�ro du compteur
    compte = millis();

  } else
  
  // agir pendant la d�tection
  if (memoire == LOW && courant == LOW)
  {
    // on va envoyer la valeur du compteur (ms courantes - ms m�moris�es)
     val = (int) millis()-compte;       
  } else
  
  // agir � la fin de la d�tection
  if (memoire == LOW && courant == HIGH)
  {
    val = (int) millis()-compte;
    // s'agissant de la fin, on pourrait ici lancer un traitement particulier
    
  }
  
  // pr�parer l'�tape suivante en transf�rant l'�tat courant dans la m�moire
  memoire = courant;

  Serial.print(0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);  
 
}
