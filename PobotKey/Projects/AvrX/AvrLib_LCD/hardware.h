/*
    hardware.h

	Adapted from AvrX original hardware.h by E. Pascual to be compatable with WinAVR makefile
*/

#ifndef TICKRATE
	#warning "TICKRATE not defined. Using default value (1000)"
	#define TICKRATE 1000       // AvrX timer queue Tick rate
#endif

#ifndef BAUDRATE
	#warning "BAUDRATE not defined. Using default value (19200)"
	#define BAUDRATE 19200L     // Debug monitor baudrate
#endif

// Application specific hardware settings

#define LED 		PORTB           	// Change this per your hardware and jumper setup
#define LEDDDR 		DDRB

#define SWITCH		PIND				
#define SWITCHDDR	DDRD
#define SWITCHPUP	PORTD

// Peripheral initialization

// main timer used for AvrX timer management
#define PRESCALE 64
#define TMR0_SUBSTEPS	1
#define TCNT0_INIT (0xFF-F_CPU/PRESCALE/TICKRATE/TMR0_SUBSTEPS + 11)		// +11 pour ajustement de la période

// NB: Clock divisor bits are different between classic and mega103 chips!
// NB: IAR chooses to use different CPU identifiers for their C compiler
// NB: GCC changes the IO bit names just for fun...

#if defined (__AVR_ATmega103__) || defined (__ATmega103__)
#	define TMC8_CK256 ((1<<CS02) | (1<<CS01))
#elif defined (__AVR_ATmega128__) || defined (__ATmega128__)
#	define TMC8_CK256 ((1<<CS02) | (1<<CS01))
#else	// Most other AVR processors
#	define TMC8_CK256 (1<<CS02)
#	define TMC8_CK64 ((1<<CS01) | (1<<CS00)) 
#endif

// serial port baud register initialisation, depending on clock speed, 
// prescaler setting and required communication speed
#define UBRR_INIT (F_CPU/(16*BAUDRATE)-1)


