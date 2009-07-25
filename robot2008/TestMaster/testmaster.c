//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "timer.h"
#include "i2c.h"		// include i2c driver

#define SLAVE_ADDR 0xA0
#define MASTER_ADDR 0xA1

unsigned char localBuffer[] = "12345";
unsigned char localBufferLength = 0x05;

int main(void)
{	
	timerInit();
	i2cInit();		
	i2cSetLocalDeviceAddr(MASTER_ADDR, TRUE);
	
	while (1) {	
		i2cMasterSend(SLAVE_ADDR, localBufferLength, localBuffer);
		delay_ms(1000);
	}
	return 0;
}
