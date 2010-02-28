//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"
#include "i2c.h"		// include i2c driver
#include "uart.h"
#include "vt100.h"
#include "rprintf.h"

#include "lcd03.h"

#define SLAVE_ADDR 0xA1

void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);

int flag_traite = FALSE;

int main(void)
{
	timerInit();	
	
	//
	i2cInit();
	i2cSetLocalDeviceAddr(SLAVE_ADDR, TRUE);	
	i2cSetSlaveReceiveHandler(i2cSlaveReceiveService);
	
	// pin D3 en sortie (led)
	sbi(DDRD,2);
	sbi(DDRD,3);
	// mettre à 0 la ligne (led allumée)
	// ou mettre à 1 la ligne (led éteinte)
	sbi(PORTD,3);	
	
	while (1) {

		sbi(PORTD,2);
		cbi(PORTD,2);
		
	} 
	return 0;
}

void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData)
{
	// inverser la led
	//PORTD ^= 0x08;
	
	
}
