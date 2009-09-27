//*****************************************************************************
//
// File Name	: 'main.c'
// Title		: Main Communication
// Author		: guileukow
// Created		: 2009-09-20
// Revised		: 2009-09-20
// Version		: 1.0
// Target MCU	: Atmel AVR series ATMEGA8515
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <inttypes.h>
#include "usart.h"
#include "OptiMouse.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))

int main(void)
{
	usart_init(UBRR_8_19200);
	OptiMouseInit();
	char cCmd = 0;
	uint8_t nTmp = 0;
	uint16_t nCount = 0;
	usart_putc('>');
	while (1) 
	{
		if(is_char_available())
		{			
			cCmd = usart_getc();
			switch(cCmd)
			{
				case 'X':
					usart_putc((unsigned char) readRegister(0x03)); // x
					usart_putc('x');
					break;
				case 'Y':
					usart_putc((unsigned char) readRegister(0x02)); // y
					usart_putc('y');
					break;
				case 'P':
					nTmp = readRegister(0x00); // backup the configuration register
					SET_BIT(nTmp, 0); // Force the LED to the "Always Awake" mode
					writeRegister(0x00, nTmp); // Write the configuration register
					writeRegister(0x08, 0x05); // write anything to the Pixel Data register
					nCount = 0;
					while(nCount < 325)
					{
						nTmp = readRegister(0x08); // read the dumped pixel
						if(bit_is_set(nTmp, 6))
						{
							if(nCount <324)
								usart_putc((unsigned char) ((nTmp & 0x3f) + 48));
							nCount++;
						}
					}
					//nTmp = readRegister(0x08); // read the dumped pixel			
					if(bit_is_set(nTmp, 7))
						usart_putc(';');
					else
						usart_putc('!');
					nTmp = readRegister(0x00); // backup the configuration register
					CLEAR_BIT(nTmp, 0); // Release the LED from the "Always Awake" mode
					writeRegister(0x00, nTmp); // Write the configuration register							
					break;
				case 'S':
					nTmp = readRegister(0x00); // backup the configuration register
					SET_BIT(nTmp, 0); // Force the LED to the "Always Awake" mode
					writeRegister(0x00, nTmp); // Write the configuration register
					nTmp = readRegister(0x08); // read the dumped pixel
					usart_putc((nTmp & 0x01) + 48);
					break;
				case 'R':
					nTmp = readRegister(0x00); // backup the configuration register
					CLEAR_BIT(nTmp, 0); // Release the LED from the "Always Awake" mode
					writeRegister(0x00, nTmp); // Write the configuration register
					nTmp = readRegister(0x08); // read the dumped pixel
					usart_putc((nTmp & 0x01) + 48);
					break;					
				case 'T':
					nTmp = readRegister(0x08); // read the dumped pixel
					usart_putc((nTmp & 0x01) + 48);
					break;					
				default:
					usart_putc('!');
					break;
			}
			usart_putc('>');
		}
		_delay_us(100);
	}
	return 0;
}

