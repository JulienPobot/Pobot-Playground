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


// simple clignotement sur la patte 0 pour le debug
// on utilise "inline" pour que le pr�compilateur recopie ce code � chaque appel
// ce qui fait gagner du temps et �conomise la pile ("stack") des fonctions
// on utilise "static" pour utiliser _delay_ms lui-m�me static
static void inline clignote(void)
{
#ifdef DEBUG
	PORTB |= _BV(0);
	_delay_ms(100);
	PORTB &= ~(_BV(0));
	_delay_ms(300);
#endif
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
				_delay_ms(0.500+((double)position)*0.005556);
				
				// passer � bas le signal servo
				PORTB &= _BV(2);
				
				// attendre les 20 ms n�cessaires
				
				_delay_ms(20);
				
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
				_delay_ms(0.500+((double)position)*0.005556);
				
				// passer � bas le signal servo
				PORTB &= _BV(2);
				
				// attendre les 20 ms n�cessaires				
				_delay_ms(20);				
				
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
