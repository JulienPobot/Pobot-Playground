#include "gps_simple.h"
#include <string.h>

#define cptoi(p) (*(p) - '0')

// message format :
// 0.........1.........2.........3.........4.....
// 0....5....0....5....0....5....0....5....0....5
// GPRMC,hhmmss,A,ddmm.mmmm,N,dddmm.mmmm,E,....
// ex:
// GPRMC,133612,A,4337.9096,N,00703.6207,E,000.0,131.8,110508,000.0,W*64

#define VALIDITY_INDICATOR	13
#define LATITUDE			15
#define LATITUDE_NS			25
#define LONGITUDE			27	
#define LONGITUDE_EW		38

u08 gps_process_GPRMC(char* data, POSITION_LL_PTR position) {
	// reject invalid data
	if (strncmp(data, "GPRMC", 5) != 0) return FALSE ;
	if (data[VALIDITY_INDICATOR] != 'A') {
		memset(position, 0 , sizeof(POSITION_LL)) ;
		return FALSE ;
	}
	
	char *p ;
	
	// process lattitude
	p = data + LATITUDE ;
	position->lat.degrees = (cptoi(p) * 10 + cptoi(p+1)) * ((data[LATITUDE_NS] == 'S') ? -1 : 1) ; p += 2 ;
	position->lat.minutes = cptoi(p) * 10 + cptoi(p+1) ; p += 3 ;
	position->lat.seconds = 6l * (long)(cptoi(p) * 10 + cptoi(p+1) + ((cptoi(p+2)>4)?1:0)) / 10 ; 
	
	// process longitude
	p = data + LONGITUDE ;
	position->lon.degrees = (cptoi(p) * 100 + cptoi(p+1) * 10 + cptoi(p+2)) * ((data[LONGITUDE_EW] == 'W') ? -1 : 1) ; p += 3 ;
	position->lon.minutes = cptoi(p) * 10 + cptoi(p+1) ; p += 3 ;
	position->lon.seconds = 6l * (long)(cptoi(p) * 10 + cptoi(p+1) + ((cptoi(p+2)>4)?1:0)) / 10 ; 
	
	return TRUE ;
}
