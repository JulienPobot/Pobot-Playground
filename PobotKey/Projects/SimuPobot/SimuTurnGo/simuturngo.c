#include <stdio.h>

#include <string.h>
#include <math.h>

#include "robot_controller.h"
#include "move.h"


void _export StartMatch (
	int run_num,					// numéro du run dans le batch (>= 1)
	int match_num,				// numéro du match dans le run (>= 1)
  char *params)					// paramètres éventuels
{
 	printf("\n");
   printf("=== Start ===\r\n");

	indexOrdre = 0;
	flag_ordre_suivant = 0;
   memcpy(&ordreCourant,&strOrdres[0],sizeof(strOrdres[0]));
   //ordreCourant = strOrdres[0];
   ordreFinMatch.type = END;

}
          
/**
 *
 */
void _export cdecl Progress (
  SENSOR_COUNT sensor_count,					// nombre de capteurs (lg du vecteur suivant)
  SENSOR_VALUES_ARRAY sensor_values,  // vecteur des valeurs de capteurs
  ROBOT_POSITION_PTR position,				// pointeur sur la position (X,Y) du robot
  ROBOT_HEADING heading,							// cap du robot
  MOTOR_SPEEDS_PTR motor_speeds				// pointeur sur les vitesses moteur à retourner
  )
{
	robotT = heading;
	robotX = position->X*10;
   robotY = position->Y*10;

   traite_ordre();
	detecte_fin_ordre();

	motor_speeds->speed_left = consigne_vitesse_gauche/250;
   motor_speeds->speed_right = consigne_vitesse_droite/250;

}

char* _export cdecl About (void)
{
 	return "Contrôleur Turn & Go - Robot 2008";
}



/**
 * Utilitaire de correction d'un angle pour le ramener à l'intérieur de -180:180
 */
double corrigeTour(double angle)
{
	// trop grand

	if (angle > 180) {
		return angle - 360;
	}
	// trop petit
	if (angle < -180) {
		return angle + 360;
	}
	// normal
	return angle;
}

int signe(double valeur)
{
	if (valeur >= 0)
	{
		return 1;
	} else {
		return -1;
	}
}

/**
 *
 */
void calculs_logique_floue(void)
{

}


/**
 * Calculs de commande des moteurs
 */ 
void calculs_turn_go(void)
{
	// facteurs de multiplication (1,-1) 
	double erreur;
		
	switch (ordreCourant.type)
	{
		case MOVE_CAP:
			// vérifier le cap
			erreur = corrigeTour(ordreCourant.cap - robotT);
			// en-dessous de 10°
			if (erreur > 20 || erreur < -20) {
				consigne_vitesse_gauche = signe(erreur)*-ordreCourant.vitesse;
				consigne_vitesse_droite = signe(erreur)*ordreCourant.vitesse;		
			} else {
				// erreur inférieure à 10°
				consigne_vitesse_gauche = -ordreCourant.vitesse * erreur / 50;
				consigne_vitesse_droite = ordreCourant.vitesse * erreur / 50;			
			}	
			break;
			
		case MOVE_X:
			//
			erreur = signe(ordreCourant.vitesse)*(ordreCourant.valeurX - robotX);
			//
			if (erreur > 40 || erreur < -40) {
				//
				if (robotT < 90 && robotT > -90) {
					consigne_vitesse_gauche = signe(erreur)*ordreCourant.vitesse;
					consigne_vitesse_droite = signe(erreur)*ordreCourant.vitesse;
				} else {
					consigne_vitesse_gauche = signe(erreur)*-ordreCourant.vitesse;
					consigne_vitesse_droite = signe(erreur)*-ordreCourant.vitesse;				
				}
			} else {
				if (robotT < 90 && robotT > -90) {
					consigne_vitesse_gauche = erreur*ordreCourant.vitesse/80;
					consigne_vitesse_droite = erreur*ordreCourant.vitesse/80;
				} else {
					consigne_vitesse_gauche = -erreur*ordreCourant.vitesse/80;
					consigne_vitesse_droite = -erreur*ordreCourant.vitesse/80;		
				}
			}			
			break;
			
		case MOVE_Y:
			//
			erreur = signe(ordreCourant.vitesse)*(ordreCourant.valeurY - robotY); 
			//
			if (erreur > 40 || erreur < -40) {
				//
				if (robotT >= 0) {
					consigne_vitesse_gauche = signe(erreur)*ordreCourant.vitesse;
					consigne_vitesse_droite = signe(erreur)*ordreCourant.vitesse;
				} else {
					consigne_vitesse_gauche = signe(erreur)*-ordreCourant.vitesse;
					consigne_vitesse_droite = signe(erreur)*-ordreCourant.vitesse;				
				}
			} else {
				if (robotT >= 0) {
					consigne_vitesse_gauche = erreur*ordreCourant.vitesse/80;
					consigne_vitesse_droite = erreur*ordreCourant.vitesse/80;
				} else {
					consigne_vitesse_gauche = -erreur*ordreCourant.vitesse/80;
					consigne_vitesse_droite = -erreur*ordreCourant.vitesse/80;		
				}
			}
			break;
	}     
}

/**
 *
 */
void detecte_fin_ordre(void)
{
	// la valeur de différence entre la position attendue et la position relevée
	double diff;

	// traitement des différents ordres possibles
	// certains sont immédiats et vont retourner tout de suite (passer au suivant)
	// d'autres nécessitent d'avoir atteint une certaine cible (position, validation..)
	switch (ordreCourant.type) 
	{
      case START:
      	flag_ordre_suivant = 1;
      	break;
		case END:
			// ne rien faire, l'action END est toujours la dernière
			break;
		case MOVE_CAP:
			diff = robotT-ordreCourant.cap;
			if ( diff < SEUIL_CAP && diff > -SEUIL_CAP)
			{
				flag_ordre_suivant = 1;
			}
			break;
		case MOVE_X:
			diff = robotX-ordreCourant.valeurX;
			
			if (diff < 10 && diff > -10) 
			{
         	printf("But X atteint : %d %d %d\r\n",robotT,robotX,robotY);
				flag_ordre_suivant = 1;
			}
			break;
		case MOVE_Y:
			if (abs(robotY-ordreCourant.valeurY) < 3)
			{        
         	printf("But Y atteint : %d %d %d\r\n",robotT,robotX,robotY);
				flag_ordre_suivant = 1;
			}
			break;			
		case MOVE_POS:
			if (abs(robotX-ordreCourant.valeurX) < 3 
				&& fabs(robotY-ordreCourant.valeurY) < 3) 
			{
				flag_ordre_suivant = 1;
			}
			break;
      default:
      	printf(" default ");
      	flag_ordre_suivant = 1;		
      	break;
	}
	
	if (flag_ordre_suivant == 1)
	{
		ordreCourant = strOrdres[++indexOrdre];
      //memcpy(&ordreCourant,&strOrdres[++indexOrdre],sizeof(ordreCourant));
		printf("\r\n");
		printf("Ordre suivant: %d %d %d\r\n",indexOrdre,ordreCourant.type,ordreCourant.cap);
		printf("\r\n");
		flag_ordre_suivant = 0;
	}
}

/**
 *
 */
void traite_ordre(void)
{	
	// exécution de l'ordre
	switch (ordreCourant.type) {
		case START:
		case WAIT:
		case PINCE_O:
		case PINCE_F:
      	break;
		case MOVE_X:
		case MOVE_Y:
		case MOVE_CAP:
			calculs_turn_go();
			break;				
		case MOVE_POS:
			calculs_logique_floue();
			break;
		case END:
			consigne_vitesse_gauche = 0;
			consigne_vitesse_droite = 0;
			break;
	}

}


