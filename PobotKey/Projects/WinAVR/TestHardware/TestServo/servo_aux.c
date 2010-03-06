//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings

#include <util/delay.h>

int main(void)
{	
	// port en sortie
	sbi(DDRB,0);
	
	// port en sortie 
	sbi(DDRD,1);
	cbi(PORTD,1);
	
	
	// port en entrée
	cbi(DDRD,0);
	cbi(PORTD,0); // pull-down

	while (1) {

		sbi(PORTB,0);
		if (inb(PIND) & 0x01) {
			cbi(PORTD,1);
			_delay_ms(1.3);
		} else {
			sbi(PORTD,1);
			_delay_ms(1.8);
		}		
		cbi(PORTB,0);
		_delay_ms(20);
		
	} 
	return 0;
}
