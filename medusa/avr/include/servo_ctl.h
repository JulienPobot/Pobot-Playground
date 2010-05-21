#ifndef SERVO_CTL_H_
#define SERVO_CTL_H_

#include <avrlibtypes.h>
#include "conf/servo_ctl.conf.h" 

#define SERVO_ID_MIN		0
#define SERVO_ID_MAX		7

#define SERVO_POSITION_MIN	0
#define SERVO_POSITION_MED	64
#define SERVO_POSITION_MAX	128

/*
 * Initialisation du port utilis� pour le contr�le des servos.
 */
void servo_port_init(void) ;

/* 
 * Initialisation des devices utilis�s pour le contr�le des servos
 */
void servo_init_devices(void) ;

/*
 * D�finition de la consigne d'une servo.
 * 
 * Param�tres :
 * id			num�ro du servo (entre SERVO_ID_MIN et SERVO_ID_MAX)
 *  			si invalide, aucune action n'est faite 
 * position		consigne (entre SERVO_POSITION_MIN et SERVO_POSITION_MAX)
 * 				si hors intervalle, ramen� dans l'intervalle
 */ 
void servo_set(u08 id, u08 position) ;

/*
 * Active la g�n�ration des pulses pour un servo
 * 
 * Param�tres :
 * id			num�ro du servo (entre SERVO_ID_MIN et SERVO_ID_MAX)
 *  			si invalide, aucune action n'est faite
 */ 
void servo_enable(u08 id) ;

/*
 * D�sctive la g�n�ration des pulses pour un servo (le met en roue libre)
 * 
 * Param�tres :
 * id			num�ro du servo (entre SERVO_ID_MIN et SERVO_ID_MAX)
 *  			si invalide, aucune action n'est faite
 */ 
void servo_disable(u08 id) ;

#endif /*SERVO_CTL_H_*/

