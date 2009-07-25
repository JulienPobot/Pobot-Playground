/**
 * @author Julien Holtzer pour Association Pobot 2008
 * @version 1.0 (07/12/2007) : création du programme
 */
 
//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <math.h>

#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "i2c.h"		// include i2c function library
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "vt100.h"		// include vt100 terminal support
#include "cmdline.h"	// include cmdline function library

//

#define SENS_AVANT		1
#define SENS_ARRIERE	0

#define LOCAL_ADDR	0xA0
#define TARGET_ADDR	0xA1
//

u08 Run;

s16 vitG;
s16 vitD;

int cv_gauche;
int cv_droit;
int cs_gauche;
int cs_droit;

unsigned char localBuffer[] = "Pascal is cool!!Pascal is Cool!!";
unsigned char localBufferLength = 0x20;


// Fonctions

void exitFunction(void);
void helpFunction(void);
void setVitesseFunction(void);
void getVitesseFunction(void);
void envoiFunction(void);

void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);

//

int main(void)
{
	u08 c;

	timerInit();	
	uartInit();
	uartSetBaudRate(9600);
	rprintfInit(uartSendByte);
	vt100Init();
	cmdlineInit();
	
	i2cInit();
	sbi(PORTC, 0); 
	sbi(PORTC, 1);
	
	i2cSetLocalDeviceAddr(LOCAL_ADDR, TRUE);
	i2cSetSlaveTransmitHandler( i2cSlaveTransmitService );
	i2cSetSlaveReceiveHandler( i2cSlaveReceiveService );
	
	cmdlineInit();
	cmdlineSetOutputFunc(uartSendByte);
	cmdlineAddCommand("exit",		exitFunction);
	cmdlineAddCommand("help",		helpFunction);
	cmdlineAddCommand("setVitesse",	setVitesseFunction);
	cmdlineAddCommand("getVitesse",	getVitesseFunction);
	cmdlineAddCommand("envoi",	envoiFunction);

	vt100ClearScreen();
	vt100SetCursorPos(1,0);
	rprintfProgStrM("\r\nTelecommande Pobot 2008\r\n");
	
	cmdlineInputFunc('\r');

	// set state to run
	Run = TRUE;

	// main loop
	while(Run)
	{
		// pass characters received on the uart (serial port)
		// into the cmdline processor
		while(uartReceiveByte(&c)) cmdlineInputFunc(c);

		// run the cmdline execution functions
		cmdlineMainLoop();
		
	}

	rprintfCRLF();
	rprintf("Programme terminé !\r\n");
	return 0;	// initialize the timer library
}

/**
 *
 */
void exitFunction(void)
{
	// to exit, we set Run to FALSE
	Run = FALSE;
}

/**
 *
 */
void helpFunction(void)
{
	rprintfCRLF();
	rprintf("Commandes disponibles :\r\n");
	rprintf("help      - affiche les commandes\r\n");
	rprintf("setVitesse   - change la vitesse des 2 moteurs\r\n");
	rprintf("getVitesse   - affiche la vitesse des 2 moteurs\r\n");
	rprintf("envoi      - envoi les vitesses\r\n");
	rprintfCRLF();
}


/**
 *
 */
void setVitesseFunction(void)
{
	vitG = cmdlineGetArgInt(1);
	vitD = cmdlineGetArgInt(2);
	if (vitG < 0) {
		cs_gauche = SENS_AVANT;
	} else {
		cs_gauche = SENS_ARRIERE;
	}
	if (vitD < 0) {
		cs_droit = SENS_AVANT;
	} else {
		cs_droit = SENS_ARRIERE;
	}
	
	cv_gauche = fabs(vitG);
	cv_droit = fabs(vitD);
}


/**
 *
 */
void getVitesseFunction(void)
{
	rprintf("\tvitesse G :");
	rprintfNum(10, 4, TRUE, ' ', vitG); 	
	rprintfCRLF();
	rprintf("\tvitesse D :");
	rprintfNum(10, 4, TRUE, ' ', vitD); 
	rprintfCRLF();
}


/**
 *
 */
void envoiFunction(void)
{
	rprintf("\tenvoi i2c");
	rprintfCRLF();
	i2cMasterSend(TARGET_ADDR, localBufferLength, localBuffer);
}

/**
 * 
 */
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData)
{
	rprintf("\ti2c receive service");
	rprintfCRLF();
	Run = FALSE;
}

u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData)
{
	rprintf("\ti2c transmit service");
	rprintfCRLF();
	return transmitDataLengthMax;
}
