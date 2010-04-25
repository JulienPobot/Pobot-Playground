/**
 * Outil de test de l'asservissement du Pobot Easy
 * 
 * Le robot doit aller tout droit en utilisant ses odomètres.
 *
 * @author Julien pour Pobot (julien@pobot.org)
 * @date 25-04-2010
 *
 */

// utilisation de la bibliothèque "Servo" pour Arduino
// fonctionne avec toute entrée numérique ("Digital")
#include <Servo.h>
#include <MsTimer2.h>

// ajout de deux macros qui calculent la consigne en adaptant 
// - la consigne constatée pour vitesse de rotation minimale (si possible nulle)
// - le signe qui rétablira l'inversion des deux moteurs face à face
#define VG(val)  (val+72)
#define VD(val)  ((val<0)?76-val:77-val)

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

int consigneG = 0;
int consigneD = 0;

// la vitesse en cours de test

#define VITESSE_MIN  -10
#define VITESSE_MAX  10

int vitesse = VITESSE_MIN;

int vitesseG = 0;
int vitesseD = 0;

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

  // utilisation du timer 2 pour gérer l'asservissement
  MsTimer2::set(500, update); // 500ms period
  MsTimer2::start();

}

/**
 * Asservissement
 */
void update(void)
{
  servG.write(VG(vitesse));   
  servD.write(VD(vitesse));

  if (counterG > consigneG)
  {
    servG.detach(); 
  } 

  if (counterD > consigneD)
  {
    servD.detach(); 
  } 
  
  if (counterG < counterD) {
    servD.write(VD(vitesse/(counterG-counterD))); 
  }
  
  if (counterD < counterG) {
    servG.write(VG(vitesse/(counterD-counterG))); 
  }

  // vérification des vitesses pour trace
  vitesseG = servG.read();
  vitesseD = servD.read();

}

/**
 * Fonction appelée en boucle
 */
void loop()
{
  // lorsqu'on atteint 10, arrêter le test
  if (vitesse > 10)
  {
    servG.detach();
    servD.detach();
    return;
  }

  // mettre en marche les moteurs
  servG.attach(servGPin);
  servD.attach(servDPin);

  // remettre à zéro les deux compteurs d'odométrie
  counterG = 0;
  counterD = 0;

  // donner des consignes
  consigneG = 400;
  consigneD = 400;

  // attendre que les deux consignes soient atteintes
  while (counterG < consigneG && counterD < consigneD)
  {
    // afficher où on en est
    Serial.print(vitesse);
    Serial.print("\t");
    Serial.print(counterG);
    Serial.print("\t");
    Serial.print(consigneG);
    Serial.print("\t");
    Serial.print(vitesseG);
    Serial.print("\t");
    Serial.print(counterD);
    Serial.print("\t");
    Serial.print(consigneD);
    Serial.print("\t");
    Serial.print(vitesseD);
    Serial.println();
    delay(300);    
  }  
  // passage à la vitesse suivante
  servG.detach();
  servD.detach();
  delay(1000);
  vitesse++;

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










