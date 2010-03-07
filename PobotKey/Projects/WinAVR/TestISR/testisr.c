/**
 * Programme de démonstration d'utilisation d'interruptions avec AVR-GCC
 *
 * - utilisation d'un ATtiny13
 * - utilisation d'une interruption externe 
 *
 */
 
    
#include "avr/io.h"
#include "avr/interrupt.h"

#include <util/delay.h>


/** Déclaration des fonctions **/

static void avr_init(void);

/** Gestion de l'interruption **/

ISR(INT0_vect)
{
    // user code here
	if (bit_is_set(PORTB,2)) {
		PORTB &= ~(_BV(2));	
	} else {
		PORTB |= _BV(2);		
	}
}

/** 
 * 
 * MAIN 
 *
 */ 
int main(void)
{
    avr_init();
    
    for(;;)
    {
        // Tasks here.
		
		PORTB |= _BV(0);
		
		_delay_ms(500);
		
		PORTB &= ~(_BV(0));
		
		_delay_ms(250);
		
		
    }
    
    return(0);
}



static void avr_init(void)
{
    // Les sorties
	
	DDRB |= _BV(0);
	DDRB |= _BV(2);
	
	// Les entrées
	
	DDRB &= ~(_BV(1));
	
	
	// L'interruption
	

	// Activer l'interruption externe "INT0"
	GIMSK |= 1<<INT0;

	// Définir un déclenchement sur front descendant
	// - lire la datasheet
	// - trouver le tableau qui dit que ISC00 = 0 et ISC01 = 1 pour le trigger falling-edge
	
	MCUCR |= 1<<ISC01;

	// Enable Interrupts
	sei(); 	
    
    
    return;
}
    
	
    