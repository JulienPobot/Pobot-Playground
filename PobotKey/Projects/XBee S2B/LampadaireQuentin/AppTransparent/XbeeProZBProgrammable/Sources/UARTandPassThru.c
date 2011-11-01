/*-------------------------------------------------------
* Copyright (C) 20140 Digi International, All Rights Reserved.
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

#include <hidef.h>      /*!< EnableInterrupts macro */
#include "derivative.h" /*!< Include peripheral declarations */
//#include "System.h"

#define   FSCALE_HOST_TX      PTBD_PTBD1
#define   FSCALE_HOST_RX      PTBD_PTBD0
#define   FSCALE_EM250_TX     PTCD_PTCD7
#define   FSCALE_EM250_RX     PTCD_PTCD6
#define   FSCALE_CFG_HOST_TX  PTBDD_PTBDD1
#define   FSCALE_CFG_EM250_TX PTCDD_PTCDD7






/*F***************************************************************
*
*   NAME:   DoPassThroughMode
*/
/*! \brief	Takes UART data coming into HOST and sends it to RADIO
*           Takes UART data coming into RADIO and sends it to HOST
*
*
*  Passes DIN, DOUT, RTS, CTS, and RESET from Computer to EM250
*/

//#define USE_DTR_FOR_RESET
#ifdef USE_DTR_FOR_RESET
 #define RESET !PTDD_PTDD5
#else
 #define RESET PTAD_PTAD5
#endif 
#define RESET_DIR_EM250 PTCDD_PTCDD4
#define WDR(); \
    __RESET_WATCHDOG();

void  DoPassThroughMode(void) 
{
    ICSSC_DRST_DRS0 = 0x1;  //Set for Midrange value
    ICSSC_DRST_DRS1 = 0x0;  //Set for Midrange value
    ICSSC_DMX32 = 1;//fine tuned for max freq w/ 32.768 input freq 
   
    DisableInterrupts;
    // Disable UARTS - we're going to manually control the lines from now on.
    _SCI1C2.Byte = 0;
    _SCI2C2.Byte = 0;

    

    //                      Freescale
    //                      ---------
    //    Tx_U1  - PTB-D1 - Output     
    //    Rx_U1  - PTB-D0 - Input      
    //    Tx_U2  - PTC-D7 - Output = DIN_XBEE
    //    Rx_U2  - PTC-D6 - Input  = DOUT_XBEE
    //
    //    CTS_U1 - PTD6 -  Input
    //    RTS_U1 - PTC1 -  Output
    //    CTS_U2 - PTD7 -  Input
    //    RTS_U2 - PTC0 -  Output

    // Configure port pins for output mode

    
    PTDDD =0; // set all Port D to an input               
    FSCALE_CFG_HOST_TX  = 1;
    PTCDD =(1<<7) | 3; // bit 7, 1, 0 are outputs, others are treated as inputs.(Reset is treated as an input when not in reset)



    FSCALE_HOST_TX  = 1;//initialize state to 11
    FSCALE_EM250_TX = 1;//


        //==================================================
        // First Set 
        //==================================================

        // Goal: Sample Tx and Rx more frequently than RTS and CTS since the
        //       flow control signals change less frequently than the data pins.
    
        // Read data from Host and write it to XBee (U1:Rx -> U2:Tx)
        // Note: Do not rewrite - this sequence produces the fastest code (so far)!

//#define STATE_ON
//#define FORCE_STATE

#ifdef STATE_ON
    PTEDD = 3;
#endif

          for(;;){
#ifdef FORCE_STATE          
            FSCALE_EM250_TX = 1;
            FSCALE_HOST_TX  = 1;
#endif
#ifdef STATE_ON
            PTED = 3;
#endif 
            if (RESET){
              RESET_DIR_EM250 = 0;
            }
L11:        if (!FSCALE_HOST_RX) { //(5 clocks)       //1 instruction  (3 instructions during transition)
              FSCALE_EM250_TX = 0;
              goto L01E;//change state to 01;sample EM250 next
            }            
L11E:       if (!FSCALE_EM250_RX) {                  //1 instruction
              FSCALE_HOST_TX = 0;
              goto L10;//change state to 10
            }
            if (!RESET){
              RESET_DIR_EM250 = 1;//reset EM250
            } 
            //PTCD_PTCD1= PTDD_PTDD7 ;               //3 instructions
            asm {                                    //equal to 1.5 instructions               
             LDA PTDD; // get RTS/CTS  PTDD_PTDD6/7    (4 clocks)
             ROLA;     //bit shift rotate through carry 
             ROLA;     //(1 clock * 3)
             ROLA;     //               
            } 
//note that if HOST_RX or EM250_RX changes,in the next 2 instructions the RTS/CTS sample will be dropped. sampling will be done next loop (saves code space)
            
            if (!FSCALE_HOST_RX) {                  //1 instruction
              FSCALE_EM250_TX = 0;
              goto L01E;
            }
            if (!FSCALE_EM250_RX) {                 //1 instruction
              FSCALE_HOST_TX = 0;
              goto L10;
            }
            //PTCD_PTCD0 = PTDD_PTDD6;              //3 instructions
            asm {              
              AND #0x3; // clear;  allows reset to pass through dependant on direction                                             
              ORA #0x80; // set EM250_TX  (2 clocks)
              STA PTCD;//write RTS/CTS   (4 clocks)   //1 instruction 
            }
            
            if (!FSCALE_HOST_RX) {                  //1 instruction
              FSCALE_EM250_TX = 0;
              goto L01E;
            }
            if (!FSCALE_EM250_RX) {                 //1 instruction
              FSCALE_HOST_TX = 0;
              goto L10;
            }
            WDR(); // 2 instructions overhead
          }//for
          
          for(;;){  
#ifdef FORCE_STATE          
            FSCALE_EM250_TX = 0;
            FSCALE_HOST_TX  = 1;
#endif 
#ifdef STATE_ON
            PTED = 1;
#endif
            if (RESET){
              RESET_DIR_EM250 = 0;
            }

L01:        if (FSCALE_HOST_RX) {
              FSCALE_EM250_TX = 1;
              goto L11E;
            }
L01E:       if (!FSCALE_EM250_RX) {
              FSCALE_HOST_TX = 0;
              goto L00;
            }
            if (!RESET){
              RESET_DIR_EM250 = 1;//reset EM250
            } 
//            PTCD_PTCD1= PTDD_PTDD7 ; 
            asm {                                                  
             LDA PTDD; 
             ROLA;     
             ROLA;     
             ROLA;
            }
          
            if (FSCALE_HOST_RX) {
              FSCALE_EM250_TX = 1;
              goto L11E;
            }
            if (!FSCALE_EM250_RX) {
              FSCALE_HOST_TX = 0;
              goto L00;
            }
            //PTCD_PTCD0 = PTDD_PTDD6; 
            asm {
              AND #0x3; // clear EM250_TX (Based on state of Host_RX)          
              STA PTCD;
            }
            WDR();
          }//for

          for(;;){
#ifdef FORCE_STATE          
            FSCALE_EM250_TX = 1;
            FSCALE_HOST_TX  = 0;
#endif 
#ifdef STATE_ON
            PTED = 2;
#endif
            if (RESET){
              RESET_DIR_EM250 = 0;
            }
            
L10:          
            if (!FSCALE_HOST_RX) {
              FSCALE_EM250_TX = 0;
              goto L00E;
            }
L10E:            
            if (FSCALE_EM250_RX) {
              FSCALE_HOST_TX = 1;
              goto L11;
            }
            if (!RESET){
              RESET_DIR_EM250 = 1;//reset EM250
            } 
//            PTCD_PTCD1= PTDD_PTDD7 ; 
            asm {                        
             LDA PTDD;
             ROLA;     
             ROLA;
             ROLA;      
            }
            
            if (!FSCALE_HOST_RX) {
              FSCALE_EM250_TX = 0;
              goto L00E;
            }
            if (FSCALE_EM250_RX) {
              FSCALE_HOST_TX = 1;
              goto L11;
            }
            //PTCD_PTCD0 = PTDD_PTDD6; 
            asm {
              AND #0x3; // clear;  allows reset to pass through dependant on direction              
              ORA #0x80; // set EM250_TX (Based on state of Host_RX)                                                 
              STA PTCD;
            }                       
            WDR();
          }//for

          for(;;){
          
#ifdef FORCE_STATE          
            FSCALE_EM250_TX = 0;
            FSCALE_HOST_TX  = 0;
#endif 
#ifdef STATE_ON
            PTED = 0;
#endif
            if (RESET){
              RESET_DIR_EM250 = 0;
            }
            
L00:        if (FSCALE_HOST_RX) {
              FSCALE_EM250_TX = 1;
              goto L10E;
            }       
L00E:       if (FSCALE_EM250_RX) {
              FSCALE_HOST_TX = 1;
              goto L01;
            }
            if (!RESET){
              RESET_DIR_EM250 = 1;//reset EM250
            } 
//            PTCD_PTCD1= PTDD_PTDD7 ; 
            asm {                        
             LDA PTDD;
             ROLA;     
             ROLA;
             ROLA;      
            }
          
            if (FSCALE_HOST_RX) {
              FSCALE_EM250_TX = 1;
              goto L10E;
            }
            if (FSCALE_EM250_RX) {
              FSCALE_HOST_TX = 1;
              goto L01;
            }            
            //PTCD_PTCD0 = PTDD_PTDD6; 
            asm {
              AND #0x3; // clear EM250_TX (Based on state of Host_RX)                                   
              STA PTCD;
            }            
            WDR();
          }//for

     
}

    

