//#include <stdio.h>
#include "detection_ponts.h"

/* TC : detection de la position du pont */ 
int detectPonts(int x1, int x2, int y1, int y2, int surface) {

	float xLength = x2 - x1;
	float yLength = y2 - y1;
	
	// FOSSE_SURFACE_MIN pos 1
	
	/* si la surface, la taille en x, en y, ou les dimentions en x et en y ne convinnent pas => pas un pont ! */
	if ( ( surface < FOSSE_SURFACE_MIN) || ( surface > FOSSE_SURFACE_MAX) || (( x1 < FOSSE_X_MIN ) && ( x1 < FOSSE_X_POS_1_MIN )) || (( x1 > FOSSE_X_MAX) && ( x1 > FOSSE_X_POS_1_MAX ) ) || ( y1 < FOSSE_Y_MIN ) || ( y1 > FOSSE_Y_MAX ) || (( xLength < FOSSE_WIDTH_MIN) &&( xLength < FOSSE_WIDTH_POS_1_MIN) ) || (( xLength > FOSSE_WIDTH_MAX ) && ( xLength > FOSSE_WIDTH_POS_1_MAX ) ) || ( yLength < FOSSE_HEIGHT_MIN ) || ( yLength > FOSSE_HEIGHT_MAX ) ) {
		return NOT_PONT;
	}

	if ( ( xLength > FOSSE_WIDTH_MIN - DELTA_ESPACE  ) && ( xLength < FOSSE_WIDTH_MIN + DELTA_ESPACE ) ) return PONT_2; 
	if ( ( xLength > FOSSE_WIDTH_MIN + DELTA_PONT - DELTA_ESPACE ) && ( xLength < FOSSE_WIDTH_MIN + DELTA_PONT + DELTA_ESPACE ) ) return PONT_3; 
	if ( ( xLength > FOSSE_WIDTH_MIN + (2 * DELTA_PONT) - DELTA_ESPACE ) && ( xLength < FOSSE_WIDTH_MIN + (2 * DELTA_PONT) + DELTA_ESPACE ) ) return PONT_4; 
	
	/* cas particulier du pont 1 (pas de fossé à gauche !!!) */
	if ( ( xLength > FOSSE_WIDTH_POS_1_MIN - DELTA_ESPACE ) && ( xLength < FOSSE_WIDTH_POS_1_MIN + DELTA_ESPACE ) ) return PONT_1; 
		
	return NOT_PONT;
}


