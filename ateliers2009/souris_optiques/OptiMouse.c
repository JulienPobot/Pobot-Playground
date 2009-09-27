/*
 OptiMouse.cpp - Part ofoptical mouse sensor library for Arduino
 Copyright (c) 2008 Martijn The.  All right reserved.
 http://www.martijnthe.nl/
 
 Based on sketches by Benoît Rousseau.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "OptiMouse.h"
#include <avr/io.h>
#include <util/delay.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))

/******************************************************************************
 * Constructors
 ******************************************************************************/

void OptiMouseInit(void)
{
	SET_BIT(DDRC, 0); // PC0 -> SCK -> output 
	CLEAR_BIT(DDRC, 2); // PC1 -> SDIO -> input
	SET_BIT(PORTC, 0); // SCK HIGH
	_delay_ms(110); // Wait for a timeout
}


/******************************************************************************
 * User API
 ******************************************************************************/

void begin(void)
{
  // Re-sync (see datasheet §5.4):
  // Toggle the SLCK line from high to low to high....
  SET_BIT(PORTC, 0); // SCK HIGH
  _delay_us(5);
  CLEAR_BIT(PORTC, 0); // SCK LOW
  _delay_us(5);
  SET_BIT(PORTC, 0); // SCK HIGH

  // Wait at least tSIWTT (0.9 second?) for the
  // OptiMouse serial transaction timer to time out:
  _delay_ms(1000);
}

// Private Methods /////////////////////////////////////////////////////////////


uint8_t readRegister(uint8_t address)
{
  int i = 7;
  uint8_t r = 0;

  // Write the address of the register we want to read:
  SET_BIT(DDRC, 2); // PC1 -> SDIO -> output
  for (; i>=0; i--)
  {
    CLEAR_BIT(PORTC, 0); // SCK LOW
    if(bit_is_set(address,i))
		SET_BIT(PORTC, 2); // SDIO HIGH
	else
		CLEAR_BIT(PORTC, 2); // SDIO LOW
    SET_BIT(PORTC, 0); // SCK HIGH
  }

  // Switch data line from OUTPUT to INPUT
  CLEAR_BIT(DDRC, 2); // PC1 -> SDIO -> input
  // Wait a bit...
  _delay_us(105);  


  // Fetch the data!
  for (i=7; i>=0; i--)
  {                             
    CLEAR_BIT(PORTC, 0); // SCK LOW
    SET_BIT(PORTC, 0); // SCK HIGH
	if(bit_is_set(PINC, 2))
		SET_BIT(r, i);
  }
  _delay_us(50);

  return r;
}

void writeRegister(uint8_t address, uint8_t data)
{
  // Set MSB high, to indicate write operation:
  address |= 0x80;

  // Write the address:
  SET_BIT(DDRC, 2); // PC1 -> SDIO -> output
  for (int i = 7; i>=0; i--)
  {
    CLEAR_BIT(PORTC, 0); // SCK LOW
    if(bit_is_set(address,i))
		SET_BIT(PORTC, 2); // SDIO HIGH
	else
		CLEAR_BIT(PORTC, 2); // SDIO LOW
    SET_BIT(PORTC, 0); // SCK HIGH
  }

  // Write the data:
  for (int i=7; i>=0; i--)
  {
    CLEAR_BIT(PORTC, 0); // SCK LOW
    if(bit_is_set(data,i))
		SET_BIT(PORTC, 2); // SDIO HIGH
	else
		CLEAR_BIT(PORTC, 2); // SDIO LOW
    SET_BIT(PORTC, 0); // SCK HIGH
  }
  _delay_us(50);
}
