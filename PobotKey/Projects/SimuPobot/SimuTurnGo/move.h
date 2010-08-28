#define SEUIL_CAP		2

float robotT,robotX,robotY;

double consigne_vitesse_gauche, consigne_vitesse_droite;

void calculs_turn_go(void);
void calculs_logique_floue(void);
void detecte_fin_ordre(void);
void traite_ordre(void);

/** Définitions & variables pour le turn&go **/
typedef struct
{
	int type;
	float cap;
	double valeurX;
	double valeurY;
	double vitesse;	
} ACTION_DEF, *ACTION_DEF_PTR;

enum ORDERS_TYPE
{
	END = 1,
	START,
	WAIT,
	PINCE_O,
	PINCE_F,
	MOVE_X,
	MOVE_Y,
	MOVE_CAP,
	MOVE_POS,
	TURB_ON,
	TURB_POS
};

volatile int indexOrdre;

// drapeau indiquant qu'il faut passer à l'exécution de l'ordre suivant
volatile int flag_ordre_suivant;

/*
ACTION_DEF PROGMEM strOrdres[]=
{
	{START,0,0,0,0},
	{PINCE_F,90,0,0,0},
	{MOVE_X,0,600,0,40},
	{MOVE_CAP,90,0,0,40},
	{MOVE_Y,90,0,600,40},
	{MOVE_CAP,-90,0,0,40},
	{MOVE_X,-90,-600,0,40},
	{MOVE_CAP,180,0,0,40},
	{MOVE_X,180,-160,0,40},
	{MOVE_CAP,-90,0,0,40},
	{MOVE_Y,-90,0,-260,40},
	{MOVE_CAP,0,0,0,40},
	{MOVE_X,0,360,0,40},
	{PINCE_O,0,0,0,0},
	{END,0,0,0,0}
};
*/

// test d'extraction

ACTION_DEF strOrdres[]=
{
	{START,0,0,0,0},
	{PINCE_F,90,0,0,0},
	{MOVE_X,0,-300,0,-20},
	{MOVE_CAP,-90,0,0,50},
	{MOVE_Y,-90,0,100,30},
	{PINCE_O,0,0,0,0},
	{END,0,0,0,0}
};



// test du déplacement en X
/*
ACTION_DEF strOrdres[]=
{
	{START,0,0,0,0},
	{MOVE_X,0,60,0,60},
	{WAIT,5,0,0,0},
	{MOVE_X,0,30,0,60},
	{WAIT,5,0,0,0},
	{MOVE_X,0,-30,0,60},
	{WAIT,5,0,0,0},
	{MOVE_X,0,25,0,60},
	{WAIT,5,0,0,0},
	{MOVE_X,0,40,0,60},
	{WAIT,5,0,0,0},
	{MOVE_X,0,-10,0,60},
	{WAIT,5,0,0,0},
	{MOVE_X,0,0,0,60},
	{END,0,0,0,0}
};
*/


// test des rotations
/*
ACTION_DEF PROGMEM strOrdres[]=
{
	{START,0,0,0,0},
	{MOVE_CAP,90,0,0,60},
	{WAIT,5,0,0,0},
	{MOVE_CAP,-90,0,0,60},
	{WAIT,5,0,0,0},
	{MOVE_CAP,0,0,0,60},
	{WAIT,5,0,0,0},
	{MOVE_CAP,-135,0,0,60},
	{WAIT,5,0,0,0},
	{MOVE_CAP,135,0,0,60},
	{WAIT,5,0,0,0},
	{MOVE_CAP,0,0,0,60},
	{WAIT,5,0,0,0},
	{MOVE_CAP,-180,0,0,60},
	{END,0,0,0,0}
};
*/

volatile ACTION_DEF ordreCourant;

volatile ACTION_DEF ordreFinMatch;

// Test fabrice bleu (droite)
/*
ACTION_DEF strOrdres[]=
{
	{START,-45,1295,-669,0},
	{MOVE_CAP,-90,900,-853,60},
	{MOVE_CAP,-90,705,-853,60},  // prise des balles bleu proches à x=700
	{TURB_ON,0,0,0,1},           // turbine aspire 
	{TURB_POS,180,0,0,0},        // turbine en position haute
	{MOVE_CAP,-90,205,-853,60},  // prise des balles bleu proches à x=200
	{PINCE_F,0,0,0,0},           // fermeture pince
	{MOVE_CAP,90,-1100,800,60},  // position de tir
	{TURB_ON,0,0,0,-1},          // turbine souffle
	{WAIT,5,0,0,0},              // attente fin du tir
	{TURB_POS,0,0,0,0},          // turbine en position basse
	{TURB_ON,0,0,0,1},           // turbine aspire 
	{TURB_POS,180,0,0,0},        // turbine en position haute
	{PINCE_O,0,0,0,0},           // ouverture pince
	{TURB_ON,0,0,0,-1},          // turbine souffle
	{TURB_POS,0,0,0,0},          // turbine en position basse
	{WAIT,5,0,0,0},              // attente fin du tir
	{MOVE_CAP,180,800,-500,60},  // traversé du terrain en diagonale pour distributeur
	{MOVE_CAP,180,800,-1045,60}, // docking
	{PINCE_F,0,0,0,0},           // fermeture pince
	{MOVE_POS,180,800,-1035,-60},// marche arrière pour sortir un balle
	{TURB_ON,0,0,0,1},           // turbine aspire 
	{TURB_POS,180,0,0,0},        // turbine en position haute
	{PINCE_O,0,0,0,0},           // ouverture pince
	{MOVE_CAP,180,800,-1045,60}, // docking
	{PINCE_F,0,0,0,0},           // fermeture pince
	{MOVE_POS,180,800,-1035,-60},// marche arrière pour sortir un balle
	{MOVE_CAP,90,-1100,800,-60}, // position de tir
	{TURB_ON,0,0,0,-1},          // turbine souffle
	{WAIT,5,0,0,0},              // attente fin du tir
	{TURB_POS,0,0,0,0},          // turbine en position basse
	{TURB_ON,0,0,0,1},           // turbine aspire 
	{TURB_POS,180,0,0,0},        // turbine en position haute
	{PINCE_O,0,0,0,0},           // ouverture pince
	{TURB_ON,0,0,0,-1},          // turbine souffle
	{TURB_POS,0,0,0,0},          // turbine en position basse
	{WAIT,5,0,0,0},              // attente fin du tir
	{MOVE_CAP,180,800,-500,60},  // traversé du terrain en diagonale pour distributeur
	{MOVE_CAP,180,800,-1045,60}, // docking
	{PINCE_F,0,0,0,0},           // fermeture pince
	{MOVE_POS,180,800,-1035,-60},// marche arrière pour sortir un balle
	{MOVE_CAP,90,-1100,800,-60}, // position de tir
	{TURB_ON,0,0,0,-1},          // turbine souffle
	{WAIT,5,0,0,0},              // attente fin du tir
	{TURB_POS,0,0,0,0},          // turbine en position basse
	{TURB_ON,0,0,0,1},           // turbine aspire 
	{TURB_POS,180,0,0,0},        // turbine en position haute
	{PINCE_O,0,0,0,0},           // ouverture pince
	{TURB_ON,0,0,0,-1},          // turbine souffle
	{TURB_POS,0,0,0,0},          // turbine en position basse
	{WAIT,5,0,0,0},              // attente fin du tir
	{END,0,0,0,0}
};
*/
