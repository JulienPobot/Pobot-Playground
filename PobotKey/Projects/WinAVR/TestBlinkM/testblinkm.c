//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "i2c.h"

/*******************************/
/** Définition des constantes **/
/*******************************/

/**
 * Variables
 */
 
u08 cmd[] = "012345";	// commande maximale qu'on puisse envoyer aux blinkm
u08 cmdSize;

u08 buffer[] = "01234";

int hue = 0;

/**
 * Fonctions
 */
int main(void)
{
	// timer
	timerInit();
	i2cInit();
	
	// initialisation des I/O
	sbi(DDRD,3);	// led verte en sortie
	sbi(PORTD,3);	// éteindre la led
	cbi(DDRD,2);	// bouton d'entrée
	cbi(PORTD,2);
	
	// initialisation des blinkm
	
	// arrêter le script
	
	cmd[0] = 'o';
	cmdSize = 1;
	i2cMasterSend(0x00,cmdSize,cmd);
	
	cmd[0] = 'c';
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmdSize = 4;
	i2cMasterSend(0x00,cmdSize,cmd);

	delay_ms(1000);
	
	cmd[0] = 'f';
	cmd[1] = 20;
	cmdSize = 2;
	i2cMasterSend(0x00,cmdSize,cmd);
	
	/**  Boucle principale **/
	
	//
	while (1) 
	{
		cbi(PORTD,3);
		//
		cmd[0] = 'h';
		cmd[1] = hue;
		cmd[2] = 0xFF;
		cmd[3] = 0xFF;
		cmdSize = 4;
		i2cMasterSend(0x00,cmdSize,cmd);
		//
		if (hue < 1000)
		{
			delay_ms(hue);
		} else {
			delay_ms(700);
		}
		sbi(PORTD,3);
		//
		cmd[0] = 'c';
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmdSize = 4;
		i2cMasterSend(0x00,cmdSize,cmd);
		//
		delay_ms(500);
		
		// interrogation du détecteur de distance
		cmd[0] = 0x42; // read distance
		cmdSize = 1; 
		i2cMasterSend(0x02,cmdSize,cmd);
		
		i2cMasterReceive(0x03,2,buffer);
		hue = ( 0x00FF & buffer[0] );
        hue += ( (0x00FF & buffer[1]) <<8 );
		
	}	
	return 0;	
}

