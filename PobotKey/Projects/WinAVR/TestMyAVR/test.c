//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "vt100.h"		// include VT100 terminal support

int main(void)
{
	// port B all output
	DDRB = 0xFF;
	PORTB = 0x00;
	
	//
	timerInit();
	uartInit();
	uartSetBaudRate(9600);
	rprintfInit(uartSendByte);
	vt100Init();
	vt100ClearScreen();
	//
	while (1) 
	{			
		rprintf("Cool program\r\n");
		
		PORTB += 1;
		
		delay_ms(500);
		
	}
		
	return 0;
}
