/*****************************************************************************
 * Démonstration AvrX et AVRlib
 * 
 * Exemple d'utilisation conjointe d'AvrX avec AVRlib
 * 
 * Cette application affiche un horloge avec bargraph sur un afficheur LCD 4x20,
 * en utilisant plusieurs tâches AvrX, ainsi qu'une communication par sémaphore
 * et par message.
 * 
 * Les tâches s'exécutant sont :
 * - la tâche horloge, qui gère le décompte du temps et l'affiche sur le LCD
 * - la tâche de scrutation du clavier (bouton poussoir de mise en pause)
 * - la tâche de gestion des actions clavier 
 * - la tâche de gestion de la mise en pause 
 * 
 * Auteur : Eric PASCUAL pour POBOT
 * Cible : ATmega
 * Compilateur : avr-gcc (WinAVR)
 */

#include <avr/io.h>				// définitions des I/O
#include <avr/interrupt.h>		// support des interruptions 
#include <stdlib.h>
#include <string.h>

#include "avrx.h"				// noyau AvrX
#include "hardware.h"			// configuration hardware (assignation des ports,...)

char MSG1[] = "AVRlib + AvrX demo" ;
char MSG2[] = "Running..." ;
char MSG3[] = "Paused    " ;
char MSG4[] = "kb:" ;
char CLK_TEMPLATE[] = "  :  :  " ;

#include "lcd.h"				// module de gestion LCD texte (AVRlib)

AVRX_TIMER (tmr_clock) ;		// timer pour le cadencement de l'horloge 
AVRX_TIMER (tmr_keyb) ;			// timer de scrutation du clavier
AVRX_MESSAGEQ(kbd_queue) ;		// file des messages clavier

// message clavier
typedef struct {
    AVRX_MESSAGE (mcb) ;
    unsigned char changes;
    unsigned char states;
} Switch_Message;
	
// sémaphore de communication
AVRX_MUTEX(sem) ;

// indicateur de pause
static volatile u08 clock_paused = 0 ;
// décompte du temps
static u08 hours = 0, mins = 0, secs = 0;


/*
 * Interrupt handler pour le timer principal, utilisé pour le cadencement d'AvrX
 */
AVRX_SIGINT(SIG_OVERFLOW0)
{
	IntProlog();                // commutation en contexte kernel AvrX
	EndCritical();				// autorise à nouveau les interruptions
	LED ^= 0x08 ;				// utilisé pour calibrer la fréquence avec un oscillo
	TCNT0 = TCNT0_INIT;			// reset du compteur du timer 
    AvrXTimerHandler();         // gestion du temps AvrX
	Epilog();                   // retour au contexte applicatif
}

/*
 * Utilitaire de formattage des valeurs affichées par l'horloge
 */
static void display_99_at(u08 value, u08 x, u08 y) {
	static char buffer[3] ;
	
	if (value > 9) {
		itoa(value, buffer, 10) ;
	} else {
		*buffer = '0' ;
		itoa(value, buffer+1, 10) ;
	}
	lcdGotoXY(x, y) ;
	lcdPrintData (buffer, 2) ;
} 

/*
 * Remise à zéro de l'horloge
 */
static void reset_clock(void) {
	hours = mins = secs = 0 ;
} 
 
/*
 * Tâche de gestion du cadencement de l'horloge
 * 
 * Périodique, s'excéutant toute les secondes
 */
AVRX_GCC_TASKDEF(clock_task, 24, 1) {
	// attend le signal de démarrage des tâches
	AvrXWaitSemaphore(&sem) ;

	// initialise le LCD
	lcdInit();
	lcdLoadCustomChar((u08*)LcdCustomChar,LCDCHAR_FORWARDARROW,7);

	lcdClear() ;
	lcdHome() ;

	// affiche les textes statiques
	lcdPrintData(MSG1, strlen(MSG1)) ;
	
	lcdGotoXY(0, 2) ;
	lcdPrintData(MSG2, strlen(MSG2)) ;
	
	lcdGotoXY(6, 1) ;
	lcdPrintData (CLK_TEMPLATE, strlen(CLK_TEMPLATE)) ;

	lcdGotoXY(4,3) ;
	lcdDataWrite(6) ;
	lcdGotoXY(15,3) ;
	lcdDataWrite(7) ;
	
	// boucle principale de la tâche
    while (1) {
    	if (!clock_paused) {
    		// gestion du temps si l'horloge n'est pas en pause
	    	secs++ ;
	    	if (secs >= 60) {
	    		secs = 0 ;
	    		mins++ ;
	    	}
	    	if (mins >= 60) {
	    		mins = 0 ;
	    		hours++ ;
	    	}
	    	if (hours >= 24) {
	    		hours = 0 ;
	    	}
	    	
	    	// afichage du temps
	    	display_99_at(hours, 6, 1) ;
	    	display_99_at(mins, 9, 1) ;
	    	display_99_at(secs, 12, 1) ;

			// affichage du bargraph représentant les secondes
			lcdGotoXY(5, 3) ;
			lcdProgressBar(secs, 60, 10) ;
    	}

		// on attend une seconde pour continuer
    	AvrXDelay(&tmr_clock, 1000);
    	
    	// clignotement de la LED du heart beat (juste pour indiquer qu'on est toujours en vie ;-)
		LED ^= 0x01 ;
    }
}

/*
 * Tâche de scrutation du clavier
 * 
 * Périodique, s'excéutant toute les 1/10e de secondes
 */
AVRX_GCC_TASKDEF(keyb_polling_task, 16, 2) {
	u08 new_state, old_state = 0;
	static Switch_Message msg ;
	
    while (1) {
    	new_state = (~SWITCH) & 0x0f ; 		// lecture de l'état des switches
    										// NB : état apppuyé = 1 ==> inversion des états des pins 
    	
    	if (new_state != old_state) {		// s'il a changé,
    		// on poste un message indiquant quels sont les changements et l'état actuel
    		msg.changes = new_state ^ old_state ;
    		msg.states = new_state ; 		
    		AvrXSendMessage(&kbd_queue, &(msg.mcb)) ;
    		// on attend son acquittement avant de poursuivre pour éviter de l'écraser par le suivant
    		AvrXWaitMessageAck(&(msg.mcb));
    		
    		old_state = new_state ;			// et on mémorise le nouvel état
    	}
    	
    	AvrXDelay(&tmr_keyb, 100);			// pause de 100ms avant scrutation suivante

    	// clignotement de la LED du heart beat clavier
    	LED ^= 0x04 ;
    }
}

/*
 * Tâche de gestion des événements du clavier
 * 
 * Asynchrone, cadencée par les messages envoyés par la tâche de scrutation
 */
AVRX_GCC_TASKDEF(keyb_handler, 16, 4) {
    MessageControlBlock *p;
    
    // affichage du texte statique pour l'état du clavier
    lcdGotoXY(13, 2) ;
    lcdPrintData(MSG4, strlen(MSG4)) ;

	char kb_display[5] = "----" ;
    while (1) {
    	// affichage de l'état courant du clavier
        lcdGotoXY(16, 2) ;
        lcdPrintData(kb_display, 4) ;

		// attente du message de signalisation d'un événement clavier
        p = AvrXWaitMessage(&kbd_queue);
        
        // récupération des données du message (l'état du clavier)
        u08 changes  = ((Switch_Message*)p)->changes;
        u08 states  = ((Switch_Message*)p)->states;
        
        // acquittement du message
        AvrXAckMessage(p);
        
        // gestion du nouvel état du clavier
        if (changes & 0x01) { 				// appui sur la touche pause
        	if (states & 0x01) { 
	    		AvrXSetSemaphore(&sem) ;	// demande de changement d'état de la pause
    			kb_display[0] = 'X' ;
        	} else {
        		kb_display[0] = '-' ;
        	}
        }
        
        if (changes & 0x02) {
        	if (states & 0x02) {			// appui sur la touche reset 
        		reset_clock() ;
        	}
        	kb_display[1] = (states & 0x02) ? 'X' : '-' ;
        }

        if (changes & 0x04) {
        	kb_display[2] = (states & 0x04) ? 'X' : '-' ;
        }

        if (changes & 0x08) {
        	kb_display[3] = (states & 0x08) ? 'X' : '-' ;
        }
    }
}

/*
 * Tâche de gestion de la pause
 * Bascule l'état de la pause à chaque appui sur la touche pause
 * 
 * Asynchrone, cadencée par le sémaphore de signalisation d'utilisation de la touche pause
 */ 
AVRX_GCC_TASKDEF(pause_task, 8, 3) {
	while(1) {
		// on attend le sémaphore qui indique qu'une touche a été appuyée 
		AvrXWaitSemaphore(&sem) ;
		
		// bascule de la LED témoin
		LED ^= 0x02 ;
		// bascule de l'état de pause de l'horloge
    	clock_paused ^= 0x01 ;
    	
    	// met à jour l'indication d'état sur le LCD
		lcdGotoXY(0, 2) ;
    	if (clock_paused) {
			lcdPrintData(MSG3, strlen(MSG3)) ;
		} else {
			lcdPrintData(MSG2, strlen(MSG2)) ;
		}    		
	}
}

//-----------------------------------------------------------------
int main(void)
{
    AvrXSetKernelStack(0);		// définition du stack du kernel


    MCUCR = 1<<SE;				// autorise l'instruction SLEEP
    TCNT0 = TCNT0_INIT;			// initialise le timer 0 (utilisé pour la gestion des timers
    TCCR0 = TMC8_CK64;			// définit son prescaler
    TIMSK = 1<<TOIE0;			// active l'interruption Timer0 Overflow

    LEDDDR = 0xFF;				// configure en sortie le port des LEDs 
    LED   = 0xFF;				// initialise toutes les sortie à 1 => éteint les LEDs 

    SWITCHDDR = 0x00 ;			// le port des switch est configuré en entrée
    SWITCHPUP = 0xFF ;			// les pullups sont activées => appui sur le poussoir = entrée à 0

	// lancement des différentes tâches
    AvrXRunTask(TCB(clock_task));	
    AvrXRunTask(TCB(keyb_polling_task));	
    AvrXRunTask(TCB(keyb_handler));	
    AvrXRunTask(TCB(pause_task));	

	// autorise le démarrage de la tâche horloge
    AvrXSetSemaphore(&sem) ;

    Epilog();                   // bascule sur le contexte de la première tâche
    							// => active l'ensemble
    							
	while(1) ;
}

