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


#include "MC9S08QE32.h"  /*!< Chip definition file */
#include "..\Shared\common.h"
#include "flashLT64.h"
#include "bootloader.h"
#include "xmodem.h"

extern char __SEG_START_FLASH_ROUTINES[];
extern char __SEG_SIZE_FLASH_ROUTINES[];



#ifdef INCLUDE_PART_ID_CODE
extern unsigned short partId;
// Start addresses for the 32KB and the 16KB part respectively
// Note: These are offset by 1KB because the 1st 1KB 
//       is reserved for app usage.
// TODO - Addresses are hardcoded but the PRM values can't be used.  Since
//        the BL doesn't place anything into the APP area, the linker 
//        doesn't make __SEG_START_APP_ROM available.
static const unsigned short uiAppStartAddress[2] = { 0x8400, 0xC400 };
#endif

#define FL_CMD_BLANK_CHECK    0x05
#define FL_CMD_BYTE_PROGRAM   0x20
#define FL_CMD_BURST_PROGRAM  0x25
#define FL_CMD_PAGE_ERASE     0x40
#define FL_CMD_MASS_ERASE     0x41

#define FLASH_PAGE_SIZE       0x200    // 512 bytes per page

#pragma CODE_SEG DEFAULT
void CopyInRAM(void);

#pragma CODE_SEG FLASH_ROUTINES
unsigned char erase(unsigned short);
unsigned char program(unsigned short, unsigned short, unsigned short);

/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*	\detail	 
*  returns 1 for ERROR	
*  returns 0 for Success
*
*/
unsigned char erase(unsigned short address) 
{
    unsigned short ptr = address;
    unsigned char result = 0; 

    // Wait for command bufffer empty 
    while (!(_FSTAT.Bits.FCBEF));

    if (_FSTAT.Bits.FACCERR) {
      _FSTAT.Bits.FACCERR = 1; 
    }

    if (_FSTAT.Bits.FPVIOL) {
      _FSTAT.Bits.FPVIOL = 1; 
    }

    // First step - write a dummy value to the page to be addressed.
    // NOTE: Don't use 0x00 as a data value here. The compiler produces code
    //       that does a "clr" opcode instead of a "mov". The flash state 
    //       machine doesn't "see" the "clr" and the erase does not occur.
    *((volatile unsigned char *)ptr) = 0x33;

    _FCMD.Byte = FL_CMD_PAGE_ERASE;
    _FSTAT.Bits.FCBEF = 1;

    // Spec says to wait 4 cycles
    asm { 
      nop 
      nop 
      nop 
      nop 
    }

    if ((_FSTAT.Bits.FPVIOL) || (_FSTAT.Bits.FACCERR)) {
        result = 1;
    }
    
    while (!(_FSTAT.Bits.FCCF));

    return result;                                                 

}

/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*	\detail		
*
*/
unsigned char program(unsigned short address, unsigned short buffer, 
                      unsigned short length) 
{
    unsigned short pAddr = address;
    unsigned short pData = buffer;
    //unsigned char result = 0;
    
    if (_FSTAT.Bits.FACCERR) {
      _FSTAT.Bits.FACCERR = 1; 
    }

    if (_FSTAT.Bits.FPVIOL) {
      _FSTAT.Bits.FPVIOL = 1; 
    }
       
    while (length--) 
    {
        WDR();
        
        while (!(_FSTAT.Bits.FCBEF));

        *((volatile unsigned char *)pAddr) = *((unsigned char *)pData);
        _FCMD.Byte = FL_CMD_BURST_PROGRAM;
        _FSTAT.Bits.FCBEF = 1;

        // Spec says to wait 4 cycles
        asm { 
          nop 
          nop 
          nop 
          nop 
        }

        if ((_FSTAT.Bits.FPVIOL) || (_FSTAT.Bits.FACCERR)) {
            //result = 1;
            //break;
            return 1;
        }

        pAddr++;
        pData++;  
    }

    while (!(_FSTAT.Bits.FCCF));

    //return result;
    return 0;
  
}

#pragma CODE_SEG DEFAULT

/*F***************************************************************
*
*   NAME:   CopyInRAM()
*/
/*! \brief	Moves a block of code from ROM and RAM.
*
*	\detail		This section of the code is the one that copies 
*            the routine into RAM. It is following the steps
*            documented in Technical Note 228.
*/
void CopyInRAM(void) 
{
    unsigned short srcPtr = (unsigned short)__SEG_START_FLASH_ROUTINES;
    unsigned short dstPtr = (unsigned short)&erase;
    int count = 0;

    for (count = 0; count < (int)__SEG_SIZE_FLASH_ROUTINES; count++, dstPtr++, srcPtr++) 
    {
        *(volatile char *)dstPtr = *(volatile char *)srcPtr;
    }

}





