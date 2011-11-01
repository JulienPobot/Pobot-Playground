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

#include <stddef.h>
#include "derivative.h"
#include "System.h"     /*!< Common definitions for BL and APP */
#include "pin_mapping.h"
#include "common.h"
#include "build_defines.h"
//#include "bootloader.h"       /*!< Generic project-wide defines */



#define RADIO_BUFF_SIZE 256
#define RADIO_INDEX_MASK 0xFF
#define HOST_BUFF_SIZE  25



#ifndef BL16
 volatile uint8 hostRxBuf[HOST_BUFF_SIZE];
 volatile uint8 hostRxBufIndex;
#endif 
volatile uint8 radioRxBuf[RADIO_BUFF_SIZE];
volatile uint8 radioRxBufIndex;

//SCIxC1
#define LOOPBACK_OFF         (0*SCI1C1_LOOPS_MASK)
#define LOOPBACK_ON          (1*SCI1C1_LOOPS_MASK)
#define UART_OFF_DURING_WAIT (1*SCI1C1_SCISWAI_MASK)
#define UART_ON_DURING_WAIT  (0*SCI1C1_SCISWAI_MASK)
#define LOOPBACK_TWO_WIRE    (0*SCI1C1_RSRC_MASK)
#define LOOPBACK_ONE_WIRE    (1*SCI1C1_RSRC_MASK)
#define CHARACTER_8BIT       (0*SCI1C1_M_MASK)
#define CHARACTER_9BIT       (1*SCI1C1_M_MASK)
#define WAKEUP_IDLE_LINE     (0*SCI1C1_WAKE_MASK)
#define WAKEUP_ADDRESS_MARK  (1*SCI1C1_WAKE_MASK)
#define IDLE_COUNT_AFTER_START_BIT (0*SCI1C1_ILT_MASK)
#define IDLE_COUNT_AFTER_STOP_BIT  (1*SCI1C1_ILT_MASK)
#define PARITY_OFF           (0*SCI1C1_PE_MASK)
#define PARITY_ON            (1*SCI1C1_PE_MASK)
#define PARITY_EVEN          (0*SCI1C1_PT_MASK)
#define PARITY_ODD           (1*SCI1C1_PT_MASK)

//SCIxS2     
#define RX_BREAK_DETECT_10_BITS (0*SCI1S2_LBKDE_MASK)
#define RX_BREAK_DETECT_11_BITS (1*SCI1S2_LBKDE_MASK)
#define TX_BREAK_10_BITS        (0*SCI1S2_BRK13_MASK)
#define TX_BREAK_13_BITS        (1*SCI1S2_BRK13_MASK)
#define RX_IDLE_DETECT_OFF      (0*SCI1S2_RWUID_MASK)
#define RX_IDLE_DETECT_ON       (1*SCI1S2_RWUID_MASK)
#define RX_DATA_INVERTED        (1*SCI1S2_RXINV_MASK)
#define RX_DATA_NORMAL          (0*SCI1S2_RXINV_MASK)

//SCIxC3
#define RX_PARITY_ERROR_INTERRUPT_DISABLE  (0*SCI1C3_PEIE_MASK)
#define RX_PARITY_ERROR_INTERRUPT_ENABLE   (1*SCI1C3_PEIE_MASK)
#define RX_FRAMING_ERROR_INTERRUPT_DISABLE (0*SCI1C3_FEIE_MASK)
#define RX_FRAMING_ERROR_INTERRUPT_ENABLE  (1*SCI1C3_FEIE_MASK)
#define RX_NOISE_ERROR_INTERRUPT_DISABLE   (0*SCI1C3_NEIE_MASK)
#define RX_NOISE_ERROR_INTERRUPT_ENABLE    (1*SCI1C3_NEIE_MASK)
#define RX_OVERRUN_INTERRUPT_DISABLE       (0*SCI1C3_ORIE_MASK)
#define RX_OVERRUN_INTERRUPT_ENABLE        (1*SCI1C3_ORIE_MASK)
#define TX_DATA_NORMAL                     (0*SCI1C3_TXINV_MASK)
#define TX_DATA_INVERTED                   (1*SCI1C3_TXINV_MASK)
#define ONE_WIRE_RX_MODE                   (0*SCI1C3_TXDIR_MASK)
#define ONE_WIRE_TX_MODE                   (1*SCI1C3_TXDIR_MASK)


#define WRITE_HOST_9_BITS(bit9,byteData)  while (!SCI1S1_TDRE);   \
                                          SCI1C3_T8=bit9;          \
                                          SCI1D = byteData;       \                              
                             
#define WRITE_RADIO_9_BITS(bit9,byteData)  while (!SCI2S1_TDRE);   \
                                           SCI2C3_T8=bit9;          \
                                           SCI2D = byteData;       \ 

//SCIxC2
#define RX_WAKEUP_CONTROL_OFF             (0*SCI1C2_RWU_MASK)
#define RX_WAKEUP_CONTROL_ON              (1*SCI1C2_RWU_MASK)
#define RX_DISABLE                        (0*SCI1C2_RE_MASK)
#define RX_ENABLE                         (1*SCI1C2_RE_MASK)
#define TX_DISABLE                        (0*SCI1C2_TE_MASK)
#define TX_ENABLE                         (1*SCI1C2_TE_MASK)
#define IDLE_INTERRUPT_DISABLE            (0*SCI1C2_ILIE_MASK)
#define IDLE_INTERRUPT_ENABLE             (1*SCI1C2_ILIE_MASK)
#define RX_INTERRUPT_DISABLE              (0*SCI1C2_RIE_MASK)
#define RX_INTERRUPT_ENABLE               (1*SCI1C2_RIE_MASK)
#define TX_COMPLETE_INTERRUPT_DISABLE     (0*SCI1C2_TCIE_MASK)
#define TX_COMPLETE_INTERRUPT_ENABLE      (1*SCI1C2_TCIE_MASK)
#define TX_BUFFER_READY_INTERRUPT_DISABLE (0*SCI1C2_TIE_MASK)
#define TX_BUFFER_READY_INTERRUPT_ENABLE  (1*SCI1C2_TIE_MASK)

#define WRITE_HOST_BREAK()          SCI1C2_SBK = 1;
#define WRITE_RADIO_BREAK()         SCI2C2_SBK = 1;


/*********************************************************************************
*
*   NAME: InitHostUART(void)   
*
*   \brief:  Initializes SCI1	
* 
*   \details: Enables Receive Interrupt and sets Baud to 9600. SCI1 Communicates
*             with Hyperterminal/Host PC.
***********************************************************************************/
void InitHostUART(void){

  /* configure SCI1/Host UART mode */
  SCI1C1 = LOOPBACK_OFF | LOOPBACK_TWO_WIRE | UART_OFF_DURING_WAIT | CHARACTER_8BIT | WAKEUP_IDLE_LINE |IDLE_COUNT_AFTER_START_BIT | PARITY_OFF | PARITY_EVEN;
  SCI1C3 = TX_DATA_NORMAL | RX_PARITY_ERROR_INTERRUPT_DISABLE | RX_FRAMING_ERROR_INTERRUPT_DISABLE|RX_NOISE_ERROR_INTERRUPT_DISABLE|RX_OVERRUN_INTERRUPT_DISABLE|ONE_WIRE_RX_MODE;
  SCI1S2 = RX_BREAK_DETECT_10_BITS | TX_BREAK_10_BITS | RX_IDLE_DETECT_OFF | RX_DATA_NORMAL;                
  SCI1BD = UART_BAUD_9600;
  SCI1C2 = RX_WAKEUP_CONTROL_OFF | RX_ENABLE | TX_ENABLE | RX_INTERRUPT_ENABLE |IDLE_INTERRUPT_DISABLE | TX_COMPLETE_INTERRUPT_DISABLE |TX_BUFFER_READY_INTERRUPT_DISABLE;
 
}
/*********************************************************************************
*
*   NAME: InitRadioUART(void)   
*
*   \brief:  Initializes SCI2	
* 
*   \details: Enables Receive Interrupt and sets Baud to 9600. SCI2 Communicates
*             with EM250, EM250 default Baud Rate is 9600.
***********************************************************************************/
void InitRadioUART(void){

  /* configure SCI2/Radio UART mode */
  SCI2C1 = LOOPBACK_OFF | LOOPBACK_TWO_WIRE | UART_OFF_DURING_WAIT | CHARACTER_8BIT | WAKEUP_IDLE_LINE |IDLE_COUNT_AFTER_START_BIT | PARITY_OFF | PARITY_EVEN;
  SCI2C3 = TX_DATA_NORMAL | RX_PARITY_ERROR_INTERRUPT_DISABLE | RX_FRAMING_ERROR_INTERRUPT_DISABLE|RX_NOISE_ERROR_INTERRUPT_DISABLE|RX_OVERRUN_INTERRUPT_DISABLE|ONE_WIRE_RX_MODE;
  SCI2S2 = RX_BREAK_DETECT_10_BITS | TX_BREAK_10_BITS | RX_IDLE_DETECT_OFF | RX_DATA_NORMAL;                
  SCI2BD = UART_BAUD_9600;
  SCI2C2 = RX_WAKEUP_CONTROL_OFF | RX_ENABLE | TX_ENABLE | RX_INTERRUPT_ENABLE |IDLE_INTERRUPT_DISABLE | TX_COMPLETE_INTERRUPT_DISABLE |TX_BUFFER_READY_INTERRUPT_DISABLE;


}

/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/

void InitUARTs(void){
  InitHostUART();
  InitRadioUART();
}


/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
void DisableRadioUARTIsr(void){
  
  //SCI2C2 &= ~(SCI2C2_TE_MASK | SCI2C2_RE_MASK | SCI2C2_RIE_MASK);  
  SCI2C2 &= ~(SCI2C2_RIE_MASK);//just disable the interupt  
}

/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
void EnableHostUARTIsr(void){
  
  SCI1C2 |= (SCI1C2_TE_MASK | SCI1C2_RE_MASK | SCI1C2_RIE_MASK);  
}
/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
void DisableHostUARTIsr(void){
  
  //SCI1C2 &= ~(SCI1C2_TE_MASK | SCI1C2_RE_MASK | SCI1C2_RIE_MASK);  
  SCI1C2 &= ~(SCI1C2_RIE_MASK); //disable interupt only 
}

/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
void EnableRadioUARTIsr(void){
  
  SCI2C2 |= (SCI2C2_TE_MASK | SCI2C2_RE_MASK | SCI2C2_RIE_MASK);  
}
/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
void  WriteToHostUART(const uint8 * send) 
{
    int16 iNumChars;
    uint8 * ptr;
    ptr  = (uint8 * )send;
    iNumChars = 0;//init
    while (*ptr) 
    {
        WDR(); 
    
        WRITE_CHAR_TO_HOST(*ptr);
        ptr++;
        iNumChars++;
    }

}
void  WriteToHostUARTLen(const uint8 * send,uint8 length) 
{
    uint8 * ptr;
    uint8 i;
    ptr  = (uint8 * )send;
    for (i = length;i;i--) 
    {
        WDR(); 
    
        WRITE_CHAR_TO_HOST(*ptr);
        ptr++;
    }

}
/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
void  WriteToRadioUART(const uint8 * send) 
{
    short iNumChars;
    uint8 * ptr;
    
    ptr = (uint8 * )send;
    iNumChars = 0;//init
    while (*ptr) 
    {
        WDR(); 
    
        WRITE_CHAR_TO_RADIO(*ptr);
        ptr++;
        iNumChars++;
    }

}
void  WriteToRadioUARTLen(const uint8 * send,uint8 length)
{
    uint8 * ptr;
    uint8 i;
    ptr  = (uint8 * )send;
    for (i = length;i;i--) 
    {
        WDR(); 
    
        WRITE_CHAR_TO_RADIO(*ptr);
        ptr++;
    }

} 


/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
#pragma TRAP_PROC

void vSci1Rx(void){//Host UART
  char cStatus;
  
  //clear the isr source
  cStatus = SCI1S1;//dummy read to clear
  //read the char into buf
#ifndef BL16
  hostRxBuf[hostRxBufIndex] = SCI1D;
    
  //move index  
  hostRxBufIndex++;
  if(hostRxBufIndex > 20){
    hostRxBufIndex = 0;
  }
#endif    
    
}

/*F***************************************************************
*
*   NAME:   
*/
/*! \brief	
*
*/
#pragma TRAP_PROC

void vSci2Rx(void){//Radio UART
  char cStatus;
  
  cStatus = SCI2S1;
  //read the char into buf
  radioRxBuf[radioRxBufIndex] = SCI2D;  
 
  radioRxBufIndex++;
  radioRxBufIndex &= RADIO_INDEX_MASK;//wrap number; 
 
}




