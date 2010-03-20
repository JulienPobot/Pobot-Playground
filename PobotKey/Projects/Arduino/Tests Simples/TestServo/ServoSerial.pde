#include <Servo.h>

/** 
* Utilisation de la bibliothèque "servo"
*/

// déclaration du servo comme objet C++
Servo servo;


/**
 * Initialisation du programme
 */
void setup()
{
  servo.attach(9);

  Serial.begin(19200);
  Serial.print("Ready");
}
/**
 * Boucle infinie qui traite les consignes reçues de l'utilisateur
 * et le transforme en rotation du servomoteur
 */
void loop()
{
  static int v = 0; // l'angle souhait�
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
      servo.write(v);
      v = 0;
      break;
    }
  }
}
