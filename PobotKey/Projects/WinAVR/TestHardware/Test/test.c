//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "pulse.h"		// include pulse driver
#include "i2c.h"		// include i2c driver

#include "global.h"

int main(void)
{
	
	timerInit();
	pulseInit();	
	pulseT1Init();
	
	DDRB = 0xFF;

	// enable moteur
	sbi(PORTB,4);
	sbi(PORTB,5);
	
	//
	pulseT1BSetFreq(800);
	pulseT1BRun(800);
	pulseT1ASetFreq(800);
	pulseT1ARun(800);
	
	while (pulseT1ARemaining() || pulseT1BRemaining());		
		
	cbi(PORTB,4);
	cbi(PORTB,5);	
	
	return 0;
}
