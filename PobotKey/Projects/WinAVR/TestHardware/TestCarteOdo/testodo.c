//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include "global.h"		// include our global settings
#include "timer.h"
#include "i2c.h"		// include i2c function library

#include "math.h"
#include "stdio.h"
#include "string.h"

#include "lcd03.h"		// include lcd03

/** Définition des constantes **/

#define SLAVE_CODEUR_ADDR	0xA1

/** Définition des variables **/

// local data buffer
unsigned char localBuffer[] = "01234567";
unsigned char localBufferLength = 0x20;

char* msg = "testtesttest";

// Structure d'échange odométrie
typedef struct 
{
	float dLeft;
	float dRight;
} ODO_MESSAGE, *ODO_MESSAGE_PTR;

volatile ODO_MESSAGE	stOdo_Message;


/** Signature des fonctions **/


/**
 * Programme principal du robot
 */
int main(void)
{	
	
	/** Démarrage et initialisations **/
	
	/* initialisation des flags */
	
	/* initalisation des périphériques utilisés */
	
	timerInit();
	i2cInit();	

	
	//
	lcd03Init();
	lcd03Display("reset permanent",1,1);
	lcd03Reset();
	
	stOdo_Message.dLeft = 0;
	stOdo_Message.dRight = 0;
	//

	/* initialisation des directions des ports */
	
	
	/* initialisation des ports/bits de sortie */
	
	
	/**  Boucle principale **/

	while (1) 
	{
		
		
		i2cMasterReceive(SLAVE_CODEUR_ADDR, sizeof(stOdo_Message), localBuffer);
		// afficher
		lcd03Display(localBuffer,2,2);
		// afficher
		sprintf(msg,"G: %d | D: %d",stOdo_Message.dLeft,stOdo_Message.dLeft);
		lcd03Display(msg,1,1);
		
		// attendre 50 ms
		timerPause(1000);
	}
	
	return 0;
}


/** Les fonctions utilitaires **/

