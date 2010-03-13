/**


	VERSION 128 Khz


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
#include <avr/sleep.h>

#include <util/delay.h>

// passage en mode DEBUG
#define DEBUG

// les diff�rentes positions du servomoteur
// on utilise des macros pour pouvoir grouper ici les d�clarations
// plut�t que de d�finir 90 ou 0 on d�finit aussi le test

#define POSITION_START	0
#define SERVO_IS_OPEN(pos) (pos >= 90)
#define SERVO_IS_CLOSED(pos) (pos <= 0)
#define SERVO_OPEN(pos) (pos++)
#define SERVO_CLOSE(pos) (pos--)

// astuce pour g�rer diff�rents �tats du programme
// chaque valeur est un entier automatiquement incr�ment�
// pas besoin de donner des valeurs, il suffit d'ajouter des noms
enum {
   phase_sleep,
   phase_detect,
   phase_open,
   phase_wait,
   phase_close
};

// valeur de l'�tat en cours
int phase = phase_sleep;

// position du servo
int position = POSITION_START;


/** D�claration des fonctions **/

// les initialisations du d�but de programme
static void avr_init(void);
static void servo_start( uint8_t pos );
static void servo_set( uint8_t pos );
static void servo_stop(void);

static volatile int8_t servo_20ms;
static volatile uint8_t servo_pos;

// Tick timer pour 1000 uS (calcul en 1/10 de ns)
// 128 Khz 128 therotique - nb instruc interrupt + calcul
// 4800Khz : 75 theorique

#define SERVO_1000 75
// Nb de fois 1000uS pour 20 Ms 0..18 + pulse 1..2
#define SERVO_20MS 18
#define SERVO_ON()   PORTB |= _BV(2)
#define SERVO_Is_ON() (PORTB & _BV(2)) 
#define SERVO_OFF()  PORTB &= ~_BV(2)


// simple clignotement sur la patte 0 pour le debug
// on utilise "inline" pour que le pr�compilateur recopie ce code � chaque appel
// ce qui fait gagner du temps et �conomise la pile ("stack") des fonctions
// on utilise "static" pour utiliser _delay_ms lui-m�me static
void inline clignote(void)
{
#ifdef DEBUG
	PORTB |= _BV(0);
	PORTB &= ~(_BV(0));
#endif
}


static void servo_start( uint8_t pos ){
	// Reset prescaler
	// GTCCR � General Timer/Counter Control Register
	/*
	12.4 Register Description.
12.4.1 GTCCR � General Timer/Counter Control Register
� Bit 7 � TSM: Timer/Counter Synchronization Mode
Writing the TSM bit to one activates the Timer/Counter Synchronization mode. In this mode, the
value that is written to the PSR10 bit is kept, hence keeping the Prescaler Reset signal asserted.
This ensures that the Timer/Counter is halted and can be configured without the risk of advancing
during configuration. When the TSM bit is written to zero, the PSR10 bit is cleared by
hardware, and the Timer/Counter start counting.
� Bit 0 � PSR10: Prescaler Reset Timer/Counter0
When this bit is one, the Timer/Counter0 prescaler will be Reset. This bit is normally cleared
immediately by hardware, except if the TSM bit is set.
	*/
	GTCCR |= _BV( TSM ) | _BV( PSR10 ); // �conomie d'�nergie
	// init Timer0
	//TCCR0A � Timer/Counter Control Register A
	/*
		Normal port operation, OC0A disconnected
		Normal port operation, OC0B disconnected.
		Waveform Generation Mode Bit Description : 0000 normal
	*/

	TCCR0A = 0b0000000; // Initial value
	
	// TCCR0B � Timer/Counter Control Register B
	/*
		Bits 2:0 � CS02:0: Clock Select
		CS02 CS01 CS00 Description
		0 0 1 clkI/O/(No prescaling)     Pour 128 Khz
		0 1 1 clkI/O/64 (From prescaler) Pour 4800 Khz
	*/
 
	TCCR0B |= (0 << CS02)|( 1 << CS01 )|( 1 << CS00) ;
	
	// TIFR0 � Timer/Counter 0 Interrupt Flag Register
	// Bit 2 � OCF0A: Output Compare Flag 0 A
	TIFR0 |= ( 1 << OCF0A ); // Clear by put 1
	
	
	// TIMSK0 � Timer/Counter Interrupt Mask Register
	// Bit 2 � OCIE0A: Timer/Counter0 Output Compare Match A Interrupt Enable
	TIMSK0 |= ( 1 << OCIE0A );
	
	
	
	// Clear Timer ( for debug )
	TCNT0 = 0;
	
	// Start Timer Clear TSM
	GTCCR &= ~(1<<TSM);


	

	// init servo_pos
	servo_pos = pos;


	// OCR0A � Output Compare Register A
	servo_20ms = SERVO_20MS;
	OCR0A = TCNT0 + SERVO_1000;
 
}

ISR( TIM0_COMPA_vect ){
	// Interrupt on OCR0A 
	
	clignote();
	
	if ( servo_20ms <= 0) {
			
			switch ( servo_20ms ) {
			
			case 0:
				// Pulse
				// Start 1 pulse
				SERVO_ON();
				OCR0A = TCNT0 + SERVO_1000;
				servo_20ms = -1;
				break;
				
			case -1:
			
				// Suite pulse suivant servo_pos
				OCR0A = TCNT0 + ( servo_pos );
				servo_20ms = -2;
				break;
			
			case -2:
				// End pulse
				SERVO_OFF();
				// Re-arm for 20 ms
				servo_20ms = SERVO_20MS;
				OCR0A = TCNT0 + SERVO_1000;
				break;
			} // End switch
	} else {
		// decrement for wait 20ms
		OCR0A = TCNT0 + SERVO_1000;
		servo_20ms--;
	}
		clignote();

}





/** Gestion de l'interruption **/

/*
 * Cette interruption est d�clench�e d�s que l'�tat de la pin PB1 change,
 * que ce soit un niveau haut ou bas. On ne peut pas utiliser de d�tection de fronts
 * sur l'interruption externe INT0, car c'est incompatible avec le mode �conomie d'�nergie souhait�
 *
 * Ce comportement est d�fini par les initialisations effectu�es en d�but de
 * programme (voir les commentaires plus loin)
 */

ISR(PCINT0_vect)
{
    // Changer la led 2 pour voir le passage ici
	
	PORTB ^= _BV(2) ;
	
}

/** 
 * 
 * MAIN 
 *
 */ 
int main(void)
{
    avr_init();
		servo_start(SERVO_1000*90/180);
		
		while(1) {};
	
	// premier clignotement
	clignote();
	
	// boucle infinie
	// qui contient la gestion des �tats du programme ("phases")
    for(;;)
    {        
		switch (phase)
		{
			case phase_sleep:
				
				// activation du mode �conomie d'�nergie
				// cela ne d�marre pas l'�conomie d'�nergie
				// mais �a active seulement la possibilit� de le faire
				sleep_enable();   
				
				clignote(); // on a donc 2 clignotements au reset
				
				// passage en mode �conomie d'�nergie
				// cette fois c'est le r�el endormissement du microcontr�leur
				// selon le mode d�finit plus bas
				sleep_cpu();
				
				// lorsque l'interruption arrive, le code reprend ici
				// on peut alors d�sactiver (pas obligatoire du tout)
				sleep_disable();				
				
				phase = phase_detect;
				
			break;
			
			case phase_detect:
				
				// plus tard, il faudra d�tecter le signal du r�veil
				
				// passer � la phase suivante si on d�tecte qu'il s'agit bien du r�veil
				// et pas du bip des heures
				phase = phase_open;				
				
			break;
			
			case phase_open:
				
				// ouvrir le servo
				
				// simple clignotement pour signaler la phase
				clignote();
				
				// gestion du servomoteur 
				
				// mettre � jour la position command�e				
				SERVO_OPEN(position);
				
				// passer � haut le signal servo
				PORTB |= _BV(2);
				
				// attendre la largeur du signal
				// 0 = 0,5 ms / 90 = 1 ms / 180 = 1,5 ms
				
				
				//_delay_ms(0.500+((double)position)*0.005556);
				// Ne jamais utiliser de float
				
				
					// 0 = 1000 ms / 90 = 1500 ms / 180 = 2000 ms
				//_delay_us( 1000 + position / 180 );

				
				
				
				
				// passer � bas le signal servo
				PORTB &= _BV(2);
				
				// attendre les 20 ms n�cessaires
				
				//_delay_us(20000);
				
				if (SERVO_IS_OPEN(position)) 
				{ 
					phase = phase_wait;				
				}
				
			break;
			
			case phase_wait:
				
				// attendre une seconde avant de fermer
				
				
			break;
			
			case phase_close:
				
				// refermer le servo
				
				// gestion du servomoteur 
				
				// mettre � jour la position command�e				
				SERVO_CLOSE(position);
				
				// passer � haut le signal servo
				PORTB |= _BV(2);
				
				// attendre la largeur du signal
				// 0 = 0,5 ms / 90 = 1 ms / 180 = 1,5 ms
				//_delay_ms(0.500+((double)position)*0.005556);
				
					// 0 = 1000 ms / 90 = 1500 ms / 180 = 2000 ms
				//_delay_us( 1000 + position / 180 );

				
				// passer � bas le signal servo
				PORTB &= _BV(2);
				
				// attendre les 20 ms n�cessaires				
				//_delay_us(20000);				
				
				if (SERVO_IS_CLOSED(position))
				{
					phase = phase_sleep;
				}
				
			break;
		}		
		
    }
    
    return(0);
}



static void avr_init(void)
{
	cli();

    // Les sorties
	
	DDRB |= _BV(0); // la led de test
	DDRB |= _BV(2); // le servo � activer
	
	// Les entr�es
	
	DDRB &= ~(_BV(1)); // la patte de d�clenchement
	
	
	// L'interruption
	
	GIMSK |= 1<<PCIE;  // interruption de changement sur une patte (PC = pin change)
	
	PCMSK |= _BV(1);   // s�lection de la patte 1 (rempla�able par 2,3,4,5..)
	
	// Choix du mode sleep (ici, le plus "profond" qui �conomise le plus le courant)
	// seul le reset ou une interruption pin change peuvent sortir le �C de ce mode
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// Activer les interruptions 
	
    sei();
	
	return;
}
