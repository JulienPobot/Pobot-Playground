/*-------------------------------------------------------
* Copyright (C) 2009 Digi International, All Rights Reserved.
*
*
* This software is provided as instructional material without charge 
* by Digi International for use by its employees and customers
* subject to the following terms.
*
* PERMISSION
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software, to deal with it without restriction, 
* including without limitation the rights to use, copy,  modify, merge, publish, 
* distribute, sublicense, and/or sell copies of it, and to permit persons to 
* whom it is furnished to do so, provided the above copyright notice 
* and this permission notice are included in all derived works
* and the use of this software is restricted to Digi products.
*
* WARRANTY
* THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, OR NONINFRINGEMENT.  
*
* LIABILITY
* IN NO EVENT SHALL DIGI INTERNATIONAL BE LIABLE FOR ANY CLAIM, DAMAGES, 
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, 
* ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE, OR THE USE 
* OR OTHER DEALINGS WITH THE SOFTWARE.
*
*-------------------------------------------------------*/

#include <hidef.h>      /*!< for EnableInterrupts macro */
#include "derivative.h" /*!< include peripheral declarations */
#include "crc16.h"
#include "..\Shared\common.h"     /*!< Common definitions for BL and APP */
#include "bootloader.h" /*!< Generic project-wide defines */

#define CRC_ITU_POLY 0x1021

unsigned int CalculateCrc(const unsigned char *pData, unsigned int len) 
{
  unsigned short crc=0;
  unsigned char i;
  unsigned char temp;
  
  do 
  {
    WDR();
    
    crc = crc ^ (*pData<<8);

    pData += 1;

    for (i = 8; i > 0; i--)
    {
      temp = (crc & 0x8000) >> 8;
      crc <<= 1;
      
      if (temp) {
        crc ^= CRC_ITU_POLY;
      }
    }
    len -= 1;
  } while (len);
  
  return crc;

}



