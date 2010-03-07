/**
 * Programme pour piloter un servomoteur depuis un réveil-matin
 *
 * - utilisation d'un ATtiny13
 * - utilisation du mode économie d'énergie (pour économiser les piles)
 * - utilisation d'une interruption externe (signal du buzzer du réveil)
 * - électronique de base (coupure de l'alimentation du servo)
 *
 */
 
    
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <util/delay.h>

enum {
   phase_sleep,
   phase_detect,
   phase_open,
   phase_wait,
   phase_close
};

int phase = phase_sleep;


/** Déclaration des fonctions **/

static void avr_init(void);

/** Gestion de l'interruption **/

ISR(PCINT0_vect)
{
    // Changer la led 2 pour voir le passage ici
	
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
	
	// mettre en veille
	
	//
		
		PORTB |= _BV(0);
		
		_delay_ms(500);
		
		PORTB &= ~(_BV(0));
		
		_delay_ms(250);
    
    
    for(;;)
    {
        // Tasks here.
		
		switch (phase)
		{
			case phase_sleep:
				
				sleep_enable();   
				sleep_cpu();
				sleep_disable();
				
				PORTB |= _BV(0);
				_delay_ms(100);
				PORTB &= ~(_BV(0));
				_delay_ms(100);
				
				phase = phase_open;
				
			break;
			
			case phase_open:
				
				PORTB |= _BV(0);
				_delay_ms(500);
				PORTB &= ~(_BV(0));
				_delay_ms(250);
				
			break;
		}		
		
    }
    
    return(0);
}



static void avr_init(void)
{
	cli();

    // Les sorties
	
	DDRB |= _BV(0);
	DDRB |= _BV(2);
	
	// Les entrées
	
	DDRB &= ~(_BV(1));
	
	
	// l'interruption
	
	GIMSK |= 1<<PCIE;  // 
	
	PCMSK |= _BV(1);
	
	// le mode sleep
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    sei();
	
	return;
}
