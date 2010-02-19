
#ifndef MARIO_CHUCK_MARIONNETTE
#define MARIO_CHUCK_MARIONNETTE

#include "WProgram.h"
#include "Articulation.h"
#include "WiiChuck.h"

// Configuration à changer
#define PIN_PAN  9
#define SMIN_PAN  10
#define SMAX_PAN  170
#define PMIN_PAN  -60
#define PMAX_PAN  60
#define CMIN_PAN  -8100
#define CMAX_PAN  8100
#define SQUARE_PAN  true

#define PIN_TILT  10
#define SMIN_TILT  30
#define SMAX_TILT  150
#define PMIN_TILT  -45
#define PMAX_TILT  45
#define CMIN_TILT  2025
#define CMAX_TILT  18225
#define SQUARE_TILT  true

#define PIN_EYESH  11
#define SMIN_EYESH  30
#define SMAX_EYESH  150
#define PMIN_EYESH  -45
#define PMAX_EYESH  45
#define CMIN_EYESH  -60
#define CMAX_EYESH  60
#define SQUARE_EYESH  false

#define PIN_EYESV  12
#define SMIN_EYESV  30
#define SMAX_EYESV  150
#define PMIN_EYESV  -45
#define PMAX_EYESV  45
#define CMIN_EYESV  -60
#define CMAX_EYESV  60
#define SQUARE_EYESV  false

#define PIN_MOUTH  8
#define SMIN_MOUTH  30
#define SMAX_MOUTH  150
#define PMIN_MOUTH  0
#define PMAX_MOUTH  90
#define CMIN_MOUTH  0
#define CMAX_MOUTH  1
#define SQUARE_MOUTH  false

//
class Marionnette
{

private:
  // les articulations du cou
  Articulation aNeckP;
  Articulation aNeckT;
  
  // les servos des yeux
  Articulation aEyesV; // vertical, 0 = bas / 180 = haut
  Articulation aEyesH; // horizontal, 0 = gauche / 180 = droite
  
  // le servo de la bouche
  Articulation aMouth;

public:

  void init();
    
  void updateChuck(WiiChuck chuck);
  
  void readFigure();
  
  void writeFigure();
  
  
};

#endif

