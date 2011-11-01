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
#include <string.h>
#include "derivative.h" /*!< include peripheral declarations */
#include "crc16.h"
#include "..\Shared\common.h"     /*!< Common definitions for BL and APP */
#include "..\Shared\sharedRAM.h"  /*!< Definition of data shared between bootloader and app */
#include "bootloader.h" /*!< Generic project-wide defines */
#include "flashlt64.h"
#include "xmodem.h"
#include "Api.h"


extern char __SEG_START_FLASH_ROUTINES[];
extern char __SEG_SIZE_FLASH_ROUTINES[];



#define QUICK_READ if (whichUART == UART_EM250) head+= ucReadApiSci2(&xbuff[head], 0);






static unsigned char xbuff[BUFFER_SIZE];
extern unsigned char head,tail;
static unsigned char whichUART;
static unsigned short timeoutTimer;
static unsigned char timeoutShift;

#define RESET_TIMER() timeoutTimer=0;

#define getChars(x) if (_inbyte(x)) goto reject;


/**************************************************************************
*   PUBLIC FUNCTION DEFINITIONS
**************************************************************************/

static void flushinput(void);
int         check(const unsigned char *, int);
//static void memcpy(unsigned char *, unsigned char *, unsigned int);
char       _inbyte(unsigned char);


 /*F***************************************************************
*
*   NAME:   OUT_BYTE
*/
/*! \brief  send 1 byte out the UART	
*
*	\detail		
*
*/
void OUT_BYTE(unsigned char sendThis){  
    if (whichUART == UART_HOST) {
      while (!SCI1S1_TDRE);
      SCI1D = sendThis;
    }else{
      vWriteCharTxDataSci2(sendThis);
    }
}



 /*F***************************************************************
*
*   NAME:   setTimeoutShift
*/
/*! \brief  sets up the timeout comparison delay is 2^timeoutShift seconds
*
*	\detail		
*
*/
void setTimeoutShift(unsigned char timeoutS){
  timeoutShift = timeoutS;
}




/*F***************************************************************
*
*   NAME:   xmodemReceive
*/
/*! \brief  Public routine to receive a packet from UART 1 or 2
*
*	\detail		
*
*/
long xmodemReceive(unsigned short address,unsigned char uart)
{

	int i = 0;
	unsigned char nakChar = 'C';//NAK with 'C' to start CRC Xmodem transfer
	unsigned char packetno = 1;
	int retry = 0;
  unsigned short startAddress = address;
  unsigned char packet_size = 0;


  whichUART = uart;
  head = 0;
  tail = 0;

              
	do//for (;;) 
	{
			  RESET_TIMER(); 
NEXT_BYTE:			      
			  getChars(1);
			  
			    while (tail != head){
			      getChars(0);//grab new bytes while processing data
				    switch (xbuff[tail]) 
				    {
  				    case SOH:
		  			    goto start_recv;

  				    case EOT:
  				      RESET_TIMER();
  				      if(_inbyte(2));//wait for 1 seconds for extra bytes before calling EOT good.
  				      if (head == 1 ){ //only 1 byte
    					    OUT_BYTE(ACK);
    					    return address - startAddress; /* !!! Function return point for good transfer */
  				      }else{
  				        tail ++;
  		  			    continue;
  				      }
  				      goto NEXT_BYTE;
/*            The only other cancel is a timeout  */
  				    case CAN:
  				      RESET_TIMER();
  				      if(_inbyte(3));//wait for 1 seconds for extra bytes before calling EOT good.
  					    if (0 == tail && head>1 && xbuff[1] == CAN) {
//    						  OUT_BYTE(ACK);  // ACK not required for cancel
  	  					  return -1;      // !!! Function return point for canceled by remote
  		  			  }else{
  		  			    tail ++;//discard character
  		  			    continue;
  		  			  }
  					    goto NEXT_BYTE; 
	  			
	  			    default:
	  			      tail++;//discard wrong character, don't ResetTimer
		  			    break;
				    }//switch
			    }//while
			    tail = 0;
			    head = 0;
      continue;
		
Abort:
//		  flushinput();
//		  OUT_BYTE(CAN);
		  OUT_BYTE(CAN);
		  OUT_BYTE(CAN);		
  		return -2;      /* !!! Function return point for sync error */

      //---------------------------------------------
start_recv:
      if (tail != 0){          
  		  for(i=0;i<(unsigned char)(head-tail);i++){//shift buffer back to zero
  		    xbuff[i] = xbuff[(unsigned char)(i+tail)];
  		  }
  		  head = head - tail;
  		  tail = 0;
      }

      RESET_TIMER();
      // buff[0]        = SOH
      // buff[1]        = block #
      // buff[2]        = 255 - block # (1's comp)
      // buff[3]->[130] = data
      // buff[131]      = crc16 hi
      // buff[132]      = crc16 lo
	
frame:
      getChars(3);
      
      // Check frame number:
      //    1's compliment &&
      //    (block # == expected) or (expected - 1) // -1 handles previously block if missed ACK
      if (xbuff[1] == (unsigned char)(~xbuff[2]) && 
		     (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1))
      {
          RESET_TIMER();
          
          if (packet_size){
            getChars((packet_size)+3+2); //get the rest of the packet and CRC
    		      
            //CRC validation  
            if (!check(&xbuff[3], packet_size)){ 
                goto reject;//if CRC is NOT valid
            }
          }else{//packetsize unknown
            if(_inbyte((PACKET_SIZE_BIG)+3+2)); //get the rest of the packet//timeout doesn't cause a NAK
    		    
    		    //_inbyte((PACKET_SIZE_BIG)+3+2);//see if any more happen in the next second  
            //CRC validation  
            if (head < PACKET_SIZE_SMALL + 6 && check(&xbuff[3], PACKET_SIZE_SMALL)){ //if SMALL packet CRC is  valid
                packet_size = PACKET_SIZE_SMALL;
            }else{
              getChars((PACKET_SIZE_BIG)+3+2); //get the rest of the packet and CRC    		      
              if (!check(&xbuff[3], PACKET_SIZE_BIG)){ //CRC validation  
                  goto reject;//if CRC is NOT valid
              }
              packet_size = PACKET_SIZE_BIG;
            }
          
          }
            
         


          // If packet number is what we expect, process it
          if (xbuff[1] != packetno)	{ //if it was the previous packet number
            retry ++;//host didn't get previous ACK
            if (++timeoutShift>5){//slow down the timeout
               timeoutShift--;
            }
      		  if (retry > (MAXRETRANS<<timeoutShift)){
              goto Abort;
            }
            goto CONFIRM_PACKET;
          }
          
          
//good packet 
//write to flash          
 			  
          //---------------------------------------------
          // Commit these PACKET_SIZE bytes to flash

          // Program this 128 byte block but only if it won't 
          // overlap the bootloader areas.
          if (address < (unsigned short)__SEG_START_FLASH_ROUTINES) 
          {
            WDR();;
              if(startAddress == address){                
                 nakChar = NAK;
                 //erase last block first to avoid running a bad app
                 for (address =(unsigned short)__SEG_START_FLASH_ROUTINES;address>startAddress;){
                   address -= 0x200;
                   WDR();
                   if (erase(address)){
                    goto reject;
                   }
                 }//for
              }//if
              //program all
              if (program((unsigned short)address, (unsigned short)&xbuff[3],(unsigned short)packet_size)) {
                  goto reject;
              } else {
                  address += packet_size;
              }
          }//if  
          packetno++;
          retry =0; //reset retries
          nakChar = NAK;
CONFIRM_PACKET:          
          flushinput();                   
			    OUT_BYTE(ACK);  // Successful block - ACK sender
    	    continue;       // continue to receive more blocks
      }// if (xbuff[1] == ...


reject: // Bad block - NACK sender
  
      retry ++;
      WDR();
      if(nakChar != NAK && (retry&1)){
        if (retry >  MAXRETRANS) {
          goto Abort;
        } 
        OUT_BYTE(nakChar);  
      }else if ((retry & (0xFF>>(unsigned char)(7-timeoutShift))) == 0 ){
  		  flushinput();
  		  if (retry > (MAXRETRANS<<timeoutShift)){
  		      goto Abort;
  		  }
  		  if (retry >  (MAXRETRANS<<timeoutShift)>>1){
  		    retry = retry <<1;
    		  if (++timeoutShift>5){//slow down the timeout
             timeoutShift--;
             retry = retry >>1;    
          }
        }
  		  OUT_BYTE(nakChar);
      }
	}while(1);//main loop
}//function

/**************************************************************************
*   PRIVATE FUNCTION DEFINITIONS
**************************************************************************/

/*F***************************************************************
*
*   NAME:   _inbyte
*/
/*! \brief  Reads in bytes from the UART	
* 
*	\detail		returns a 1 if a timeout occurs
            returns a 0 otherwise 
*
*/
char _inbyte(unsigned char number) 
{
  do{
    WDR();
    if (whichUART == UART_HOST) {
      if (SCI1S1_RDRF) {
        xbuff[head] = SCI1D;
        head++;
      } 
    } else {
      head+= ucReadApiSci2(&xbuff[head], 0);
    } 
    if (!(++timeoutTimer)) return 1;       
  }while ( ((unsigned char)(head-tail)) < number);
  return 0;
}
  
/*F***************************************************************
*
*   NAME:   check
*/
/*! \brief	Wrapper routine for performing CRC on packet
*
*	\detail		
*
*/
 int check(const unsigned char *buf, int sz)
{

  // Notice: "sz" is PACKET_SIZE
  unsigned short crc  = 0;
  unsigned short tcrc = (((unsigned short)buf[sz])<<8) + (unsigned short)buf[sz+1];
  //sz = (int)128;  
		  
  crc = CalculateCrc(buf, sz);

  return (crc == tcrc) ? 1 : 0;
	
}

/*F***************************************************************
*
*   NAME:   flushinput
*/
/*! \brief	Flush any bytes in the UART input buffer
*
*	\detail		
*
*/
static void flushinput(void)
{
  if(_inbyte(0));
	head = 0;
	tail = 0;
}
