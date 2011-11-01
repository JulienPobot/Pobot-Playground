/*H************************************************************************
*/
/*! \file    
*
*   \brief   
*             
*   
*   \details  
*   
*     Copyright (c) Spectrum Design Solutions, Inc. 2009.
*     All rights reserved.
*
*/
#include "MC9S08QE16.h"  /*!< Chip definition file */
#include "common.h"
#include "xmodem.h"
#include "flashLT64.h"

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
    unsigned char result = 0;
    
    if (_FSTAT.Bits.FACCERR) {
      _FSTAT.Bits.FACCERR = 1; 
    }

    if (_FSTAT.Bits.FPVIOL) {
      _FSTAT.Bits.FPVIOL = 1; 
    }
       
    while (length--) 
    {
        THROW_FIDO_A_BONE_SO_HE_DOESNT_RESET_US
        
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
            result = 1;
            break;
        }

        pAddr++;
        pData++;  
    }

    while (!(_FSTAT.Bits.FCCF));

    return result;
  
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

/*F***************************************************************
*
*   NAME:   DoFlashUpdate
*/
/*! \brief	
*
*	\detail		
*
*/
bool DoFlashUpdate(unsigned char uart)
{              
    bool result = TRUE; 
    long length = 0;

    // Copy the flash routines to RAM
    CopyInRAM();
    
    length = xmodemReceive(0xC400, (long)(__SEG_START_FLASH_ROUTINES - 0xC400), uart);

    if (length < 0) 
    {
        // Erase the first page so that the application image doesn't look 
        // valid. This prevents main.c from jumping into bad code at next 
        // cold power on.
        erase(0xC400);
        result = FALSE;
    } 
    
    return (result);
    
}



