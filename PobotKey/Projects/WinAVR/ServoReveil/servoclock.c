/**
 * Programme pour piloter un servomoteur depuis un r�veil-matin
 *
 * - utilisation d'un ATtiny13
 * - utilisation du mode �conomie d'�nergie (pour �conomiser les piles)
 * - utilisation d'une interruption externe (signal du buzzer du r�veil)
 * - �lectronique de base (coupure de l'alimentation du servo)
 *
 */
 
    
#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>


/** D�claration des fonctions **/

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
	
	// Les entr�es
	
	DDRB &= ~(_BV(1));
	
	
	// l'interruption
	

	// Enable INT0 External Interrupt
	GIMSK |= 1<<INT0;

	// Falling-Edge Triggered INT0
	MCUCR |= 1<<ISC01;

	// Enable Interrupts
	sei(); 	
    
    
    return;
}