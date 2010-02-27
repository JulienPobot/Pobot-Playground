
    
#include <avr/io.h>

#include <util/delay.h>


static void avr_init(void);


int main(void)
{
    avr_init();
    
    for(;;)
    {
        // Tasks here.
		
		PORTB |= 0x01;
		
		_delay_ms(500);
		
		PORTB &= 0xFE;
		
		_delay_ms(250);
		 
    }
    
    return(0);
}



static void avr_init(void)
{
    // Initialize device here.

	
	
	// le servo
	
	DDRB |= 0x01;
	
	
	// l'interruption
	
	
    
    
    return;
}
    
	
    