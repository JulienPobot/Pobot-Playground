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

#include <hidef.h>        /*!< for EnableInterrupts macro */
#include "derivative.h"   /*!< include peripheral declarations */
#include "..\Shared\common.h"       /*!< Common definitions for BL and APP */
#include "..\Shared\sharedRAM.h"    /*!< Definition of data shared between bootloader and app */
#include "bootloader.h"   /*!< Generic project-wide defines */
#include "flashLT64.h"
#include "Api.h"
#include "xmodem.h"
#include "build_defines.h"


void  processRequest(unsigned char);
void  SendString(char*, unsigned char); 

extern void DoPassThroughMode(void);





#define   CMD_SEND_MENU       0x0D
#define   CMD_SEND_MENU2      0x0A
#define   CMD_SEND_MENU3      ' '
#define   CMD_CMD_PROMPT      '>'

#define   CMD_MENU_OPTION_B   'B'
#define   CMD_MENU_OPTION_F   'F'
#define   CMD_MENU_OPTION_V   'V'
#define   CMD_MENU_OPTION_A   'A'
#define   CMD_MENU_OPTION_R   'R'
#define   CMD_MENU_OPTION_T   'T'

#define NUM_MENU_ITEMS  2
static const char menu1[] = "\rB-Bypass Mode";                            
#pragma CONST_SEG EXTRA_DATA
static const char menu2[] = "\rF-Update App" 
                            "\rT-Timeout"
                            "\rV-BL Version"
                            "\rA-App Version"
                            "\rR-Reset\r>";
#pragma CONST_SEG DEFAULT
static const char *menu[NUM_MENU_ITEMS] = {menu1,menu2/*,menu3,menu4,menu5,menu6*/};





static char badCommands=0;
static unsigned char timeoutMenuH=0;
static unsigned short timeoutMenuL=0;
static unsigned char tBuff[2]= {0,0};



//Changes
//
//023 Changes
//  condensed version to XYZ = 023
//  
//024 Beta Changes
//  changed SOPT1 write to include STOP modes
//  Added feature to allow App to request that SOPT1 not be written by the Bootloader
//  Fixed AppVersion responce to return Unknown when App is not complete
//  Added Hardware Detection for string returned
//  Removed a bunch of extra characters and \n
//  
//025 Changes
//  release version 024
//  

/**********************************
*       Versions
*
*  BLFFF-HHH-XYZ_DDD
*  FFF = Flash size in k;  032 = 32k Flash
*  HHH = Hardware depenancy 2B0 = S2B used Freescale MC9S08QE = 0
*  X   = Change of size of bootloader/ customization/ functionality 
*        '0' to '9', 'A' to 'F' reserved for Digi. D is used for Debuggable code
*  Y  = Major Change  ('0' to '9', 'A' to 'Z')
*  Z  = Minor revision (Beta revisions are Even, Releases are Odd)
*  DDD = Prefered Update XMODEM Payload size. 
*        064 = 64 bytes per packet 
*        Double the payload size is expected to also be handled, but not guaranteed.
*/

//DEBUG_ENABLED etc should be defined in "build_defines.h" so that the Version shows the Debug option shows in the VERSION

#ifdef BL32
#ifdef DEBUG_ENABLED
static const unsigned char VERSION[BL_VERSION_STRING_SIZE] = "BL032-2B0-D25_064";
#else
static const unsigned char VERSION[BL_VERSION_STRING_SIZE] = "BL032-2B0-025_064";
#endif
//release VERSION "BL032-2B0-025_064"
#endif

#ifdef BL16
#ifdef DEBUG_ENABLED
static const unsigned char VERSION[BL_VERSION_STRING_SIZE] = "BL016-2B0-D25_064";
#else
static const unsigned char VERSION[BL_VERSION_STRING_SIZE] = "BL016-2B0-025_064";
#endif
#endif

const unsigned long pVersion    @0x0000FFAA = (unsigned long)VERSION;     //TODO - hardcoded
volatile const unsigned long pAppVersion @0x0000F1BC = 0xFFFFFFFF;        //TODO - hardcoded - rrm updated
unsigned char timeoutShift = 0;
unsigned char *version[BL_VERSION_STRING_SIZE];

/*F***************************************************************
*
*   NAME:   processRequest
*
*  \brief	Takes in bytes from UART 1 or 2 and processes them
*
*/
void  processRequest(unsigned char uart) 
{
    byte hostByte;
    unsigned char j,length;

    // Get character local and echo it back
    if (UART_HOST == uart){      
      hostByte = SCI1D;
      SCI1D = hostByte;//echo back character
    }else{
      hostByte =0;
      length = ucReadApiSci2(getTxDataBuffer(),getTxDestination64ptr());// reads API and prepares to echo data back  
      if (length ==1){
        hostByte = *getTxDataBuffer(); 
        vWriteTxDataSci2Len(1);//echo back character
      }
    }//if

    if (hostByte>0x60){
      hostByte &= 0xDF;   // Convert to upper case for above 'a'    
    }
    WDR(); 

    if (APP_CAUSE_FIRMWARE_UPDATE == AppResetCause) {
      AppResetCause = APP_CAUSE_NOTHING;
      fixRX64BitAddress((unsigned char *) 0x20E);
      vSetTxDestination64((unsigned char *) 0x20E);
      setTimeoutShift(3);//default to a slower start timeout for over the air
      uart = UART_EM250;
      goto UPDATE_APP;
    }

    switch (hostByte) 
    {
        case 0:
          //do nothing
          break;
    
        case CMD_MENU_OPTION_B:
          if (UART_HOST == uart){
            DoPassThroughMode();    // No return from here
          }else{
            goto PRINT_MENU;
          }
        break;
            
        case CMD_MENU_OPTION_F:
            // Do xmodem download and program the flash at the same time. 
            // If any errors occur, don't jump into the application if 
            // we know the download/programming failed.
            if (UART_EM250 == uart){
              fixRX64BitAddress(getTxDestination64ptr());//Only receive from this address
            }
UPDATE_APP:            
            CopyInRAM();
#ifdef BL32            
            if (xmodemReceive(0x8400, uart) > 0) {
#endif
#ifdef BL16            
            if (xmodemReceive(0xC400, uart) > 0) {
#endif
              // Just finished a successful flash update. Force a processor 
              // reset. After getting back to the top of main, check if the App works. 
              goto RESET_HERE;
            }
            vInitApi();//reset Api stuff on upload failure
        break;
        case CMD_MENU_OPTION_T:
             timeoutShift++;
             if (timeoutShift > 6){
                timeoutShift=0;//wrap
             } 
             tBuff[0] = timeoutShift | 0x30;//make it an ASCII number
             SendString(tBuff,uart);
             setTimeoutShift(timeoutShift);
        break;
        case CMD_MENU_OPTION_A:
            if ((unsigned short)pAppVersion != 0xFFFF) {
              //SendString("\r", uart);
              SendString((char *)pAppVersion, uart);
              if (BL_CAUSE_BAD_APP == BLResetCause){
                SendString("\rBad",uart);
              }
            } else {
              SendString("Unknown", uart);
            }
            break;
        case CMD_MENU_OPTION_V:
            SendString((char *)version, uart);
            break;
        case CMD_MENU_OPTION_R:
RESET_HERE:            
            BLResetCause = BL_CAUSE_NOTHING;
            AppResetCause = APP_CAUSE_NOTHING;
            for (;;); //wait for watchdog and enter app 
        case CMD_SEND_MENU:
        case CMD_SEND_MENU2:
        case CMD_SEND_MENU3:
             badCommands = 0;//this resets the bad commands counter
PRINT_MENU:
#if (UART_HOST==0 && UART_EM250==1)
             j = uart;//Skips menu option B for Radio UART
#else             
             j = 1;
             if (UART_HOST == uart){
               j = 0;//include menu option B for Local UART
             }
#endif             
             for (; j < NUM_MENU_ITEMS; j++) {
                SendString((char *)menu[j],uart);
             }
        default:

            
            badCommands++;
            badCommands &=0x3f;
            if (!badCommands){
              goto RESET_HERE;          
            }   
            break;
    }//switch
    
    WDR();
    if (!++timeoutMenuL){//this timer forces a reset after 9 to 10 minutes of accumulated idle time
      if (!++timeoutMenuH){
        goto RESET_HERE;
      }
    }
}

/*F***************************************************************
*
*   NAME:   SendString
*/
/*! \brief	Sends an ASCII string to the host via UART
*
*/
void  SendString(char *send, unsigned char uart) 
{
  char *ptr;
  unsigned char length;
  
  if (UART_HOST == uart){
    ptr = send;
    while (*(unsigned char *)ptr) 
    {
        WDR();     
        SEND_CHARACTER_TO_HOST(*(unsigned char *)ptr);
        ptr++;
    }
  }else{//write api packets
    length = 0;
    ptr = getTxDataBuffer();
    while (*send){
      ptr[0] = send[0];
      ptr++;
      send++;
      length++;
      if (length > 30){
        vWriteTxDataSci2Len(length);  
        length = 0;
        ptr = getTxDataBuffer();
      }
      WDR();
    }
    if (length){
      vWriteTxDataSci2Len(length);  
    }
  }//if    
}

