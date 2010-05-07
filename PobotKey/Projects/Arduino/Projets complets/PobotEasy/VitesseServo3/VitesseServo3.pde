/**
 * Outil de test des odomètres : compte le temps entre 2 incréments
 * 
 *
 * @author Julien pour Pobot (julien@pobot.org)
 * @date 30-04-2010
 *
 */
 
 // test

// utilisation de la bibliothèque "Servo" pour Arduino
// fonctionne avec toute entrée numérique ("Digital")
#include <Servo.h>

// ajout de deux macros qui calculent la consigne en adaptant 
// - la consigne constatée pour vitesse de rotation minimale (si possible nulle)
// - le signe qui rétablira l'inversion des deux moteurs face à face
#define V(val)  (val+72)
// #define V(val)  ((val<0)?76-val:77-val)

// déclaration des deux objets qui représentent les servomoteurs
Servo serv;

// connections du servomoteur gauche
int servPin = 9;
int odoPin = 0; // 0 pour la patte Digital 2, 1 pour la patte Digital 3

// sauvegarde du dernier temps
long milli = 0;

// sauvegarde du dernier écart
long diff = 0;

// la vitesse en cours de test
int vitesse = 1;

// si on doit afficher la dernière valeur conne 
boolean affiche = false;

/**
 * Fonction de configuration initiale du programme
 */
void setup()
{
  // démarrage de la liaison série
  Serial.begin(38400);
  // initialisation des servomoteurs
  serv.attach(servPin);
  // EN UTILISANT LES MACROS
  serv.write(V(vitesse));
  // attach the odometer interrupt
  attachInterrupt(odoPin, tick, CHANGE);
}

/**
 * Fonction appelée en boucle
 */
void loop()
{
  if (affiche)
  {
    affiche = false;
    Serial.print(diff);
    Serial.print(",");
  }
}

// incrément lors d'une interruption pour roue gauche
void tick()
{   
  diff = millis()-milli;
  affiche = true;
  milli = millis();
}







