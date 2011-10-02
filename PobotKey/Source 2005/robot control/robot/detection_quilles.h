#define NOT_QUILLE 3
#define PILE_QUILLES 2
#define QUILLE_DEBOUT 1
#define QUILLE_COUCHEE 0

// 640x480
// ratio min poir quille couchée, pour ne pas avoir de haut de quille debout detecté comme quille couchée
//#define QUILLE_XY_RATIO_MIN_COUCHEE 0.15
// seuils X/Y
//#define QUILLE_XY_RATIO_MIN_DEBOUT 1.4
//#define QUILLE_XY_RATIO_MAX_DEBOUT 1.8
// les ratios acceptables pour une quille debout
//#define QUILLE_SURFACE_Y_RATIO_MIN 25.0 
//#define QUILLE_SURFACE_Y_RATIO_MAX 260.0 
// la surface minimale d'une quille (même couchée), sinon c'est une tache !
//#define QUILLE_SURFACE_MIN 1500 

// ratio min pour quille couchée, pour ne pas avoir de haut de quille debout detecté comme quille couchée
#define QUILLE_XY_RATIO_MIN_COUCHEE 0.15
// seuils X/Y
// si trop de quilles debout détectées couchées on peut passer à 1.40 (voire 1.35 ??)
//  mais on aura des quilles couchées dans l'axe qui vont être vues comme debout
#define QUILLE_XY_RATIO_MIN_DEBOUT 1.45
#define QUILLE_XY_RATIO_MAX_DEBOUT 2.0
// les ratios acceptables pour une quille debout ATTENTION : max < min de pile !!!
#define QUILLE_SURFACE_Y_RATIO_MIN 13.5 
#define QUILLE_SURFACE_Y_RATIO_MAX 150.0 
// la surface minimale d'une quille (même couchée), sinon c'est une tache !
#define QUILLE_SURFACE_MIN 500
// ratio min max d'une pile
#define PILE_XY_RATIO_MIN 0.3
// simulation #define PILE_XY_RATIO_MAX 0.8
#define PILE_XY_RATIO_MAX 0.95
// surface min/max d'une pile
// simulation #define PILE_SURFACE_Y_RATIO_MIN 200.0
// attention : min > max de pile !
#define PILE_SURFACE_Y_RATIO_MIN 155.0
#define PILE_SURFACE_Y_RATIO_MAX 1000.0

/* returns one of : NOT_QUILLE, PILE_QUILLES, QUILLE_DEBOUT, QUILLE_COUCHEE */
int detectDebout(int x1, int x2, int y1, int y2, int surface);
