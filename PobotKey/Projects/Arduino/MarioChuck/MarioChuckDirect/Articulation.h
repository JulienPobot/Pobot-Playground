
#ifndef MARIO_CHUCK_ARTICULATION
#define MARIO_CHUCK_ARTICULATION

#include "WProgram.h"
#include <MegaServo.h>

class Articulation
{

private:

  MegaServo servo;

  // on ne stocke que les valeurs int�ressantes

  // l'angle du servo (pourrait �tre remplac� par un retour d'info r�el
  int svalue;

  // les minimums
  int smin;
  int pmin;
  int cmin;  
  int cmax;
  
  // si on applique un filtre parabolique
  boolean square;

  // le coeff pour passer de la position r�elle � s
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
