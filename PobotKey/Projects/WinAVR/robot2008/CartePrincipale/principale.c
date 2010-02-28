//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include "global.h"		// include our global settings
#include "timer.h"
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "vt100.h"		// include vt100 terminal support
#include "i2c.h"		// include i2c function library
#include "cmdline.h"	// include cmdline function library

#include "string.h"
#include "math.h"

#include "lcd03.h"		// include lcd03
#include "Fuzzy.h"		// include Fuzzy

/** Définition des constantes **/

#ifndef PI2
	#define PI2		(2*M_PI)
#endif

#define SLAVE_CODEUR_ADDR	0xA1
#define SLAVE_MOTEURS_ADDR	0xA2

#define SERVO1_DDR		DDRB
#define SERVO1_PORT		PORTB
#define SERVO1_PIN		PB0

#define SERVO2_DDR		DDRB
#define SERVO2_PORT		PORTB
#define SERVO2_PIN		PB1

#define ticksParCm			155,2139635
#define entraxeEnTicks		2281,645264


/** Définition des variables **/

// local I2C data buffer
unsigned char localI2CBuffer[] = "12345678901234567890";
unsigned char localI2CBufferLength = 0x20;

int count_ms = 0;
int count_forsec = 0;
int count_sec = 0;

int flag_boucle = FALSE;
int flag_sec = FALSE; // pour le test I2C

int lcd_ligne = 0;
int lcd_colonne = 0;

unsigned char lcd_ligne1[] = "Pobot 2008       H00";
unsigned char lcd_ligne2[] = "X:   |Y:   |T:    | ";
unsigned char lcd_ligne3[] = "VG:    |VD:   |     ";
unsigned char lcd_ligne4[] = "Err: 0              ";

// première version int pos_servo[] = {0x6C,0x6C,0x6C,0x6C};

// consignes des servos.. au neutre par défaut
u16 pos_servo[] = {3000,3000,3000,3000};

// Les consignes de vitesses (signées entre -250 et +250)
// envoyées par I2C à la carte moteurs dans la boucle principale de 50 ms
// (dès le démarrage du robot, et même à l'arrêt)
s16 consigne_vitesse_gauche = 0;
s16 consigne_vitesse_droite = 0;

// les valeurs envoyées par les odomètres
u08 encoder_left = 0;
u08 encoder_right = 0;


typedef struct 
{
	s16 dLeft;
	s16 dRight;
} ODO_MESSAGE, *ODO_MESSAGE_PTR;

volatile ODO_MESSAGE stOdo_Message;

// position du robot
double robotX = 0;
double robotY = 0;
double robotDelta = 0;

/** Signature des fonctions **/

void interruptionTimer(void);

// utilitaires
void glitch(void);
void erreur(void);

// communication avec l'utilisateur
void cmdServoFunction(void);
void cmdMoteursFunction(void);
void cmdOdometresFunction(void);

// boucle des traitements 
void inline travailCyclique(void);

// calculs d'odométrie
void inline calculPosition(void);

// gestion des servomoteurs
void inline gestionServos(void);

// lecture I2C des informations des odomètres
void inline readOdometres(void);

// envoi I2C des consignes de vitesse et sens à la carte moteur
void inline sendConsignesVitesses(void);

// envoi I2C du caractère suivant dans le parcours de la matrice 4x
void afficheNextChar(void);
// test de la liaison I2C
void testLiaisonI2C(void);

/**
 * Programme principal du robot
 */
int main(void)
{	
	u08 i; // compteur pour tests
	u08 c; // cmdline
	
	/** Démarrage et initialisations **/
	
	/* initialisation des flags */
	
	/* initalisation des périphériques utilisés */
	
	timerInit();
	i2cInit();	
	uartInit();
	uartSetBaudRate(9600);
	rprintfInit(uartSendByte);
	vt100Init();
	cmdlineInit();

	
	//
	lcd03Init();
	lcd03Display("reset permanent",1,1);
	lcd03Reset();
	
	//
	cmdlineInit();	
	cmdlineSetOutputFunc(uartSendByte);
	cmdlineAddCommand("servo",	cmdServoFunction);
	cmdlineAddCommand("moteurs", cmdMoteursFunction);
	cmdlineAddCommand("odo", cmdOdometresFunction);
	vt100ClearScreen();
	vt100SetCursorPos(1,0);
	rprintfProgStrM("\r\nTelecommande Pobot 2008\r\n");	
	cmdlineInputFunc('\r');
	
	// initialize the timer system 
	// pour la boucle d'action
	timer1Init();
	timer1SetPrescaler(TIMER_CLK_DIV64);
	timerAttach(TIMER1OVERFLOW_INT,interruptionTimer);	
		
	/* initialisation des directions des ports */
	
	sbi(DDRA,0);	// ligne de glitch
	sbi(DDRA,7);	// led d'erreur
	sbi(DDRA,1);	// led de seconde
	
	sbi(DDRA,2);	// ligne de test pour l'interrupt
	
	// les servos
	sbi(SERVO1_DDR,SERVO1_PIN);
	sbi(SERVO2_DDR,SERVO2_PIN);
	
	/* initialisation des ports/bits de sortie */
	
	cbi(PORTA,0); // la ligne de glitch est à 0 (note, la led sera donc allumée)
	sbi(PORTA,7); // on éteint la led d'erreur
	sbi(PORTA,1); // on éteint la led de seconde
	cbi(PORTA,2); // la ligne de test de l'interrupt est à 0 (led allumée)

	cbi(SERVO1_PORT,SERVO1_PIN);
	cbi(SERVO2_PORT,SERVO2_PIN);
	
	/**  Boucle principale **/

	while (1) 
	{	
	
		if (flag_boucle) 
		{
			// appel des traitements toutes les 50 ms
			travailCyclique();
			
			// terminé, on peut attendre le tour suivant
			// on baisse le flag
			flag_boucle = FALSE;
			
		} else {		
			
			// si on a du temps...
			
			/** Mise à jour de l'écran LCD **/
			// c'est une procédure particulière pour traiter l'écran LCD
			afficheNextChar();
			
			// dialogue console
			while(uartReceiveByte(&c)) {
				cmdlineInputFunc(c);
			}
			// run the cmdline execution functions
			cmdlineMainLoop();
			
		}
	}
	
	return 0;
}

/**
 *
 */
void inline travailCyclique(void)
{
	sbi(PORTA,0); // pour vérifier à l'oscillo les 50 ms
	
	/** lecture des capteurs **/
	
	/* lecture bus I2C */
	readOdometres();
	
	calculPosition();
	
	// lecture I/O
	
	/** tests & comparaisons **/
	
	/** choix d'une stratégie **/
	
	// logique floue
	
	/** commande des actionneurs **/
	
	/* lecture I2C : commande de vitesses */
	sendConsignesVitesses();

	/* servos */
	
	gestionServos();
	
	/** Traitement des secondes **/
		
	// un overflow vient d'arriver, donc 50 ms est écoulée
	count_ms++;
	// test des cas particuliers
	if (count_ms == 20) 
	{
		count_ms = 0;
		// 1 seconde = 20 * 50 ms
		count_sec++;
		// faire flipper la led de seconde
		PORTA ^= 0x02; // 0010 = bit 1			
		// mettre à jour le chrono
		lcd_ligne1[19]++;
		if (lcd_ligne1[19] == ':') 
		{
			lcd_ligne1[18]++;
			lcd_ligne1[19] = '0';
		}
	}
	
	// Fin de la boucle cyclique de traitements
	
	cbi(PORTA,0);
}


/** Fonctions des interruptions **/

/**
 * Interruption sur overflow du compteur
 * - timer : timer1 (16 bits)
 * - prescaler : 64
 * - overflow : 12500 pas d'horloge à compter à partir de 0xCF2B = 50 ms
 */
void interruptionTimer(void)
{		
	// réarmer le timer à 50 ms : 0xCF2B
	TCNT1H = 0xCF;
	TCNT1L = 0x2B;
	
	//sbi(PORTA,2); // ligne de test allumée
	
	// il faut déclencher la boucle de traitement
	// on utilise un flag à réarmer, mais on va d'abord tester 
	// si le traitement précédent s'est bien déroulé
	if (flag_boucle) {
		// erreur, le flag n'a pas été remis à zéro
		// a priori on a dépassé le temps d'exécution
		erreur();
	} else {
		// tout va bien, la boucle a remis le flag à zéro
		flag_boucle = TRUE;
	}
	
	
	//cbi(PORTA,2); // ligne de test éteinte
}

/** Les fonctions utilitaires **/

/** 
 * Une impulsion sur une patte pour le suivi à l'oscillo
 */
void glitch(void)
{
	sbi(PORTA,0);
	cbi(PORTA,0);
}

/** 
 * Une erreur a eu lieu, il faut allumer la led
 */ 
void erreur(void)
{
	cbi(PORTA,7);
	//
	lcd_ligne4[19] = 'V';
}

/**
 * Affiche le prochain caractère sur le LCD via I2C
 */
void afficheNextChar(void)
{
	// affichage du LCD
	if (lcd_ligne == 0) 
		lcd03Char(lcd_ligne1[lcd_colonne]);
	
	if (lcd_ligne == 1) 
		lcd03Char(lcd_ligne2[lcd_colonne]);
	
	if (lcd_ligne == 2) 
		lcd03Char(lcd_ligne3[lcd_colonne]);
	
	if (lcd_ligne == 3) 
		lcd03Char(lcd_ligne4[lcd_colonne]);
	
	// mise à jour de la ligne/colonne
	lcd_colonne++;
	
	// overflow
	if (lcd_colonne == 20) 
	{
		lcd_colonne = 0;
		lcd_ligne++;
		if (lcd_ligne == 4) 
		{
			lcd_ligne = 0;
		}	
	}

}

void testLiaisonI2C(void)
{
	localI2CBufferLength = 0;
	//
	localI2CBuffer[localI2CBufferLength++] = 'A';		// un octet 
	//
	i2cMasterSend(SLAVE_CODEUR_ADDR, localI2CBufferLength, localI2CBuffer);


}

void cmdServoFunction(void)
{
	pos_servo[0] = cmdlineGetArgInt(1);
	pos_servo[1] = cmdlineGetArgInt(2);
}



void cmdMoteursFunction(void)
{
	consigne_vitesse_gauche = cmdlineGetArgInt(1);
	consigne_vitesse_droite = cmdlineGetArgInt(2);
}


void cmdOdometresFunction(void)
{
	rprintfCRLF();
	rprintf("Odo gauche : ");
	rprintfNum(10, 10, TRUE, ' ', stOdo_Message.dLeft); 
	rprintfCRLF();
	rprintf("Odo droite : "); 
	rprintfNum(10, 10, TRUE, ' ', stOdo_Message.dRight); 
	rprintfCRLF();
	rprintfCRLF();
	
}

void inline sendConsignesVitesses(void)
{
	// initialisation du buffer à envoyer
	localI2CBufferLength = 0;
	// remplissage
	localI2CBuffer[localI2CBufferLength++] = fabs(consigne_vitesse_gauche);
	// un octet pour la vitesse du moteur gauche
	localI2CBuffer[localI2CBufferLength++] = (consigne_vitesse_gauche < 0) ? 1:0;	
	// un octet pour le sens du moteur gauche
	localI2CBuffer[localI2CBufferLength++] = fabs(consigne_vitesse_droite);
	// un octet pour la vitesse du moteur droit
	localI2CBuffer[localI2CBufferLength++] = (consigne_vitesse_droite < 0) ? 1:0;	
	// un octet pour le sens du moteur droit
	// envoi I2C
	i2cMasterSend(SLAVE_MOTEURS_ADDR, localI2CBufferLength, localI2CBuffer);
}

/**
 * Gestion des servomoteurs
 *  - on n'utilise pas d'interruption
 * 	- on boucle jusqu'à avoir parcouru le créneau
 */ 
void inline gestionServos(void)
{
	u08 subcompteur;
	u16 compteur;
	
	/* PREMIER SERVOMOTEUR : PINCE GAUCHE */
	
	// monter la ligne	
	sbi(SERVO1_PORT,SERVO1_PIN);	
	
	// boucle naive : attendre selon la valeur considérée
	for (compteur = 0; compteur < pos_servo[0]; compteur++)
	{
		// rien : on utilise le coût en cycles de la boucle "for" 
		// pour attendre entre 1ms et 2ms selon la position du servo.
	}
	
	// désarmer la ligne
	cbi(SERVO1_PORT,SERVO1_PIN);

	/* SECOND SERVOMOTEUR : PINCE GAUCHE */
	
	// monter la ligne	
	sbi(SERVO2_PORT,SERVO2_PIN);	
	
	// boucle naive : attendre selon la valeur considérée
	for (compteur = 0; compteur < pos_servo[1]; compteur++)
	{
		// rien : on utilise le coût en cycles de la boucle "for" 
		// pour attendre entre 1ms et 2ms selon la position du servo.
	}
	
	// désarmer la ligne
	cbi(SERVO2_PORT,SERVO2_PIN);

		
}

/**
 *
 */
void inline readOdometres(void)
{
	i2cMasterReceive(SLAVE_CODEUR_ADDR, sizeof(stOdo_Message), localI2CBuffer);
	
	// copie des données dans le buffer d'envoi I2C
	memcpy((u08*)&stOdo_Message, localI2CBuffer, sizeof(stOdo_Message));
	
		
	
}

/**
 * Calculs d'odométrie
 */
void inline calculPosition(void)
{

	double dAngle = 0;
	double dPos = 0;
	double dX = 0;
	double dY = 0;
	
	// variation de l'angle
	dAngle = (stOdo_Message.dRight-stOdo_Message.dLeft)/2.0;   
	// variation du déplacement du robot
	dPos = (stOdo_Message.dRight+stOdo_Message.dLeft)/2.0;   
	
	//conversion en radian
	robotDelta += dAngle / entraxeEnTicks;
	if (robotDelta > M_PI) {
		robotDelta -= PI2;
	}
	if (robotDelta < -M_PI) {
		robotDelta += PI2;
	}
	
	
	//calcul des décalages selon X et Y
	dX = cos(robotDelta) * dPos;
	dY = sin(robotDelta) * dPos;
	
	

	//conversion de la position en mètre
	robotX += dX / ticksParCm;
	robotY += dY / ticksParCm;

}
