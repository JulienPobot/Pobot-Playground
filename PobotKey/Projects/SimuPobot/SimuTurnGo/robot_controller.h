/*
	Interface des DLL controlleurs de robot  
*/

#ifndef __robot_controller_h__
#define __robot_controller_h__

typedef struct {
	float speed_left ;
  float speed_right ;
} MOTOR_SPEEDS, *MOTOR_SPEEDS_PTR ;

typedef float SENSOR_VALUE, SENSOR_VALUES_ARRAY[] ;

typedef unsigned int SENSOR_COUNT ;

typedef struct {
	float X ;
  float Y ;
} ROBOT_POSITION, *ROBOT_POSITION_PTR ;

typedef float ROBOT_HEADING ;

typedef int INIT_RETURN_CODE ;
#define INIT_OK 0

typedef int COLLISION_RETURN_CODE ;
#define COLLISION_CONTINUE 					0
#define COLLISION_STOP_SIMULATION 	1

typedef unsigned char BYTE_BOOL ;
#define FALSE	0
#define TRUE 1

// -----------------------------------------------------------------------------------
// Entry points
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// Initialisation de la DLL (facultatif)
//
// Appel�e juste apr�s le chargement de celle-ci.

INIT_RETURN_CODE _export cdecl Init (
	char *params	// param�tres facultatifs d'initialisation, transmis par le simulateur
  ) ;

// -----------------------------------------------------------------------------------
// Nettoyage final avant d�chargement de la DLL. (facultatif)
//
// Est appel� juste avant le d�chargement de la DLL.

void _export cdecl Cleanup (void) ;

// -----------------------------------------------------------------------------------
// R�cup�ration d'un texte d'information concernant la DLL (facultatif)
//
// Permet de retourner un court texte d'information sur la DLL.

char* _export cdecl About (void) ;

// -----------------------------------------------------------------------------------
// Progression de la simulation (OBLIGATOIRE)
//
// Appel� � chaque pas de temps de simulation et doit retourner les consignes de vitesse
// des moteurs dans la structure moter_speeds.

void _export cdecl Progress (
	SENSOR_COUNT sensor_count,					// nombre de capteurs (lg du vecteur suivant)
  SENSOR_VALUES_ARRAY sensor_values,  // vecteur des valeurs de capteurs
  ROBOT_POSITION_PTR position,				// pointeur sur la position (X,Y) du robot
  ROBOT_HEADING heading,							// cap du robot
  MOTOR_SPEEDS_PTR motor_speeds				// pointeur sur les vitesses moteur � retourner
  ) ;

// -----------------------------------------------------------------------------------
// D�tection de collision (facultatif)
//
// Est appel� � chaque d�tection de collision, fatale ou non. Le type et la
// "traversabilit�" de l'obstacle sont fournies et cette fonction doit retourner si
// la collision doit �tre consid�r�e comme fatale ou non. Attention cependant, si
// celle-ci avait �t� d�termin�e comme fatale par le simulateur, la valeur renvoy�e
// n'est pas prise en compte. En d'autres termes, une collision fatale ne peut pas �tre
// "d�fatalis�e", mais l'inverse est possible.

COLLISION_RETURN_CODE _export cdecl Collision (
  char *obstacle_type,			// type d'obstacle rencontr�
  int can_traverse					// 1 si obstacle pouvant �tre travers�, 0 sinon
	) ;

// -----------------------------------------------------------------------------------
// R�cup�ration des r�sultats en fin de run, sous forme d'une cha�ne de
// caract�res de format libre. (facultatif)
//
// Appel� � l'issue de chaque run, et permet de retourner des r�sultats d'�valuation
// par exemple. La cha�ne retourn�e est �crite dans le fichier r�sultat.

char* _export cdecl GetRunResult (void) ;

// -----------------------------------------------------------------------------------
// Signal de d�but de match. (facultatif)
//
// Appel� au d�but de chaque match.

void _export StartMatch (
	int run_num,					// num�ro du run dans le batch (>= 1)
	int match_num,				// num�ro du match dans le run (>= 1)
  char *params					// param�tres �ventuels
  ) ;

// -----------------------------------------------------------------------------------
// Signal de fin de match. (facultatif)
//
// Appel� � la fin de chaque match.

void _export MatchEnded (
	int coverage,					// taux de couverture du terrain
	BYTE_BOOL last_run,	 	// 1 si dernier run de la simulation
  BYTE_BOOL last_match	// 1 si dernier match du run
  ) ;

#endif
