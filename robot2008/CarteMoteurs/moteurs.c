//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "i2c.h"		// include I2C driver

#include "vitesses.h"	// tables des vitesses


/** Définition des constantes **/

#define SLAVE_MOTEURS_ADDR	0xA2


#define DDR_MOT_GAUCHE		DDRB
#define DDR_MOT_DROITE		DDRB
#define DDR_ENA_GAUCHE		DDRD
#define DDR_ENA_DROITE		DDRD
#define DDR_DIR_GAUCHE		DDRB
#define DDR_DIR_DROITE		DDRD

#define PORT_MOT_GAUCHE		PORTB
#define PORT_MOT_DROITE		PORTB
#define PORT_ENA_GAUCHE		PORTD
#define PORT_ENA_DROITE		PORTD
#define PORT_DIR_GAUCHE		PORTB
#define PORT_DIR_DROITE		PORTD

#define PIN_MOT_GAUCHE		PB1
#define PIN_MOT_DROITE		PB2
#define PIN_ENA_GAUCHE		PD5
#define PIN_ENA_DROITE		PD7
#define PIN_DIR_GAUCHE		PB0
#define PIN_DIR_DROITE		PD6

/** Définition des variables **/

// consignes de vitesses et de sens des deux moteurs
u16 cv_gauche = 0;
u08 cs_gauche = 0;
u16 cv_droite = 0;
u08 cs_droite = 0;

// vitesses et sens des deux moteurs
u16 v_gauche = 0;
u08 s_gauche = 0;
u16 v_droite = 0;
u08 s_droite = 0;

/** Signature des fonctions **/

// Interruptions timers
void interruptGauche(void);	// déclenchement d'un front sur le driver du moteur gauche
void interruptDroite(void);	// déclenchement d'un front sur le driver du moteur droit

/* I2C */

// quand le master veut envoyer une donnée au slave
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);

// quand le master veut recevoir une donnée au slave
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);

/**
 * Programme des moteurs du robot 2008
 */
int main(void)
{
	/** Démarrage et initialisations **/
	
	/* initialisation des flags */
	
	/* initalisation des périphériques utilisés */
	
	timerInit();

		
	// le timer 0 pour le moteur gauche
	
	timer0Init();
	timer0SetPrescaler(TIMER_CLK_DIV256);
	timerAttach(TIMER0OVERFLOW_INT,interruptGauche);
		
	// le timer 2 pour le moteur droit
	timer2Init();
	timer2SetPrescaler(TIMERRTC_CLK_DIV256);
	timerAttach(TIMER2OVERFLOW_INT,interruptDroite);
	
	// I2C
	i2cInit();
	i2cSetLocalDeviceAddr(SLAVE_MOTEURS_ADDR, TRUE);
	i2cSetSlaveReceiveHandler( i2cSlaveReceiveService );
	i2cSetSlaveTransmitHandler( i2cSlaveTransmitService );
	
	
	/* initialisation des directions des ports */
	
	// pulse moteurs
	sbi(DDR_MOT_GAUCHE,PIN_MOT_GAUCHE);
	sbi(DDR_MOT_DROITE,PIN_MOT_DROITE);
	// enable moteurs
	sbi(DDR_ENA_GAUCHE,PIN_ENA_GAUCHE);
	sbi(DDR_ENA_DROITE,PIN_ENA_DROITE);
	// sens moteurs (direction)
	sbi(DDR_DIR_GAUCHE,PIN_DIR_GAUCHE);
	sbi(DDR_DIR_DROITE,PIN_DIR_DROITE);
	
	
	/* initialisation des ports/bits de sortie */
	
	// pas de pulse
	cbi(PORT_MOT_GAUCHE,PIN_MOT_GAUCHE);
	cbi(PORT_MOT_DROITE,PIN_MOT_DROITE);	
	// enable par défaut
	sbi(PORT_ENA_GAUCHE,PIN_ENA_GAUCHE);
	sbi(PORT_ENA_DROITE,PIN_ENA_DROITE);
	// sens par défaut
	sbi(PORT_DIR_GAUCHE,PIN_DIR_GAUCHE);
	sbi(PORT_DIR_DROITE,PIN_DIR_DROITE);
	
	// led de test = led d'erreur
	sbi(DDRD,3);
	sbi(PORTD,3);
	
	// pico de test = ligne pour l'oscillo
	sbi(DDRD,4);
	cbi(PORTD,4);
	
	// pico de test "I2C" : ligne pour l'oscillo
	sbi(DDRB,5);
	cbi(PORTB,5);

	// init de test
	
	// 2 vitesses identiques dans un sens
	cs_gauche = 1;
	cs_droite = 1;		
	cv_gauche = 0;
	cv_droite = 0;

	/**  Boucle principale **/
	while (1) 
	{
		/*
		cs_gauche = 1;
		cs_droite = 1;		
		cv_gauche = 50;
		cv_droite = 50;
		timerPause(3000);
		//
		
		cs_gauche = 0;
		cs_droite = 0;		
		cv_gauche = 150;
		cv_droite = 150;
		timerPause(3000);
		*/
	}
	

	return 0;
}

/**
 *
 */
void interruptGauche(void)
{
	// fin de l'interrupt (amorcé précédemment)		
	TCNT0 = table_vitesse[v_gauche];
	
	// monter la ligne de test oscillo
	sbi(PORTD,4);
	
	// commander le moteur (pulse)	
	if (cv_gauche != 0 || v_gauche != cv_gauche) 
	{
		// on ne fait le pulse que si la vitesse n'est pas nulle pour une consigne nulle
		// (consigne de vitesse à 0 et consigne atteinte)
		
		// changement d'état sur port PB1 = pulse moteur droite
		PORTB ^= 0x02; 
		
	}		
	
	// cas 0 : on est déjà à la consigne (et dans le bon sens bien sûr)
	if (v_gauche == cv_gauche && s_gauche == cs_gauche)
	{
		// descendre la ligne de test oscillo
		cbi(PORTD,4);
		return;
	}
	
	// mettre à jour la vitesse et le sens
	
	// cas 1 : on est dans le bon sens
	if (s_gauche == cs_gauche) 
	{
		// cas a : il faut monter
		if (v_gauche < cv_gauche)
		{			
			v_gauche ++;
		}
		
		// cas b : il faut descendre
		if (v_gauche > cv_gauche)
		{
			v_gauche --;
		}
	} 
	else		
	// cas 2 : pas le bon sens
	{
		// cas a : on a atteint le milieu, on change le sens
		if (v_gauche == 0) 
		{
			s_gauche = cs_gauche;
			if (s_gauche == 1) {			
				sbi(PORT_DIR_GAUCHE,PIN_DIR_GAUCHE);
			} else {
				cbi(PORT_DIR_GAUCHE,PIN_DIR_GAUCHE);
			}
			v_gauche ++;
			
		} else {			
			// cas b : on descend la vitesse
			v_gauche--;		
		}
	}

	// descendre la ligne de test oscillo
	cbi(PORTD,4);
}

/**
 *
 */
void interruptDroite(void)
{

	// fin de l'interrupt (amorcé précédemment)		
	TCNT2 = table_vitesse[v_droite];
	
	// monter la ligne de test oscillo
	sbi(PORTD,4);
	
	// commander le moteur (pulse)	
	if (cv_droite != 0 || v_droite != cv_droite) 
	{
		// on ne fait le pulse que si la vitesse n'est pas nulle pour une consigne nulle
		// (consigne de vitesse à 0 et consigne atteinte)
		
		
		// changement de sens sur port PB2 = pulse moteur droite
		PORTB ^= 0x04; 
	
		
	}		
	
	// cas 0 : on est déjà à la consigne (et dans le bon sens bien sûr)
	if (v_droite == cv_droite && s_droite == cs_droite)
	{
		// descendre la ligne de test oscillo
		cbi(PORTD,4);
		return;
	}
	
	// mettre à jour la vitesse et le sens
	
	// cas 1 : on est dans le bon sens
	if (s_droite == cs_droite) 
	{
		// cas a : il faut monter
		if (v_droite < cv_droite)
		{			
			v_droite ++;
		}
		
		// cas b : il faut descendre
		if (v_droite > cv_droite)
		{
			v_droite --;
		}
	} 
	else		
	// cas 2 : pas le bon sens
	{
		// cas a : on a atteint le milieu, on change le sens
		if (v_droite == 0) 
		{
			s_droite = cs_droite;
			if (s_droite == 1) {			
				sbi(PORT_DIR_DROITE,PIN_DIR_DROITE);
			} else {
				cbi(PORT_DIR_DROITE,PIN_DIR_DROITE);
			}
			
			v_droite ++;
		} else {			
			// cas b : on descend la vitesse
			v_droite--;		
		}
	}
	
	// monter la ligne de test oscillo
	cbi(PORTD,4);
	
}


/**
 *
 */
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData)
{
	sbi(PORTB,5);
	if (receiveDataLength == 4) {
		cv_gauche = receiveData[0];
		cs_gauche = receiveData[1];
		cv_droite = receiveData[2];
		cs_droite = receiveData[3];
		// acknowledge : inverse la led
	} else {
	}
	cbi(PORTB,5);
}

/**
 *
 */
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData)
{
	// pas d'envoi au maitre
	return 0;
}
