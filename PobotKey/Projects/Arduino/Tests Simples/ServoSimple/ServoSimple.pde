#include <Servo.h>

#define INCR  1

// on déclare le servomoteur comme une variable type
Servo servo;

// l'angle actuel du servomoteur
int angle = 0;
// le sens actuel du servomoteur
int sens = 0;

/**
 * Initialisation du programme
 */
void setup()
{
  servo.attach(9);
}

/**
 * Boucle infinie qui traite les consignes re�ues de l'utilisateur
 * et le transforme en rotation du servomoteur
 */
void loop()
{
  while (1)
  {
    // faire tourner le servomoteur
    if (sens == 0)
    {
      angle += INCR;
    } 
    else {
      angle -= INCR;
    }
    // si on est arriv� � une extremit�
    if (angle == 0)
    {
      sens = 0;
    } 
    if (angle == 180) {
      sens = 1;
    }
    delay(10);
    servo.write(angle);
  }
}
