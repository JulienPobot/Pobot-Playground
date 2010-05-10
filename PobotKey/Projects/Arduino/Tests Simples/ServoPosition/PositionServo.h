/**
 * PositionServo.h
 *
 * Classe fille de la bibliothèque "Servo"
 * définissant les fonctions spécifiques au retour d'informations
 *
 * (c) 2009 - J. Holtzer pour Pobot 
 */

#ifndef PositionServo_h
#define PositionServo_h

#include "WProgram.h"

// déclaration de la classe et de l'héritage de Servo
class PositionServo : 
public Servo
{

  // ajout des variables du retour d'informations
private:

  // la patte où est connecté le câble de retour d'info
  int pinAnalog;

  // le nombre de degrés total entre les deux positions extrema
  int totalDegres;

  // la valeur correspondant à la position la plus basse 
  int valLow; 

  // la valeur correspondant à la position la plus haute
  int valHigh; 

public:

  PositionServo() : 
  Servo() {
    // patte ANALOG 0
    pinAnalog = 0;
    // 180 degrés de développement standard
    totalDegres = 180;
    // 205 = 1 volt pour 1024 = 5 volts (ADC 10 bits)
    valLow = 205;
    // 615 = 3 volts pour 1024 = 5 volts (ADC 10 bits)
    valHigh = 615;
  };    

  // personnalisation des valeurs (connexion et débattement)
  void set(int pinArg, int degres) {
    pinAnalog = pinArg;
    totalDegres = degres;
  };

  // calibrage des positions extrema
  void calibrate(int level) { 
    if (level == LOW) {
      valLow = analogRead(pinAnalog);
    } 
    else {
      valHigh = analogRead(pinAnalog);
    }
  };

  // affichage de la position (valeur analogique brute)
  int position() { 
    return analogRead(pinAnalog); 
  };

  // affichage de la position (valeur en degrés entre 0 et total degrés)
  int degres() {
    return (position()-valLow)*totalDegres/(valHigh-valLow);   
  }

};

#endif

