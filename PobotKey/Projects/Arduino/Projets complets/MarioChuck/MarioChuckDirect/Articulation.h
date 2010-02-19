
#ifndef MARIO_CHUCK_ARTICULATION
#define MARIO_CHUCK_ARTICULATION

#include "WProgram.h"
#include <MegaServo.h>

class Articulation
{

private:

  MegaServo servo;

  // on ne stocke que les valeurs intéressantes

  // l'angle du servo (pourrait être remplacé par un retour d'info réel
  int svalue;

  // les minimums
  int smin;
  int pmin;
  int cmin;  
  int cmax;
  
  // si on applique un filtre parabolique
  boolean square;

  // le coeff pour passer de la position réelle à s
  float pcoeff;

  // le coeff pour passer du nunchuck au servo
  float ccoeff;

public:

  void init(int pin, int valsmin, int valsmax, int valpmin, int valpmax, int valcmin, int valcmax, boolean square);

  /**
   * Update d'une valeur avec le nunchuck
   * @param value integer : nouvelle valeur entre cmin et cmax
   */
  void updateChuck(int value);

  void updatePosition(int value);

  int readPosition();

};

#endif
