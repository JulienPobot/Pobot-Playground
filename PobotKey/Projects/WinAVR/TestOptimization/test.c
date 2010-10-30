//----- Include Files ---------------------------------------------------------
#include "avr/io.h"		// include I/O definitions (port names, pin names, etc)
#include "avr/interrupt.h"	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library (timing, PWM, etc)

#include "sbit.h"		// la macro "sbit" de JB

#define PAC1	SBIT(PORTC,1)
#define PAC2	SBIT(PORTC,2)
#define PAC3	SBIT(PORTC,3)
#define PAC4	SBIT(PORTC,4)

void inline allumer(void);

void inline eteindre(void);

int main(void)
{	
	timerInit();	
	DDRC = 0xFF; // port C en sortie		
	
	
	while (1)
	{	
		
		// lancer un front haut pour démarrer le chrono
		sbi(PORTC,0);		
		
		// allumer toute la série de leds
		allumer();
		
		// les éteindre dans le sens contraire
		eteindre();
		
		// un front bas pour éteindre le chrono
		cbi(PORTC,0);
		
		delay_ms(1);
	}	
	
	return 0;	
}

void inline allumer(void)
{
	// pourrait faire 100 lignes de ce genre :
	
	/*
	sbi(PORTC,1);	
	sbi(PORTC,2);	
	sbi(PORTC,3);	
	sbi(PORTC,4);	
	*/
	PAC1 = 1;
	PAC2 = 1;
	PAC3 = 1;
	PAC4 = 1;
}

void inline eteindre(void)
{
	/*
	cbi(PORTC,4);	
	cbi(PORTC,3);	
	cbi(PORTC,2);	
	cbi(PORTC,1);	
	*/
	PAC4 = 0;
	PAC3 = 0;
	PAC2 = 0;
	PAC1 = 0;
}
