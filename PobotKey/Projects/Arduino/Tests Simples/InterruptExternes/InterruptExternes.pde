#include <Servo.h>

// les moteurs
Servo sGauche;
Servo sDroite;

// les compteurs
int compteurGauche = 0;
int compteurDroite = 0;

void setup()
{
  // les moteurs
  sGauche.attach(9); // patte 9
  sDroite.attach(10); // patte 10
  
  // les interruptions
  attachInterrupt(0,gauche,RISING); // patte 2
  attachInterrupt(1,droite,RISING); // patte 3
  
  // initialisation : on avance !
  sGauche.write(110);
  sDroite.write(70); // les moteurs sont inversés
}

void loop()
{
  // rien pour ce test 
}


void gauche()
{ 
  if (compteurGauche > 0) 
  {
     compteurGauche--;    
  } else {
     sGauche.write(90); // arrêt du moteur  
  }
}

void droite()
{
  if (compteurDroite > 0)
  {
     compteurDroite--;  
  } else {
     sDroite.write(90); // arrêt du moteur
  }
}
