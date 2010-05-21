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
 * Initialisation du port utilisé pour le contrôle des servos.
 */
void servo_port_init(void) ;

/* 
 * Initialisation des devices utilisés pour le contrôle des servos
 */
void servo_init_devices(void) ;

/*
 * Définition de la consigne d'une servo.
 * 
 * Paramètres :
 * id			numéro du servo (entre SERVO_ID_MIN et SERVO_ID_MAX)
 *  			si invalide, aucune action n'est faite 
 * position		consigne (entre SERVO_POSITION_MIN et SERVO_POSITION_MAX)
 * 				si hors intervalle, ramené dans l'intervalle
 */ 
void servo_set(u08 id, u08 position) ;

/*
 * Active la génération des pulses pour un servo
 * 
 * Paramètres :
 * id			numéro du servo (entre SERVO_ID_MIN et SERVO_ID_MAX)
 *  			si invalide, aucune action n'est faite
 */ 
void servo_enable(u08 id) ;

/*
 * Désctive la génération des pulses pour un servo (le met en roue libre)
 * 
 * Paramètres :
 * id			numéro du servo (entre SERVO_ID_MIN et SERVO_ID_MAX)
 *  			si invalide, aucune action n'est faite
 */ 
void servo_disable(u08 id) ;

#endif /*SERVO_CTL_H_*/

