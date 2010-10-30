  
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <util/delay.h>


// les différentes positions du servomoteur
// on utilise des macros pour pouvoir grouper ici les déclarations
// plutôt que de définir 90 ou 0 on définit aussi le test

#define POSITION_START	0
#define SERVO_IS_OPEN(pos) (pos >= 90)
#define SERVO_IS_CLOSED(pos) (pos <= 0)
#define SERVO_OPEN(pos) (pos++)
#define SERVO_CLOSE(pos) (pos--)

// astuce pour gérer différents états du programme
// chaque valeur est un entier automatiquement incrémenté
// pas besoin de donner des valeurs, il suffit d'ajouter des noms
enum {
   phase_sleep,
   phase_detect,
   phase_open,
   phase_wait,
   phase_close
};

// valeur de l'état en cours
int phase = phase_sleep;

// position du servo
int position = POSITION_START;


/** Déclaration des fonctions **/

// les initialisations du début de programme
static void avr_init(void);


/** Gestion de l'interruption **/

/*
 * Cette interruption est déclenchée dès que l'état de la pin PB1 change,
 * que ce soit un niveau haut ou bas. On ne peut pas utiliser de détection de fronts
 * sur l'interruption externe INT0, car c'est incompatible avec le mode économie d'énergie souhaité
 *
 * Ce comportement est défini par les initialisations effectuées en début de
 * programme (voir les commentaires plus loin)
 */

ISR(PCINT0_vect)
{
    // Changer la led de test pour voir le passage ici
	
	PORTB ^= _BV(0) ;
	
}

/** 
 * 
 * MAIN 
 *
 */ 
int main(void)
{
    avr_init();
	
	// boucle infinie
	// qui contient la gestion des états du programme ("phases")
    for(;;)
    {        
		switch (phase)
		{
			case phase_sleep:
				
				// activation du mode économie d'énergie
				// cela ne démarre pas l'économie d'énergie
				// mais ça active seulement la possibilité de le faire
				sleep_enable();   
				
				
				// passage en mode économie d'énergie
				// cette fois c'est le réel endormissement du microcontrôleur
				// selon le mode définit plus bas
				sleep_cpu();
				
				// lorsque l'interruption arrive, le code reprend ici
				// on peut alors désactiver (pas obligatoire du tout)
				sleep_disable();				
				
				phase = phase_detect;
				
			break;
			
			case phase_detect:
				
				// plus tard, il faudra détecter le signal du réveil
				
				// passer à la phase suivante si on détecte qu'il s'agit bien du réveil
				// et pas du bip des heures
				phase = phase_open;				
				
			break;
			
			case phase_open:
				
				// ouvrir le servo
				
				// gestion du servomoteur 
				
				// mettre à jour la position commandée				
				SERVO_OPEN(position);
				
				// passer à haut le signal servo
				PORTB |= _BV(2);
				
				// attendre la largeur du signal
				// 0 = 0,5 ms / 90 = 1 ms / 180 = 1,5 ms
				
				
				//_delay_ms(0.500+((double)position)*0.005556);
				// Ne jamais utiliser de float
				_delay_ms(1);
				
				// 0 = 1000 ms / 90 = 1500 ms / 180 = 2000 ms
				// _delay_us( 1000 + position / 180 );
			
				
				// passer à bas le signal servo
				PORTB &=~ _BV(2);
				
				// attendre les 20 ms nécessaires
				
				_delay_ms(20);
				
				if (SERVO_IS_OPEN(position)) 
				{ 
					phase = phase_wait;				
				}
				
			break;
			
			case phase_wait:
				
				// attendre une seconde avant de fermer
				_delay_ms(1000);
				
				phase = phase_close;
				
			break;
			
			case phase_close:
				
				// refermer le servo
				
				// gestion du servomoteur 
				
				// mettre à jour la position commandée				
				SERVO_CLOSE(position);
				
				// passer à haut le signal servo
				PORTB |= _BV(2);
				
				// attendre la largeur du signal
				// 0 = 0,5 ms / 90 = 1 ms / 180 = 1,5 ms
				//_delay_ms(0.500+((double)position)*0.005556);
				_delay_ms(2);
				
					// 0 = 1000 ms / 90 = 1500 ms / 180 = 2000 ms
				//_delay_us( 1000 + position / 180 );

				
				// passer à bas le signal servo
				PORTB &=~ _BV(2);
				
				// attendre les 20 ms nécessaires				
				//_delay_us(20000);	

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
	DDRB |= _BV(2); // le servo à activer
	
	DDRB |= _BV(3); // le micro
	PORTB |= _BV(3);
	
	// Les entrées
	
	DDRB &=~ _BV(1); // la patte de déclenchement
	
	
	// L'interruption
	
	// Pour ATtiny : 
	GIMSK |= 1<<PCIE;  // interruption de changement sur une patte (PC = pin change)
	PCMSK |= _BV(1);   // sélection de la patte 1 (remplaçable par 2,3,4,5..)
	
		
	// Pour AT90s2313 : 
	// GIMSK |= _BV(6); // int 0
	
	
	// Choix du mode sleep (ici, le plus "profond" qui économise le plus le courant)
	// seul le reset ou une interruption pin change peuvent sortir le µC de ce mode
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// Activer les interruptions 
	
    sei();
	
	return;
}
