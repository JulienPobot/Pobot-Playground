#include "WProgram.h"

// sur la carte Roméo, le moteur A est sur D4 et D5
// et le moteur B est sur D6 et D7

class ZarRP5 {

public:

  void init()
  {  
    // moteur gauche
    pinMode(4,OUTPUT); // direction
    pinMode(5,OUTPUT); // vitesse
    // moteur droit
    pinMode(6,OUTPUT); // vitesse
    pinMode(7,OUTPUT); // direction
  }

  void arrete()
  {
    digitalWrite(5,LOW);
    digitalWrite(6,LOW); 
  }

  void avance(int vitesse)
  {
    // les deux moteurs en avant
    digitalWrite(4,HIGH);
    digitalWrite(7,LOW);
    // vitesse donnée en paramètre
    analogWrite(5,vitesse);
    analogWrite(6,vitesse);    
  }

};



