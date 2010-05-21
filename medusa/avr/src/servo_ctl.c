#include <avr/io.h> 
#include <avr/interrupt.h>

#include "servo_ctl.h"

// nombre de servos dans une trame d'impulsions (10 <=> 20ms de récurrence)
#define SERVO_COUNT		10

// durées typiques (en µs)
#define PULSE_WIDTH_EXTENT	1024
#define PULSE_MED_WIDTH 	1500

#define PULSE_MIN_WIDTH		(PULSE_MED_WIDTH - PULSE_WIDTH_EXTENT / 2)
#define PULSE_MAX_WIDTH		(PULSE_MED_WIDTH + PULSE_WIDTH_EXTENT / 2)

// Cette macro traduit en nombre de clocks une durée exprimée en µs 
// Compte tenu de la configuration de l'horloge et du timer 1, 1µs correspond à 2 clocks
#define _TMR1_US(x)	((x) * 2)

// largeurs des pulses des servos, en fonction de la position demandée
u16 pulse_widths[SERVO_COUNT];

// masque d'état des servos (enable/disable) -> disable par défaut
u08 servo_enable_mask = 0;

// id du servo courant (dont on génère le créneau)
u08 cur_servo = 0;

/*
 Interrupt handler pour l'Ouput Compare A du timer 1
 
 Le passage à cette valeur déclenche le rebouclage du compteur, et
 correspond au début du créneau.
 */
SIGNAL(SIG_OUTPUT_COMPARE1A) {
	// On monte la sortie du servo en cours de traitement (s'il est utilisé et activé)
	if (_BV(cur_servo) & SERVOS_IN_USE & servo_enable_mask) {
		PORT_SERVOS |= _BV(cur_servo);

		// On définit le comparateur B en fonction de la durée de l'impulsion à générer.
		OCR1B = pulse_widths[cur_servo];

	} else {

		/* 
		 * Pour les impulsions correspondant aux slots inutilisés ou servos désactivés, on prend la valeur du neutre 
		 * (en fait n'importe quelle valeur convient, puisqu'on ne génère pas d'impulsion dans ce cas)
		 */
		OCR1B = _TMR1_US(PULSE_MED_WIDTH);
	}
}

/*
 Interrupt handler pour l'Ouput Compare B du timer 1
 
 Le passage à cette valeur correspond à la fin du créneau pour
 le servo en cours.
 */
SIGNAL(SIG_OUTPUT_COMPARE1B) {
	// on descend la sortie du servo en cours de traitement (sans effet sur un servo disabled, puisque le
	// front montant n'a pas été généré)
	PORT_SERVOS &= ~_BV(cur_servo);

	// on passe au servo suivant, en rebouclant en fin de série
	if (++cur_servo == SERVO_COUNT) {
		cur_servo = 0;
	}
}

void servo_port_init(void) {
	// configuration en output des pins du port de contrôle 
	// pour les canaux utilisés  
	DDR_SERVOS |= SERVOS_IN_USE;
	// - toutes les sorties inactives
	PORT_SERVOS &= ~SERVOS_IN_USE;
}

void servo_init_devices(void) {
	// Initialisation du Timer/Counter 1
	/*
	 On utilise un prescale de 8, ce qui pour un quartz à 16 MHz donne donc 2000 clocks
	 par ms. Les valeurs des compteurs expriment donc les timings par pas de 0.5 µs.
	 
	 Le créneau maximal de la commande du servo étant de 2ms, on va configurer le timer 
	 pour qu'il déclenche une interruption et reboucle automatiquement en atteignant
	 cette valeur (mode CTC), grâce au premier comparateur (OCR1A). Cette interruption nous 
	 servira à déclencher le font montant du créneau de commande. 
	 
	 L'instant du front descendant sera signalé par l'interruption du deuxième comparateur 
	 (OCR1B) dont la valeur sera variable et traduira la position qu'on veut donner au servo.
	 */

	// Configuration du timer 1
	// - mode CTC avec TOP indiqué par registre OCR1A
	// => seul TCCR1B est à définir, le reste de la config donnant 0 pour TCCR1A
	// (voir pages 135 et suivantes du datasheet)
	TCCR1B |= _BV(WGM12) // Waveform Generation Mode = CTC (TOP = OCR1A)
			| _BV(CS11) // prescale à 8
	;

	// Définition du TOP à la largeur d'impulsion max + epsilon 
	/*
	 (pour éviter d'éventuels pbs aux limites pour la génération d'un créneau de 2ms,
	 situation dans laquelle OCR1A et OCR1B auraient les mêmes valeurs, et donc les 
	 interruptions correspondantes pourraient arriver dans la mauvaise séquence)
	 */
	OCR1A = _TMR1_US(PULSE_MAX_WIDTH + 10);

	// initialisation de OCR1B à la valeur de mi-course des servo
	OCR1B = _TMR1_US(PULSE_MED_WIDTH);

	// initialisation du point de départ du compteur juste après cette valeur,
	// pour démarrer le processus dans la bonne phase
	TCNT1 = OCR1B + 10;

	// Activation des interuptions des comparateurs A et B du timer 1
	TIMSK |= _BV(OCIE1A) | _BV(OCIE1B);

	u08 i;
	for (i = 0; i < SERVO_COUNT; i++) {
		servo_set(i, SERVO_POSITION_MED) ;
	}
}

void servo_set(u08 id, u08 position) {
	if (id > SERVO_ID_MAX)
		return;
	if (position > SERVO_POSITION_MAX)
		position = SERVO_POSITION_MAX;
	pulse_widths[id]
			= _TMR1_US (PULSE_MIN_WIDTH + PULSE_WIDTH_EXTENT / SERVO_POSITION_MAX * position);
}

void servo_enable(u08 id) {
	if (id > SERVO_ID_MAX)
		return;
	servo_enable_mask |= _BV(id) ;
}

void servo_disable(u08 id) {
	if (id > SERVO_ID_MAX)
		return;
	servo_enable_mask &= ~_BV(id) ;
}
