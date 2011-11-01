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


#include <hidef.h>            /*!< EnableInterrupts macro */
#include <stdio.h>
#include "derivative.h"       /*!< Include peripheral declarations */
#include "..\Shared\common.h"           /*!< Common definitions for BL and APP */
#include "MC9S08QE32.h"       /*!< Chip definition file */
#include "bootloader.h"       /*!< Generic project-wide defines */
#include "flashLT64.h"        /*!< DoFlashUpdate() prototype */
#include "UARTandPassThru.h"  /*!< API file for UART code */
#include "Api.h"
#include "build_defines.h"


/* rrm - jump to application space "reset" vector */
/*       note that this location may change w/    */
/*       larger bootloader                        */
#define JMP_APP ((void (*)(void))*((unsigned short *) APP_RESET_VECTOR))    //APP_Reset vector is always 2 bytes before the bootloader
#define HARDWARE_ID_MASK 0x70
#define HARDWARE_B 0x00
#define HARDWARE_C 0x10

extern void processRequest(unsigned char);
extern unsigned char version[];
extern const unsigned char *VERSION;


#pragma DATA_SEG SHARED_DATA
volatile APP_RESET_CAUSES AppResetCause @0x200;
volatile BL_RESET_CAUSES  BLResetCause  @0x204;
#pragma DATA_SEG DEFAULT

void vResetEM250(void);

// The (BOOTLOADER_START & 0xFE)<<8 = bootloader section.

//the    .prm file must also be changed to match the  (BOOTLOADER_START)<<8 for the bootloader memory locations
/* rrm - config to protect F200-FFFF */
#define BOOTLOADER_START 0xF2
#define APP_RESET_VECTOR 0xF1FE
#define APP_DEBUGGABLE   0xF1BB
const unsigned char NVPROT_INIT @0x0000FFBD = BOOTLOADER_START-2;//this redirects the interupt vectors and locks the bootloader
extern const unsigned long pVersion    @0x0000FFAA;
/* rrm - vector table shall be redirected to  0xF1C0 */

//DEBUG_ENABLED should be defined in "build_defines.h" so that the Version changes if using debug capabilities
#ifdef DEBUG_ENABLED
const unsigned char NVOPT_INIT @0x0000FFBF = 0x3E;
#else
const unsigned char NVOPT_INIT @0x0000FFBF = 0;//don't allow debug
#endif

extern const unsigned long pAppVersion @0xF1BC;//@0xE9BC;   //TODO - hardcoded - rrm changed to EBBC from EDBC
extern unsigned short partId;
extern const unsigned short uiAppStartAddress[2];



/*F***************************************************************
*
*   NAME:   initBootloader()
*/
/*! \brief	Initializes UARTS, FLASH for erasing etc.
*
*/    
void initBootloader(void){ 
   // Power on UART1 & 2 (SCI), Debug and Flash and turn off everything else
    _SCGC1.Byte = SCGC1_SCI1_MASK | SCGC1_SCI2_MASK;
    _SCGC2.Byte = SCGC2_DBG_MASK | SCGC2_FLS_MASK;
    
    // Configure flash divisor
    FCDIV = FLASH_FCDIV_SETTING;
  
    // Initialize UARTs 
    SCI1BD = UART_BAUD_9600;
    SCI1C2 = SCI1C2_TE_MASK | SCI1C2_RE_MASK;
    SCI2BD = UART_BAUD_9600;
    SCI2C2 = SCI2C2_TE_MASK | SCI2C2_RE_MASK;
    if(memcpy(version,(char*)pVersion,BL_VERSION_STRING_SIZE));
    PTEPE=HARDWARE_ID_MASK;
    if (!(PTED & HARDWARE_ID_MASK)){//HARDWARE_B
    }else if((PTED & HARDWARE_ID_MASK)==HARDWARE_C){
      version[BL_VERSION_HW_OFFSET]='C';
    }else{
      version[BL_VERSION_HW_OFFSET]=PTED & HARDWARE_ID_MASK;
    }
}

/*F***************************************************************
*
*   NAME:   Bootloader Main
*
*  \brief	sets the clock
*         checks for reset reason
*         if reset means that Shared RAM isn't valid then Shared RAM is cleared 
*         Jumps to the App if valid
*         Otherwise will stay in bootloader forever
*
*/
void main(void) 
{
    // Configure the clock
    //-----------------------------------------
    // Runs MC9S08 @ 39.85 Mhz DCO Output. The Bus_Clock is Automatically Divided by 2
    // Max Specified Bus Frequency is 20MHz...
    DisableInterrupts;
    ICSTRM = NVICSTRM;//these are defaults on startup, and now for watchdogs and other errors
    ICSSC = MULTIPLY_BY_1216 | NVFTRIM;
    ICSC1 = CLOCK_SELECT_FLL | REF_TO_FLL_DIVIDE_BY_1  | FLL_REF_SELECT_INTERNAL |ENABLE_ICSIRCLK |DISABLE_IREF_DURING_STOP;
    ICSC2 = (DIVIDE_BY_2*ICSC2_BDIV0_MASK);//bus clock div by 1 
    WDR(); 
 
    
    //-----------------------------------------------------------------------
    // Check what caused reset and cause field. Are we suppose to 
    // just jump to the application?

    // If initial power on, external reset or low voltage then initialize
    // shared memory area (by default it is not initialized so we can pass
    // data back and forth between BL and App).  Note: If CmdMode != 0, we
    // are in command mode; this means the host is actively communicating 
    // with us; this is in shared RAM so that we reboot and not exit Cmd Mode.
    if ((SRS_POR) || (SRS_PIN) || (SRS_LVD)) 
    {
        SOPT1 = (SOPT1_COPE_MASK | SOPT1_STOPE_MASK | SOPT1_COPT_MASK | SOPT1_BKGDPE_MASK | SOPT1_RSTPE_MASK); //this register can only be written once
        AppResetCause = APP_CAUSE_NOTHING;
        BLResetCause = BL_CAUSE_NOTHING;

        // If there was no forced override, and application is valid, just
        // pass control back to the application. Otherwise, stay in bootloader
        // and wait for an update.
        if ((PTDD_PTDD5) || (!PTDD_PTDD7) || (PTBD_PTBD0)) {
            goto JumpToApplicationSpace;
        } 
    } else// App should handle all types of resets. 
    {
        if (BL_CAUSE_NOTHING != BLResetCause ||  APP_CAUSE_REQUEST_SOPT1_NOT_WRITTEN != AppResetCause){//If App requests that the Bootloader not write the SOPT1 register
          SOPT1 = (SOPT1_COPE_MASK | SOPT1_STOPE_MASK | SOPT1_COPT_MASK | SOPT1_BKGDPE_MASK | SOPT1_RSTPE_MASK); //write once register
        }else{
          BLResetCause ++;//inc bootloader cause. This triggers a halt to an endless loop of a bad app.
          goto JumpToApplicationSpace;
        }
        
        if (BLResetCause < BL_CAUSE_BAD_APP && (AppResetCause <= 255)) {//as long as both the BLCause and AppCause is valid
          BLResetCause ++;//inc bootloader cause. This triggers a halt to an endless loop of a bad app.
          goto JumpToApplicationSpace;
        }    
    }


START_BL_MENU:
    
    //-----------------------------------------------------------------------
    // We're staying in the bootloader so initialize the HW the way we want
    // Don't need interrupts in the bootloader....
    //DisableInterrupts;
    initBootloader();
    
    // Initialize API state machine variables
    vInitApi();
    if (APP_CAUSE_BYPASS_MODE == AppResetCause) {
      AppResetCause = APP_CAUSE_NOTHING;
      DoPassThroughMode();    // No return from here
    }
    //-----------------------------------------------------------------------
    // No exit from here except via intentional watchdog reset. We will 
    // keep returning here until there is a valid application present. 
    do//for (;;)
    { 
        // Check if host sent us anything
        if (SCI1S1_RDRF) {
            // Read in byte(s) from host and check for a valid command
            processRequest(UART_HOST);
        }
        processRequest(UART_EM250);
        
    } while(1);//infinite loop; do while takes 2 bytes less than for(;;){

    //-----------------------------------------------------------------------
    // Common exit point for passing control to the application.  Code must
    // jump here - there is no fall-through from above.
JumpToApplicationSpace:
    
    // If application image looks valid, jump into it
    if ((unsigned short )pAppVersion != 0xFFFF &&
        *((unsigned short *)APP_RESET_VECTOR) < APP_RESET_VECTOR){
      //&& *(unsigned long *)&uiAppStartAddress[partId] !=0xFFFFFFFF) {    
        JMP_APP();
    } else { // should go to the BL menu
        goto START_BL_MENU;   
    }
}

/************************************************************************
*
*   NAME: vInitGpio(void)   
*
*   \brief: Initializes appropriate MC9S08 GPIO that are connected to
*           to the EM250.
*      	
*   \details:  
*
*/
/*
void vInitGpio(void){
  //init the port C bit 7...(C7,DIN_XBEE)
  PTCD_PTCD7 = 0;//write zero to PTC0 bit  
  PTCDD_PTCDD7 = 1;//set direction as output
  PTCD_PTCD7 = 0;//set it lo
  //init the port C bit 4...(#RESET_XBEE)
  PTCD_PTCD4 = 1;//write one to PTC4 bit  
  PTCDD_PTCDD4 = 1;//set direction as output
  PTCD_PTCD4 = 1;//set it hi
  //init the port D bit 6...(#CTS_XBEE)
  PTDDD_PTDDD6 = 0;//set as input
   
  
}
*/

/****************************************************************
*
*   NAME: vResetEM250(void)   
*
*   \brief: Resets EM250
*
*   \details: Pulls RESET_EM250 line low for ~50ms and then releases	
*
*/
void vResetEM250(void){
  unsigned char counter = 0;

  PTCD_PTCD4 = 0;//set it lo
  //wait ~52.4ms (EM250 spec says min 250nS...)
  //if(iInitAndStartTpm1Ovf(0x3FFF));
      //error here 
  for(;++counter;){ //wait for a bit
    WDR();
  }
      
  
  PTCD_PTCD4 = 1; //bring back hi
  
  
}


#ifdef INCLUDE_PART_ID_CODE
/*
 * NOTE: This code was removed because the Freescale parts share device IDs.
 *       Actually, a 16kb part IS a 32kb part and a 64k and 96k part are 
 *       really 128KB parts and there is no way to tell them apart. So, we
 *       made multiple binary images - one for each part.....
 */

/*F***************************************************************
*
*   NAME:   idPart
*/
/*! \brief	Return the device ID of the device we're running on.
*
*   \note   The Freescale parts don't implement real device IDs so
*           this routine determines how much RAM is on the device
*           and from that determines which device it is.
*/
static unsigned short idPart() 
{
    // The 32KB part has 2KB of RAM; the 16KB has 1KB.  Both start
    // at offset 0x80.  Therefore, on the 16KB part, 0x480 is not
    // a valid RAM location.
    volatile unsigned short *ptr = (volatile unsigned short *)0x480;
   
    // Write to test area
    *ptr = 0xA5A5;
    // Write to 2nd area to clear any bus latency or other "cache" effects.
    *(ptr+1) = 0x5A5A;
    
    // Read back from the original address - if it matches, there is RAM 
    // there; otherwise it is unimplemented space.
    return (*ptr == 0xA5A5) ? DEVID_32KB : DEVID_16KB;  

}
#endif




