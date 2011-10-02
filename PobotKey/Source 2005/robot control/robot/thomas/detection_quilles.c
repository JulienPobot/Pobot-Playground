#include <stdio.h>
#include "detection_quilles.h"

/* TC : detection de la position des quilles */ 
int detectDebout(int x1, int x2, int y1, int y2, int surface) {

	float xLength = x2 - x1;
	float yLength = y2 - y1;
	float surfaceRatio = (float) surface / (float) y1;
	float yxRatio = yLength / xLength;
	
	
	// debug : print ratios !
	// fprintf(stderr, "x/y ratio = %f, surface : %d, surface ratio : %f\n", yxRatio, surface, surfaceRatio);

	// Y/X ratio OK ?
	if ((yxRatio < QUILLE_XY_RATIO_MIN_DEBOUT) || (yxRatio > QUILLE_XY_RATIO_MAX_DEBOUT)) {
		// Y/X NOT OK : surface OK ? 
		if ((surface < QUILLE_SURFACE_MIN) || (surfaceRatio > QUILLE_SURFACE_Y_RATIO_MAX) || (yxRatio < QUILLE_XY_RATIO_MIN_COUCHEE) ) {
	       		//return NOT_QUILLE;
			
			// surface not OK : PILE ?
			if ( (yxRatio > PILE_XY_RATIO_MIN) && (yxRatio < PILE_XY_RATIO_MAX) && (surfaceRatio > PILE_SURFACE_Y_RATIO_MIN) && (surfaceRatio < PILE_SURFACE_Y_RATIO_MAX) ) 
				return PILE_QUILLES;
			else
				return NOT_QUILLE;
		}
		else
			return QUILLE_COUCHEE;
	}
	
	// Y/X OK, surface min OK ?
	if ( surfaceRatio > QUILLE_SURFACE_Y_RATIO_MIN ) { 

		// OK, not to big ?
		if (surfaceRatio <  QUILLE_SURFACE_Y_RATIO_MAX)
			return QUILLE_DEBOUT;		
		else 
			return NOT_QUILLE;
	}
	
	// Y/X OK but to small to be up ! 
	if (surface < QUILLE_SURFACE_MIN) 
		return NOT_QUILLE;
	else
		return QUILLE_COUCHEE;

}


