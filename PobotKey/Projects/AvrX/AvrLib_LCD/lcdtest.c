/*****************************************************************************
 * lcdtest.c
 * 
 * D�monstration AvrX et AVRlib
 * 
 * Exemple d'utilisation conjointe d'AvrX avec AVRlib
 * 
 * Cette application affiche un horloge avec bargraph sur un afficheur LCD 4x20,
 * en utilisant plusieurs t�ches AvrX, ainsi qu'une communication par s�maphore.
 * 
 * Les t�ches s'ex�cutant sont :
 * - la t�che horloge, qui g�re le d�compte du temps et l'affiche sur le LCD
 * - la t�che de scrutation du clavier (bouton poussoir de mise en pause)
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

#include "hardware.h"			// configuration hardware (assignation des ports,...)

char MSG1[] = "Hello AVRlib" ;
char MSG2[] = "Running..." ;
char MSG3[] = "Paused    " ;
char CLK_TEMPLATE[] = "  :  :  " ;

#include "lcd.h"				// module de gestion LCD texte (AVRlib)

//-----------------------------------------------------------------
int main(void)
{
	// initialise le LCD
	lcdInit();
	lcdLoadCustomChar((u08*)LcdCustomChar,LCDCHAR_FORWARDARROW,7);

	lcdClear() ;
	lcdHome() ;

	// affiche les textes statiques
	lcdPrintData(MSG1, strlen(MSG1)) ;
   
	while(1) ;
}

