#include "WProgram.h"
#include <Servo.h>

#define INCR  1

// on d\u00e9clare le servomoteur comme une variable typ\u00e9e
void setup();
void loop();
Servo servo1;

// l'angle actuel du servomoteur
int angle = 0;
// le sens actuel du servomoteur
int sens = 0;

/**
 * Initialisation du programme
 */
void setup()
{
  servo1.attach(14); // le servomoteur est sur ANALOG 0
  servo1.setMaximumPulse(2200); // 2,2 ms pour la valeur maximum
}

/**
 * Boucle infinie qui traite les consignes re\u00e7ues de l'utilisateur
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
    // si on est arriv\u00e9 \u00e0 une extremit\u00e9
    if (angle == 0)
    {
      sens = 0;
    } 
    if (angle == 180) {
      sens = 1;
    }
    delay(10);
    servo1.write(angle);
    Servo::refresh();
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

