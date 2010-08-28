
#include <math.h>
#include <stdio.h>

#include "fuzzy.h"
#include "robot_controller.h"

#define NB_TARGET	13

#define PI	3.141595358


// définition des points
// départ : -85, -120
// poteau bleu : 0, -120
// poteau rouge : 0, 120
// poteau blanc B : -85, -70
// poteau blanc R : -85, 70
// zone de marquage : 85, 120

// stratégie 1 :
// départ / poteau bleu / poteau blanc B / zone de marquage
//float X[NB_TARGET] = { 0, 0, -65,-85, 85, 0.0 };
//float Y[NB_TARGET] = { -100, -120, -70, -70, 120, 0.0 };

// stratégie 2 :
// départ / poteau blanc / poteau bleu / poteau blanc / zone de marquage / idem
//float X[NB_TARGET] = { -65,-85, 0, 0, -65,-85, 85};
//float Y[NB_TARGET] = { -70, -70, -100, -120, -70, -70, 120};

// stratégie 3 :

float X[NB_TARGET] = { 0, 80, -80, 40, -65,-85, 0, 0, -65,-85, 85, 50, 0};
float Y[NB_TARGET] = { 0, 130, -130, -60, -70, -70, -100, -120, -70, -70, 120, 50, 0};


//float X[NB_TARGET] = { 50, 50, -50, -50};
//float Y[NB_TARGET] = { -50, 50, 50, -50};
float T[NB_TARGET] = { 0, 90, 90, 0};



// entrée du générateur flou
float inputs[2] = {0, 0};

// sortie du générateur flou
float	crisp_outputs[2] = { 0, 0};

int cible_index = 0;

// la distance à parcourir jusqu'au prochain point
float d = 0;

// l'angle entre la position du robot et le prochain point
float a = 0;

// distance en entrée du moteur flou
float distance = 0;

// angle en entrée du moteur flou
float angle;

// distance au mur en entrée du moteur flou
float dist_mur = 500;

// angle d'attaque du mur en entrée du moteur flou
float angle_menace = 0;

// tick pour limiter le nombre de traces
int tr_count = 0;

float out_vitesse = 0;  
float out_direction = 0;

/**
 *
 */
void trace(char* label, float distance, float angle)
{
	if (tr_count == 50) {
      printf("%s\tdist : %f\t\tangle : %f\r\n",label,distance,angle);
   }
}

void _export StartMatch (
	int run_num,					// numéro du run dans le batch (>= 1)
	int match_num,				// numéro du match dans le run (>= 1)
  char *params)					// paramètres éventuels
{
 	printf("\n");
 	printf("\n");
   printf("=== Start ===\r\n");

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
	// calcul de la distance séparant le robot de sa cible
   float distX = position->X - X[cible_index];
   float distY = position->Y - Y[cible_index];
   int dist_mur3;
   int dist_mur2;
   int dist_mur1;
   int dist_mur4;
   int dist_min;
   int mur;
	// pour les traces
   tr_count--;
  	if(tr_count <= 0) {
   	tr_count = 50;
   }
   d = sqrt(distX*distX+distY*distY);

	// article de Frédéric :
   // IF (x-x1)=0 THEN µgoal =90
   //	ELSE IF ((x-x1)>0 THEN µgoal = arctan((y1-y)/(x1-x)
   // ELSE µgoal = 180 + arctan((y1-y)/(x1-x)

	if (distX == 0) {
   	a = 90;
   } else if (distX < 0) {
   	a = atan(distY/distX)/PI*180;
   } else {
   	a = 180+atan(distY/distX)/PI*180;
      
   }

   distance = d *10.0;
	angle = a - heading;

   if (angle > 180) {
   	angle -= 360;
   }
   if (angle < -180) {
    	angle += 360;
   }

   /* initialisation */
   
   inputs[0] = distance;
   inputs[1] = angle;
   inputs[2] = dist_mur;

   /* Calcul de la distance au mur */

   dist_mur3 = 150 - position->Y;
   dist_mur2 = 105 - position->X;
   dist_mur1 = abs(position->Y - 150);
   dist_mur4 = abs(position->X - 105);

   // on calcule le minimum de ces quatres distances
   dist_min = dist_mur1;
   mur = 1;
   if (dist_min > dist_mur2) {
     dist_min = dist_mur2;
     mur = 2;
   }
   if (dist_min > dist_mur3) {
     dist_min = dist_mur3;
     mur = 3;
   }
   if (dist_min > dist_mur4) {
     dist_min = dist_mur4;
     mur = 4;
   }

   //  choix d'utiliser le mur ou pas
   if (mur == 1 && heading > -135 && heading < -45)
   {
        inputs[2] = dist_min;
        angle_menace = - heading - 90;
   }

   //  choix d'utiliser le mur ou pas
   if (mur == 2 && heading > -45 && heading < 45)
   {
        inputs[2] = dist_min;
        angle_menace = - heading;
   }

   //  choix d'utiliser le mur ou pas
   if (mur == 3 && heading > 45 && heading < 135)
   {
        inputs[2] = dist_min;
        angle_menace = heading + 90;
   }        

   //  choix d'utiliser le mur ou pas
   if (mur == 3 && heading > 135 || heading < -135)
   {
        inputs[2] = dist_min;
        angle_menace = 180 + heading;
   }

   if (angle_menace > 180) {
   	angle_menace -= 360;
   }
   if (angle_menace < -180) {
    	angle_menace += 360;
   }

   if (distance < 200.0) {
    	cible_index++;
      if (cible_index >= NB_TARGET) {
       	cible_index = 0;
      }
   }

   if (tr_count == 50) {
		//printf("[cible=%d]\t[dist=%f]\t[point=%f]\t[angle=%f]\r\n",cible_index,distance,a,angle);
      //printf("[mur=%d]\t[dist_mur=%04f]\t[angle_menace=%4f]\r\n",mur,inputs[2],angle_menace);
		//printf("[%f]\t[%f]\t[%f]\t[%f]\r\n",sensor_values[0],sensor_values[1],sensor_values[2],sensor_values[3]);
   }

   inputs[2] = inputs[2] * 10;
   inputs[3] = angle_menace;

   fuzzy_step(inputs, crisp_outputs);


   motor_speeds->speed_left = crisp_outputs[1] /250.1;
   motor_speeds->speed_right = crisp_outputs[0]/250.1;

}


char* _export cdecl About (void)
{
 	return "Contrôleur Coupe - Robot 2008";
}
