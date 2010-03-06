//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "timerx8.h"		// include timer function library (timing, PWM, etc)
#include "i2c.h"		// include I2C	

#define TEMP_ADDRESS	0x90
#define CLOCK_ADDRESS	0xD0
#define MEM_ADDRESS		0xA0

#define DS1307_SEC 0
#define DS1307_MIN 1
#define DS1307_HR 2
#define DS1307_DOW 3
#define DS1307_DATE 4
#define DS1307_MTH 5
#define DS1307_YR 6

// buffer de commande I2C (voir plus loin)
u08 cmd[] = "012345678";	// commande maximale qu'on puisse envoyer aux composants I2C
u08 cmdSize;

u08 rtc[7];
	
int lireTemperature(void);

int lireHeures(void);

int lireMinutes(void);

int lireJour(void);

int lireMois(void);

u08 transformToBcd(u08 val);

u08 transformFromBcd(u08 bcd);

void initClock(void);

int lireStockMemoire(void);

void ecrireStockMemoire(int val);


/**
 *
 */
 
#define FREQUENCE_CAPTURE 	10
  
void timerTemperature(void);

// indice de stockage des températures
int index = 0;

// chronomètre
int count_ms = 0;
int count_sec= 0;
int count_min= 0;

// déclenchement de la capture
int flag_exec_temperature = TRUE;

// lecture UART
char caractere = ' ';

// indice de boucle 
int count = 0;

/* 
 *
 *
 *
 */
int main(void)
{	
	/** (0) phase d'init **/
	
	sbi(DDRB,0);
	cbi(PORTB,0);
	timerInit();
	uartInit();
	uartSetBaudRate(9600);
	i2cInit();
		
	// envoi d'un glitch
	
	sbi(PORTB,0);
	cbi(PORTB,0);
	
	/** (1) lecture de l'indice courant **/
	
	
	sbi(PORTB,0);
	cbi(PORTB,0);
	
	// on peut reprendre l'enregistrement en cas d'interruption
	index = lireStockMemoire();
	uartSendByte(index);
	
	// envoi de deux glitch
	
	sbi(PORTB,0);
	cbi(PORTB,0);
	sbi(PORTB,0);
	cbi(PORTB,0);
	
	/** (2) initialisation du timer **/

	timer0Init();
	timer0SetPrescaler(TIMER_CLK_DIV1024);
	timerAttach(TIMER0OVERFLOW_INT,timerTemperature);
	
	
	/** (3) boucle infinie **/
	
	while (1)
	{	
		// traitement de la capture
		// déclenché par le timer (toutes les FREQUENCE_CAPTURE minutes)
		if (flag_exec_temperature)
		{
			// désactiver
			flag_exec_temperature = FALSE;
				
			sbi(PORTB,0);
			cbi(PORTB,0);
			sbi(PORTB,0);
			cbi(PORTB,0);	
			sbi(PORTB,0);
			cbi(PORTB,0);	
			
			// signaler qu'une nouvelle valeur est disponible
			uartSendByte('n');		
			
			if (index < 255)
			{
				// incrémenter l'indice de stockage
				index++;			
				ecrireStockMemoire(index);
				
				// laisser du temps à la mémoire
				delay_ms(10);
				
				// enregistrer dans la mémoire
				int temp = lireTemperature();
				uartSendByte(temp);
				cmd[0] = index;
				cmd[1] = temp;
				cmdSize = 2;
				i2cMasterSend(MEM_ADDRESS,cmdSize,cmd);			
				delay_ms(2);			
			}
		}		
		
		// traitement de la console série
		
		if (uartReceiveByte(&caractere))
		{			
			switch (caractere)
			{
				case 'c':					
					uartSendByte(lireHeures());
					uartSendByte(lireMinutes());
					uartSendByte(lireJour());
					uartSendByte(lireMois());
					break;
				case 'r':
					ecrireStockMemoire(0);					
					index = 0;
					// effacer toutes les températures
					for (count = 0; count < 250; count++)
					{
						cmd[0] = count;
						cmd[1] = 0xFF;
						cmdSize = 2;
						i2cMasterSend(MEM_ADDRESS,cmdSize,cmd);					
						// il faut laisser du temps à la mémoire
						delay_ms(3);
					}
					break;
				case 'i':
					uartSendByte(index);
					break;
				case 's':
					// lancer un stockage supplémentaire
					flag_exec_temperature = TRUE;
					break;
				case 't':
					uartSendByte(lireTemperature());
					break;
				case 'l':	
					for (count = 0; count <= index; count++)
					{
						cmd[0] = count;
						cmdSize = 1;
						i2cMasterSend(MEM_ADDRESS,cmdSize,cmd);
						i2cMasterReceive(MEM_ADDRESS,1,cmd);
						uartSendByte((int)cmd[0]);					
					}
					// terminer l'envoi
					uartSendByte(0xFF);
					break;
				default:
					uartSendByte(caractere);
					break;
			}
		}
		
	}
	
	return 0;
	
}


/**
 *
 */
void timerTemperature(void)
{
	
	/** (1) réarmer le timer **/
	TCNT0 = 76; // 256 - valeur pour 50 ms 
	
	// incrémenter le compteur des 50 ms
	count_ms++;
	
	// détecter
	if (count_ms >= 20)
	{
		// incrémenter le compteur des secondes
		count_sec++;		
		count_ms = 0;
	}
	
	if (count_sec >= 60)
	{
		// incrémenter le compteur des minutes
		count_min++;
		count_sec = 0;		
	}
	
	if (count_min >= FREQUENCE_CAPTURE)
	{	
		// activer le traitement de la température
		flag_exec_temperature = TRUE;
		count_min = 0;		
	} 		
	
}


/**
 * Lire l'indice courant du stock dans la mémoire de la RTC
 */
int lireStockMemoire(void)
{
	cmd[0] = 0x00;	// première mémoire
	cmdSize = 1;
	//
	i2cMasterSend(MEM_ADDRESS,cmdSize,cmd);
	i2cMasterReceive(MEM_ADDRESS,1,cmd);
	//
	return cmd[0];
}

/** 
 * Ecrire l'indice courant dans la mémoire de la RTC
 */
void ecrireStockMemoire(int val)
{
	cmd[0] = 0x00;	// première mémoire
	cmd[1] = val;
	cmdSize = 2;
	//
	i2cMasterSend(MEM_ADDRESS,cmdSize,cmd);
}

 
/**
 *
 */
int lireHeures(void)
{
	cmd[0] = 2;		// le registre des heures
	cmdSize = 1;
	//
	i2cMasterSend(CLOCK_ADDRESS,cmdSize,cmd);
	i2cMasterReceive(CLOCK_ADDRESS,1,cmd);
	//
	return transformFromBcd(cmd[0]);
}

/**
 *
 */
int lireMinutes(void)
{
	cmd[0] = 1;		// le registre des minutes
	cmdSize = 1;
	//
	i2cMasterSend(CLOCK_ADDRESS,cmdSize,cmd);
	i2cMasterReceive(CLOCK_ADDRESS,1,cmd);
	//
	return transformFromBcd(cmd[0]);
}

/**
 *
 */
int lireJour(void)
{
	cmd[0] = 4;		// le registre des minutes
	cmdSize = 1;
	//
	i2cMasterSend(CLOCK_ADDRESS,cmdSize,cmd);
	i2cMasterReceive(CLOCK_ADDRESS,1,cmd);
	//
	return transformFromBcd(cmd[0]);
}

/**
 *
 */
int lireMois(void)
{
	cmd[0] = 5;		// le registre des minutes
	cmdSize = 1;
	//
	i2cMasterSend(CLOCK_ADDRESS,cmdSize,cmd);
	i2cMasterReceive(CLOCK_ADDRESS,1,cmd);
	//
	return transformFromBcd(cmd[0]);
}


/**
 * Lecture de la température sur le LM75
 */
int lireTemperature(void)
{	
	int multiplicateur; // un multiplicateur pour le signe
	
	u08 msb = 0; // premier octet (poids le plus fort)
	u08 lsb = 0; // second octet (poids le plus faible)
	
	// Lecture I2C
	cmd[0] = 0x00;	// registre 00 	
	cmdSize = 1;	// 1 seul octet à envoyer
	
	// TEMP_ADDRESS = 0x90 car pour AVRlib il faut ajouter le 0 à la fin
	// donc on passe de 100 1000 (0x48) à 1001 0000 (0x90)
	i2cMasterSend(TEMP_ADDRESS,cmdSize,cmd);					
	i2cMasterReceive(TEMP_ADDRESS,2,cmd);
	
	msb = cmd[0];
	lsb = cmd[1];
	
	// Récupération et retrait du bit de signe
	if (msb & 0x80) {
		// en complément à 2, le bit de poids le plus fort à 1 signifie nombre négatif
		multiplicateur = -1;
	} else {
		multiplicateur = 1;
	}
	msb = msb & 0x7F;
	
	// Calcul température (première version, pas de virgule)
	return ((msb << 1)/2) * multiplicateur;
	
}

/** 
 * Transform a binary value into BCD coded value
 *
 */
u08 transformToBcd(u08 val)
{
	// 4 higher bits are the integer result of division by 10
	// 4 lower bits are the remaining part
	return ((val/10) << 4) + val % 10;
}

/** 
 * Decode a BCD coded value into binary value
 *
 */
u08 transformFromBcd(u08 bcd)
{
	return 10*((bcd & 0xF0)>>4) + (bcd & 0x0F);
}
