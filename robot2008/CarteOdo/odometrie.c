
//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "encoder_pobot.h"	// include encoder driver
#include "i2c.h"		// include I2C driver
#include "math.h"		// include mathematical operations

#include "string.h"

/** Définition des constantes **/

#define SLAVE_CODEUR_ADDR	0xA1

#ifndef PI2
	#define PI2		(2*M_PI)
#endif

#define	entraxeEnTics	2903,912154


/** Définition des variables **/

// compteurs de la lecture des encodeurs
//s32 dLeft = 0;
//s32 dRight = 0;

typedef struct 
{
	s16 dLeft;
	s16 dRight;
} ODO_MESSAGE, *ODO_MESSAGE_PTR;

volatile ODO_MESSAGE stOdo_Message;

// un flag pour commander l'analyse d'un déplacement
u08 flag_analyse = FALSE;

/** Signature des fonctions **/

void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);

// analyse des compteurs (et calculs d'odométrie)
void analyseOdo(void);

/**
 * Programme principal du robot
 */
int main(void)
{
	
	/** Démarrage et initialisations **/
	
	/* initialisation des flags */
	
	/* initalisation des périphériques utilisés */

	timerInit();
	//
	i2cInit();
	i2cSetLocalDeviceAddr(SLAVE_CODEUR_ADDR, TRUE);
	i2cSetSlaveReceiveHandler( i2cSlaveReceiveService );
	i2cSetSlaveTransmitHandler( i2cSlaveTransmitService );
	//
	encoderInit();
	
	/* initialisation des directions des ports */
	
	
	/* initialisation des ports/bits de sortie */

	// led de test
	sbi(DDRB,5);
	cbi(PORTB,5);
	// pico de test
	sbi(DDRB,4);
	cbi(PORTB,4);
	
	/**  Boucle principale **/

	// mise à zéro
	stOdo_Message.dLeft = 0;
	stOdo_Message.dRight = 0; 
	
	// première analyse au démarrage
	analyseOdo();
	
	//
	while (1) 
	{
		// attendre la demande d'analyse pour faire les calculs
		if (flag_analyse) 
		{			
			analyseOdo();
			flag_analyse = FALSE;
		}	
		
	}	
	return 0;	
}

/**
 *
 */
void analyseOdo(void)
{
	// lire la nouvelle position des encoders
	stOdo_Message.dLeft = encoderGetPosition(0);
	stOdo_Message.dRight = encoderGetPosition(1);		
	
	// remettre à 0
	//encoderSetPosition(0,0);
	//encoderSetPosition(1,0);
	
	
}

/**
 *
 */
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData)
{
	// rien, pas de réception du maitre pour l'instant
}

/**
 *
 */
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData)
{	
	// passage 
	//sbi(PORTB,4);
		
	// copie des données dans le buffer d'envoi I2C
	memcpy(transmitData, (u08*) &stOdo_Message, sizeof(stOdo_Message));
	
	// demander un nouveau calcul (ce qui permet de laisser le plus de temps
	flag_analyse = TRUE;
	
	// baisser le pico de test oscillo
	//cbi(PORTB,4);
	// retour du nombre de caractères à transmettre
	return sizeof(stOdo_Message);
}
