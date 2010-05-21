#include <avr/io.h> 
#include <avr/interrupt.h>

#include "servo_ctl.h"

// nombre de servos dans une trame d'impulsions (10 <=> 20ms de r�currence)
#define SERVO_COUNT		10

// dur�es typiques (en �s)
#define PULSE_WIDTH_EXTENT	1024
#define PULSE_MED_WIDTH 	1500

#define PULSE_MIN_WIDTH		(PULSE_MED_WIDTH - PULSE_WIDTH_EXTENT / 2)
#define PULSE_MAX_WIDTH		(PULSE_MED_WIDTH + PULSE_WIDTH_EXTENT / 2)

// Cette macro traduit en nombre de clocks une dur�e exprim�e en �s 
// Compte tenu de la configuration de l'horloge et du timer 1, 1�s correspond � 2 clocks
#define _TMR1_US(x)	((x) * 2)

// largeurs des pulses des servos, en fonction de la position demand�e
u16 pulse_widths[SERVO_COUNT];

// masque d'�tat des servos (enable/disable) -> disable par d�faut
u08 servo_enable_mask = 0;

// id du servo courant (dont on g�n�re le cr�neau)
u08 cur_servo = 0;

/*
 Interrupt handler pour l'Ouput Compare A du timer 1
 
 Le passage � cette valeur d�clenche le rebouclage du compteur, et
 correspond au d�but du cr�neau.
 */
SIGNAL(SIG_OUTPUT_COMPARE1A) {
	// On monte la sortie du servo en cours de traitement (s'il est utilis� et activ�)
	if (_BV(cur_servo) & SERVOS_IN_USE & servo_enable_mask) {
		PORT_SERVOS |= _BV(cur_servo);

		// On d�finit le comparateur B en fonction de la dur�e de l'impulsion � g�n�rer.
		OCR1B = pulse_widths[cur_servo];

	} else {

		/* 
		 * Pour les impulsions correspondant aux slots inutilis�s ou servos d�sactiv�s, on prend la valeur du neutre 
		 * (en fait n'importe quelle valeur convient, puisqu'on ne g�n�re pas d'impulsion dans ce cas)
		 */
		OCR1B = _TMR1_US(PULSE_MED_WIDTH);
	}
}

/*
 Interrupt handler pour l'Ouput Compare B du timer 1
 
 Le passage � cette valeur correspond � la fin du cr�neau pour
 le servo en cours.
 */
SIGNAL(SIG_OUTPUT_COMPARE1B) {
	// on descend la sortie du servo en cours de traitement (sans effet sur un servo disabled, puisque le
	// front montant n'a pas �t� g�n�r�)
	PORT_SERVOS &= ~_BV(cur_servo);

	// on passe au servo suivant, en rebouclant en fin de s�rie
	if (++cur_servo == SERVO_COUNT) {
		cur_servo = 0;
	}
}

void servo_port_init(void) {
	// configuration en output des pins du port de contr�le 
	// pour les canaux utilis�s  
	DDR_SERVOS |= SERVOS_IN_USE;
	// - toutes les sorties inactives
	PORT_SERVOS &= ~SERVOS_IN_USE;
}

void servo_init_devices(void) {
	// Initialisation du Timer/Counter 1
	/*
	 On utilise un prescale de 8, ce qui pour un quartz � 16 MHz donne donc 2000 clocks
	 par ms. Les valeurs des compteurs expriment donc les timings par pas de 0.5 �s.
	 
	 Le cr�neau maximal de la commande du servo �tant de 2ms, on va configurer le timer 
	 pour qu'il d�clenche une interruption et reboucle automatiquement en atteignant
	 cette valeur (mode CTC), gr�ce au premier comparateur (OCR1A). Cette interruption nous 
	 servira � d�clencher le font montant du cr�neau de commande. 
	 
	 L'instant du front descendant sera signal� par l'interruption du deuxi�me comparateur 
	 (OCR1B) dont la valeur sera variable et traduira la position qu'on veut donner au servo.
	 */

	// Configuration du timer 1
	// - mode CTC avec TOP indiqu� par registre OCR1A
	// => seul TCCR1B est � d�finir, le reste de la config donnant 0 pour TCCR1A
	// (voir pages 135 et suivantes du datasheet)
	TCCR1B |= _BV(WGM12) // Waveform Generation Mode = CTC (TOP = OCR1A)
			| _BV(CS11) // prescale � 8
	;

	// D�finition du TOP � la largeur d'impulsion max + epsilon 
	/*
	 (pour �viter d'�ventuels pbs aux limites pour la g�n�ration d'un cr�neau de 2ms,
	 situation dans laquelle OCR1A et OCR1B auraient les m�mes valeurs, et donc les 
	 interruptions correspondantes pourraient arriver dans la mauvaise s�quence)
	 */
	OCR1A = _TMR1_US(PULSE_MAX_WIDTH + 10);

	// initialisation de OCR1B � la valeur de mi-course des servo
	OCR1B = _TMR1_US(PULSE_MED_WIDTH);

	// initialisation du point de d�part du compteur juste apr�s cette valeur,
	// pour d�marrer le processus dans la bonne phase
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
