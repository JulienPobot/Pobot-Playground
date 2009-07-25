
//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include "i2c.h"		// include timer function library (timing, PWM, etc)

#include "lcd03.h"

#define LCD_ADDR	0xC6	// adresse I2C de l'écran LCD

// local data buffer
unsigned char localBuffer[] = "12345678901234567890";
unsigned char localBufferLength = 0x20;

/**
 *
 */
void lcd03Init(void) 
{		
	localBufferLength = 0;
	//
	localBuffer[localBufferLength++] = 0;		// Registre du LCD
	localBuffer[localBufferLength++] = 19;		// Allume le rétroéclairage LCD	
	localBuffer[localBufferLength++] = 12;		// Nettoie l'écran	
	localBuffer[localBufferLength++] = 1;		// Cursor home
	localBuffer[localBufferLength++] = 6;		// Blinking cursor
	//
	i2cMasterSend(LCD_ADDR, localBufferLength, localBuffer);

}

/**
 *
 */
void lcd03Char(char car)
{
	localBufferLength = 0;
	localBuffer[localBufferLength++] = 0;			
	localBuffer[localBufferLength++] = car;
	//
	i2cMasterSend(LCD_ADDR, localBufferLength, localBuffer);
}

/**
 *
 */
void lcd03Display(char* msg,u08 ligne,u08 colonne)
{
	u08 index = 0;
	localBufferLength = 0;
	//
	localBuffer[localBufferLength++] = 0;			// Registre du LCD	
	localBuffer[localBufferLength++] = 3;			// Déplace le curseur
	localBuffer[localBufferLength++] = ligne;		// --> ligne
	localBuffer[localBufferLength++] = colonne;	// --> colonne
	//
	while (msg[index] != 0) 
	{		
		localBuffer[localBufferLength++] = msg[index++];		
	}	
	//
	i2cMasterSend(LCD_ADDR,  localBufferLength, localBuffer);


}

void lcd03Reset(void) 
{
	localBufferLength = 0;
	//
	localBuffer[localBufferLength++] = 0;		// Registre du LCD
	localBuffer[localBufferLength++] = 12;		// Nettoie l'écran	
	localBuffer[localBufferLength++] = 2;		// Déplace le curseur
	localBuffer[localBufferLength++] = 1;		// --> caractère
	//
	i2cMasterSend(LCD_ADDR, localBufferLength, localBuffer);

}
