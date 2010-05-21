#ifndef GPS_SIMPLE_H_
#define GPS_SIMPLE_H_

#include <avrlibtypes.h>

struct _DMS {
	s16	degrees ;
	u08 minutes;
	u08 seconds ;
} ;

typedef struct _POSITION_LL {
	struct _DMS	lon ;
	struct _DMS	lat ;
} POSITION_LL, *POSITION_LL_PTR;

u08 gps_process_GPRMC(char* data, POSITION_LL_PTR position) ;

#endif /*GPS_SIMPLE_H_*/
