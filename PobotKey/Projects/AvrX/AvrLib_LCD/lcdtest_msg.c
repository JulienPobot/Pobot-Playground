/*****************************************************************************
 * D�monstration AvrX et AVRlib
 * 
 * Exemple d'utilisation conjointe d'AvrX avec AVRlib
 * 
 * Cette application affiche un horloge avec bargraph sur un afficheur LCD 4x20,
 * en utilisant plusieurs t�ches AvrX, ainsi qu'une communication par s�maphore
 * et par message.
 * 
 * Les t�ches s'ex�cutant sont :
 * - la t�che horloge, qui g�re le d�compte du temps et l'affiche sur le LCD
 * - la t�che de scrutation du clavier (bouton poussoir de mise en pause)
 * - la t�che de gestion des actions clavier 
 * - la t�che de gestion de la mise en pause 
 * 
 * Auteur : Eric PASCUAL pour POBOT
 * Cible : ATmega
 * Compilateur : avr-gcc (WinAVR)
 */

#include <avr/io.h>				// d�finitions des I/O
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
	
// s�maphore de communication
AVRX_MUTEX(sem) ;

// indicateur de pause
static volatile u08 clock_paused = 0 ;
// d�compte du temps
static u08 hours = 0, mins = 0, secs = 0;


/*
 * Interrupt handler pour le timer principal, utilis� pour le cadencement d'AvrX
 */
AVRX_SIGINT(SIG_OVERFLOW0)
{
	IntProlog();                // commutation en contexte kernel AvrX
	EndCritical();				// autorise � nouveau les interruptions
	LED ^= 0x08 ;				// utilis� pour calibrer la fr�quence avec un oscillo
	TCNT0 = TCNT0_INIT;			// reset du compteur du timer 
    AvrXTimerHandler();         // gestion du temps AvrX
	Epilog();                   // retour au contexte applicatif
}

/*
 * Utilitaire de formattage des valeurs affich�es par l'horloge
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
 * Remise � z�ro de l'horloge
 */
static void reset_clock(void) {
	hours = mins = secs = 0 ;
} 
 
/*
 * T�che de gestion du cadencement de l'horloge
 * 
 * P�riodique, s'exc�utant toute les secondes
 */
AVRX_GCC_TASKDEF(clock_task, 24, 1) {
	// attend le signal de d�marrage des t�ches
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
	
	// boucle principale de la t�che
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

			// affichage du bargraph repr�sentant les secondes
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
 * T�che de scrutation du clavier
 * 
 * P�riodique, s'exc�utant toute les 1/10e de secondes
 */
AVRX_GCC_TASKDEF(keyb_polling_task, 16, 2) {
	u08 new_state, old_state = 0;
	static Switch_Message msg ;
	
    while (1) {
    	new_state = (~SWITCH) & 0x0f ; 		// lecture de l'�tat des switches
    										// NB : �tat apppuy� = 1 ==> inversion des �tats des pins 
    	
    	if (new_state != old_state) {		// s'il a chang�,
    		// on poste un message indiquant quels sont les changements et l'�tat actuel
    		msg.changes = new_state ^ old_state ;
    		msg.states = new_state ; 		
    		AvrXSendMessage(&kbd_queue, &(msg.mcb)) ;
    		// on attend son acquittement avant de poursuivre pour �viter de l'�craser par le suivant
    		AvrXWaitMessageAck(&(msg.mcb));
    		
    		old_state = new_state ;			// et on m�morise le nouvel �tat
    	}
    	
    	AvrXDelay(&tmr_keyb, 100);			// pause de 100ms avant scrutation suivante

    	// clignotement de la LED du heart beat clavier
    	LED ^= 0x04 ;
    }
}

/*
 * T�che de gestion des �v�nements du clavier
 * 
 * Asynchrone, cadenc�e par les messages envoy�s par la t�che de scrutation
 */
AVRX_GCC_TASKDEF(keyb_handler, 16, 4) {
    MessageControlBlock *p;
    
    // affichage du texte statique pour l'�tat du clavier
    lcdGotoXY(13, 2) ;
    lcdPrintData(MSG4, strlen(MSG4)) ;

	char kb_display[5] = "----" ;
    while (1) {
    	// affichage de l'�tat courant du clavier
        lcdGotoXY(16, 2) ;
        lcdPrintData(kb_display, 4) ;

		// attente du message de signalisation d'un �v�nement clavier
        p = AvrXWaitMessage(&kbd_queue);
        
        // r�cup�ration des donn�es du message (l'�tat du clavier)
        u08 changes  = ((Switch_Message*)p)->changes;
        u08 states  = ((Switch_Message*)p)->states;
        
        // acquittement du message
        AvrXAckMessage(p);
        
        // gestion du nouvel �tat du clavier
        if (changes & 0x01) { 				// appui sur la touche pause
        	if (states & 0x01) { 
	    		AvrXSetSemaphore(&sem) ;	// demande de changement d'�tat de la pause
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
 * T�che de gestion de la pause
 * Bascule l'�tat de la pause � chaque appui sur la touche pause
 * 
 * Asynchrone, cadenc�e par le s�maphore de signalisation d'utilisation de la touche pause
 */ 
AVRX_GCC_TASKDEF(pause_task, 8, 3) {
	while(1) {
		// on attend le s�maphore qui indique qu'une touche a �t� appuy�e 
		AvrXWaitSemaphore(&sem) ;
		
		// bascule de la LED t�moin
		LED ^= 0x02 ;
		// bascule de l'�tat de pause de l'horloge
    	clock_paused ^= 0x01 ;
    	
    	// met � jour l'indication d'�tat sur le LCD
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
    AvrXSetKernelStack(0);		// d�finition du stack du kernel


    MCUCR = 1<<SE;				// autorise l'instruction SLEEP
    TCNT0 = TCNT0_INIT;			// initialise le timer 0 (utilis� pour la gestion des timers
    TCCR0 = TMC8_CK64;			// d�finit son prescaler
    TIMSK = 1<<TOIE0;			// active l'interruption Timer0 Overflow

    LEDDDR = 0xFF;				// configure en sortie le port des LEDs 
    LED   = 0xFF;				// initialise toutes les sortie � 1 => �teint les LEDs 

    SWITCHDDR = 0x00 ;			// le port des switch est configur� en entr�e
    SWITCHPUP = 0xFF ;			// les pullups sont activ�es => appui sur le poussoir = entr�e � 0

	// lancement des diff�rentes t�ches
    AvrXRunTask(TCB(clock_task));	
    AvrXRunTask(TCB(keyb_polling_task));	
    AvrXRunTask(TCB(keyb_handler));	
    AvrXRunTask(TCB(pause_task));	

	// autorise le d�marrage de la t�che horloge
    AvrXSetSemaphore(&sem) ;

    Epilog();                   // bascule sur le contexte de la premi�re t�che
    							// => active l'ensemble
    							
	while(1) ;
}

