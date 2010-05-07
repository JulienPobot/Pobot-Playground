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

#define VITESSE_MIN  5
#define VITESSE_MAX  30

int vitesse = VITESSE_MAX;

float vitesseG = 0;
float vitesseD = 0;

int erreur = 0;
int errSum = 0;
int errDif = 0;
int lastErreur = 0;

int correction = 0;

static int kP=5;
static int kI=1;
static int kD=0;

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
  attachInterrupt(odoGPin, tickG, RISING);
  attachInterrupt(odoDPin, tickD, RISING);

  // utilisation du timer 2 pour gérer l'asservissement
  MsTimer2::set(50, update); // 500ms period
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

  // l'erreur
  erreur = counterD-counterG;
  correction = computePID(erreur);

  //
  vitesseG = vitesse + correction;
  vitesseD = vitesse - correction;

  servG.write(VG(vitesseG)); 
  servD.write(VD(vitesseD)); 

  /*
  if (counterG < counterD) {
   vitesseD = vitesse + correction;
   // vitesseD = vitesse/((counterD-counterG)/2);
   servD.write(VD(vitesseD)); 
   }
   
   if (counterD < counterG) {
   vitesseG = vitesse + correction;
   //vitesseG = vitesse/((counterG-counterD)/2);
   servG.write(VG(vitesseG)); 
   }
   */



}

int computePID(int err)
{
  int P,I,D;

  errSum += err;                 //Somme les erreurs depuis le début 

  errDif = err - lastErreur;      //Calcule la variation de l'erreur
  lastErreur = err;

  P = err * kP;                  //Proportionnelle
  I = errSum * kI;                 //Intégrale
  D = errDif * kD;                 //Dérivée

  return P + I + D;                //Le résultat est la somme des trois
  //composantes calculées précédemment

}

/**
 * Fonction appelée en boucle
 */
void loop()
{
  // lorsqu'on atteint 10, arrêter le test
  if (vitesse > VITESSE_MAX)
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
    Serial.print(counterD);
    Serial.print("\t");
    Serial.print(vitesseG);
    Serial.print("\t");
    Serial.print(vitesseD);
    Serial.println();
    // ajout de la génération d'un son pour "entendre" la vitesse
    //tone(4,abs(vitesseG)*10,50);
    delay(200);    
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












