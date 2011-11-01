/*-------------------------------------------------------
* Copyright (C) 2010 Digi International, All Rights Reserved.
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
* REQUEST FOR IMPROVEMENTS
* If you have suggestions for improvements, or source code which corrects,
* enhances, or extends the functionality of this software, please forward
* them to the Tech Support group in the Lindon, Utah office 
* of Digi International addressed to the attention of 'Linus'.
*-------------------------------------------------------*/

#include "vector_table.h"
#include "common.h"


extern void vTpm1Ovf(void);/* timer isr located in timer.c */
extern void vTpm2Ovf(void);/* timer isr located in timer.c */
extern void _Startup(void);/* _Startup located in Start08.c */
extern void vSci1Rx(void);/* sci1 rx isr */
extern void vSci2Rx(void);/* sci2 rx isr */
extern void vAdc(void);   // AD isr  
extern int16  WriteToHostUART(uint8 *);

void vDummyIsr(void);

#pragma CONST_SEG VECTORS
/*
void (* const vector_table[])(void) = // Relocated Interrupt vector table 
{ 
//See Project Settings->Linker Files->Application.prm

          vTpm3Ovf,               // Int.no.  0 Vtpm3ovf (at F1C0)              Change in Application.prm 
         vDummyIsr,               // Int.no.  1 Vtpm3ch5 (at F1C2)              Change in Application.prm 
         vDummyIsr,               // Int.no.  2 Vtpm3ch4 (at F1C4)              Change in Application.prm 
         vDummyIsr,               // Int.no.  3 Vtpm3ch3 (at F1C6)              Change in Application.prm 
         vDummyIsr,               // Int.no.  4 Vtpm3ch2 (at F1C8)              Change in Application.prm 
         vDummyIsr,               // Int.no.  5 Vtpm3ch1 (at F1CA)              Change in Application.prm 
         vDummyIsr,               // Int.no.  6 Vtpm3ch0 (at F1CC)              Change in Application.prm 
         vDummyIsr,               // Int.no.  7 Vrtc (at F1CE)                  Change in Application.prm 
         vDummyIsr,               // Int.no.  8 Vsci2tx (at F1D0)               Change in Application.prm 
           vSci2Rx,               // Int.no.  9 Vsci2rx (at F1D2)               Change in Application.prm 
         vDummyIsr,               // Int.no. 10 Vsci2err (at F1D4)              Change in Application.prm 
         vDummyIsr,               // Int.no. 11 Vacmpx (at F1D6)                Change in Application.prm 
              vAdc,               // Int.no. 12 Vadc (at F1D8)                  Change in Application.prm 
         vDummyIsr,               // Int.no. 13 Vkeyboard (at F1DA)             Change in Application.prm 
         vDummyIsr,               // Int.no. 14 Viic (at F1DC)                  Change in Application.prm 
         vDummyIsr,               // Int.no. 15 Vsci1tx (at F1DE)               Change in Application.prm 
           vSci1Rx,               // Int.no. 16 Vsci1rx (at F1E0)               Change in Application.prm 
         vDummyIsr,               // Int.no. 17 Vsci1err (at F1E2)              Change in Application.prm 
         vDummyIsr,               // Int.no. 18 Vspi (at F1E4)                  Change in Application.prm 
         vDummyIsr,               // Int.no. 19 VReserved12 (at F1E6)           Change in Application.prm 
          vTpm2Ovf,               // Int.no. 20 Vtpm2ovf (at F1E8)              Change in Application.prm 
         vDummyIsr,               // Int.no. 21 Vtpm2ch2 (at F1EA)              Change in Application.prm 
         vDummyIsr,               // Int.no. 22 Vtpm2ch1 (at F1EC)              Change in Application.prm 
         vDummyIsr,               // Int.no. 23 Vtpm2ch0 (at F1EE)              Change in Application.prm 
          vTpm1Ovf,               // Int.no. 24 Vtpm1ovf (at F1F0)              Change in Application.prm 
         vDummyIsr,               // Int.no. 25 Vtpm1ch2 (at F1F2)              Change in Application.prm 
         vDummyIsr,               // Int.no. 26 Vtpm1ch1 (at F1F4)              Change in Application.prm 
         vDummyIsr,               // Int.no. 27 Vtpm1ch0 (at F1F6)              Change in Application.prm 
         vDummyIsr,               // Int.no. 28 Vlvd (at F1F8)                  Change in Application.prm 
         vDummyIsr,               // Int.no. 29 Virq (at F1FA)                  Change in Application.prm 
         vDummyIsr,               // Int.no. 30 Vswi (at F1FC)                  Change in Application.prm 
         _Startup                 // Int.no. 31 Vreset (at F1FE)                Change in Application.prm 

};
*/
void vDummyIsr(void){
  for(;;){
    if(WriteToHostUART("STUCK IN UNASSIGNED ISR\n\r>"));
  } 
}