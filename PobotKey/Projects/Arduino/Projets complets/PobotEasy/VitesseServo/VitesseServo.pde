/**
 * Outil de calibrage des moteurs d'un Pobot Easy
 *
 * - deux odomètres sur interruptions externes
 * - deux servomoteurs
 *
 * @author Julien pour Pobot (julien@pobot.org)
 * @date 31-08-2009
 *
 */

// utilisation de la bibliothèque "Servo" pour Arduino
// fonctionne avec toute entrée numérique ("Digital")
#include <Servo.h>

// déclaration des deux objets qui représentent les servomoteurs
Servo servG;
Servo servD;

// connections du servomoteur gauche
int servGPin = 9;
int odoGPin = 0; // 0 pour la patte Digital 2, 1 pour la patte Digital 3

// connections du servomoteur droit
int servDPin = 10;
int odoDPin = 1; // 0 pour la patte Digital 2, 1 pour la patte Digital 3

// les deux compteurs pour incréments des odomètres
int counterG = 0;
int counterD = 0;

// la vitesse en cours de test
int vitesse = 59;

// utilisation d'une mémoire pour le démarrage d'une mesure
// attention, "long" obligatoire pour ne pas avoir de dépassement
long timer = 0;

/**
 * Fonction de configuration initiale du programme
 */
void setup()
{
  // démarrage de la liaison série
  Serial.begin(38400);
  // initialisation des servomoteurs
  servG.attach(servGPin);
  servD.attach(servDPin);
  // attach the odometer interrupt
  attachInterrupt(odoGPin, tickG, CHANGE);
  attachInterrupt(odoDPin, tickD, CHANGE);
}

/**
 * Fonction appelée en boucle
 */
void loop()
{
  // lorsqu'on atteint 100, arrêter le test
  if (vitesse > 100)
  {
    servG.detach();
    servD.detach();
    return;
  }
  if (timer == 0) {
    // passage à la vitesse suivante
    vitesse++;
    // donner la nouvelle consigne de vitesse
    servG.write(vitesse);
    servD.write(vitesse);
    // remettre à zéro les deux compteurs d'odométrie
    counterG = 0;
    counterD = 0;
    // se remettre à compter le temps 
    timer = millis();      
  }
  // toutes les 5 secondes, changer de vitesse 
  // et envoyer les valeurs sur la liaison série
  long diff = millis() - timer;
  if (diff > 5000.0) {
    // demander le passage à la mesure suivante
    timer = 0;
    // envoyer les valeurs séparées par une tabulation (pour Excel)
    Serial.print(vitesse);
    Serial.print("\t");
    Serial.print(counterG);
    Serial.print("\t");
    Serial.print(counterD);
    Serial.println();
  }
}

// incrément lors d'une interruption pour roue gauche
void tickG()
{  
  counterG++;
}

// incrément lors d'une interruption pour roue droite
void tickD()
{
  counterD++; 
}





