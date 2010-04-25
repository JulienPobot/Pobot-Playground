/**
 * Calibrage de l'odométrie d'un Pobot Easy
 *
 * On donne un nombre d'incréments à faire
 * et on affiche le temps mis pour le faire
 *
 * Cela permet aussi de vérifier si l'angle parcouru est le même
 *
 * @author Julien Holtzer - julien.holtzer@gmail.com
 * @date 25-04-2010
 *
 */

#include <Servo.h>

// servo object representing the motor
Servo serv;

// connexion du servo (digital)
int servPin = 9;

// connexion de l'odométrie (0 = digital 2, 1 = digital 3)
int odoPin = 0;

// vitesse par défaut
int valeur = 90;

// compteur des incréments
int counter = 0;

// timer de mesure du temps passé pour faire n incréments
int timer = 0;

void setup()
{
  // liaison série
  Serial.begin(38400);
  
  // interrupt pour le compteur  
  attachInterrupt(odoPin, tick, CHANGE);
  
}

void tick()
{
 counter++; 
}

void loop()
{
  // si on a reçu une nouvelle consigne de vitesse
  if (Serial.available())
  {
    // lecture de la valeur
    valeur = Serial.read();  
  }
  
  // nouvelle boucle : test de la vitesse donnée
  
  // 1) arrêter le servo   
  serv.detach();
  delay(300);
  // 2) réinitialiser le compteur 
  counter = 0;
  timer = millis();
  // 3) redémarrer le servo à  la vitesse souhaitée
  serv.attach(servPin);
  serv.write(valeur);
  // 4) attendre le nombre d'incréments souhaités
  while (counter < 32)
  {
    delay(0);
  }
  // 5) arrêter le servo et mesurer le temps passé
  serv.detach();
  timer = millis() - timer;
  // 6) envoyer les valeurs
  Serial.print(valeur);
  Serial.print(",");
  Serial.print(counter);
  Serial.print(",");
  Serial.print(timer);
  Serial.println();
  
  // attendre 
  delay(500); 
  
}

