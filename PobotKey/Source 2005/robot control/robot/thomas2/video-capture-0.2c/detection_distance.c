#include "detection_distance.h"

/* TC : detection de la position de l'objet en fonction de son ordonnée la plus basse */ 
int detectDistance(int y2, int xBary) {

	float cst_dist_a;
	float cst_dist_b;
	float xCorrection = (xBary -160) * (240 - y2) * X_CORRECTION;

	if (y2 > Y2_SWITCH_TAB1_TAB2) {
		cst_dist_a = CST_DIST_A_TAB1;
		cst_dist_b = CST_DIST_B_TAB1;
	}
	else {
		cst_dist_a = CST_DIST_A_TAB2;
		cst_dist_b = CST_DIST_B_TAB2;
	}
	       	
	//return (cst_dist_a * (float) y2) / (1000 + (cst_dist_b * (float) y2));
	return (cst_dist_a * (float) y2) / (1000 + (cst_dist_b * (float) y2)) + xCorrection;
}


