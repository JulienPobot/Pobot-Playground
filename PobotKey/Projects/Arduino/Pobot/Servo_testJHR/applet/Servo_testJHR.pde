#include <Servo.h>
// on déclare le servomoteur comme une variable typée
Servo servo1;
/**
 * Initialisation du programme
 */
void setup()
{
  servo1.attach(9); // le servomoteur est sur ANALOG 0
  
  Serial.begin(38400); // communication avec la console série
  Serial.print("Ready");
}
/**
 * Boucle infinie qui traite les consignes reçues de l'utilisateur
 * et le transforme en rotation du servomoteur
 */
void loop()
{
  static int v = 0; // l'angle souhaité
  //
  int val = analogRead(0);
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);
  //
  if ( Serial.available()) {
    // récupérer le caractère suivant disponible sur la liaison série
    char ch = Serial.read();
    switch(ch) {
    case '0'...'9':
      // construire la valeur saisie
      v = v * 10 + ch - '0';
      break;
    case 's':
      // envoyer la consigne au servomoteur
      servo1.write(v);
      v = 0;
      break;
    }
  }
}
