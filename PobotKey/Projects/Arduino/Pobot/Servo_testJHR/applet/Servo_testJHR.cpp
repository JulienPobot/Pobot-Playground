#include <Servo.h>
// on d\u00e9clare le servomoteur comme une variable typ\u00e9e
#include "WProgram.h"
void setup();
void loop();
Servo servo1;
/**
 * Initialisation du programme
 */
void setup()
{
  servo1.attach(9); // le servomoteur est sur ANALOG 0
  
  Serial.begin(38400); // communication avec la console s\u00e9rie
  Serial.print("Ready");
}
/**
 * Boucle infinie qui traite les consignes re\u00e7ues de l'utilisateur
 * et le transforme en rotation du servomoteur
 */
void loop()
{
  static int v = 0; // l'angle souhait\u00e9
  //
  int val = analogRead(0);
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);
  //
  if ( Serial.available()) {
    // r\u00e9cup\u00e9rer le caract\u00e8re suivant disponible sur la liaison s\u00e9rie
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

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

