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

#include "MC9S08QE32.h"
#include "..\Shared\common.h"
#include "..\Shared\sharedRAM.h"    /*!< Definition of data shared between bootloader and app */
#include "bootloader.h" 
#include "Api.h"


#define MAX_TX_DATA_BUFFER_SIZE 70;

#define MAX_PACKET_LENGTH (MAX_TX_DATA_BUFFER_SIZE + TX_DATA_OFFSET + 1)
#define LENGTH_OFFSET 2
#define COMMAND_OFFSET 5
#define COMMAND_DATA_OFFSET 7
#define API_DELIMITER 0x7E
//RX defines
#define RX_SOURCE64_OFFSET 4
#define RX_SOURCE16_OFFSET 12
#define RX_DATA_OFFSET 15
#define RX_OPTIONS_OFFSET 14
#define RX_FRAME_OFFSET 3
#define RX_FRAME_TYPE 0x90
#define RX_BROADCAST 2
#define RX_ONE_CHAR_LENGTH 13
#define MAX_RX_PACKET_LENGTH 150
//state machine for API
#define STATE_DELIM 0
#define STATE_LENGTH 1
#define STATE_PACKET 2
#define STATE_VALID  3
#define STATE_CHECKSUM  4
#define STATE_DATA 5






// **********************************************************************************


static unsigned char apibuf[MAX_TX_DATA_SIZE + 18];


static unsigned char tail,head;
static unsigned char ucSci2RcvBuf[256];
static unsigned char state;
static unsigned char lengthApi;
static unsigned char fixedSourceAddress[8];
static unsigned char isFixed;

//these are not needed.
/*
static const unsigned char TXPACKET[17]  = {0x7E,0,0,0x10,0, 
                                     0,0,0,0,0,0,0xff,0xff, //64 bit address
                                     0xFF,0xFE,             //16 bit address (Unknown)
                                     0,                     //broadcast radius 0 =maximum
                                     0}  ;                  //Options 1 = disable ACK


/*
const unsigned char RXPACKET[15]  = {0x7E,0,0,0x90, 
                                     0,0,0,0,0,0,0xff,0xff, //64 bit address
                                     0xFF,0xFE,             //16 bit address (Unknown)
                                     0,                     //Options 1 = acknowledged, 2 = broadcast
                                     }  ;                  //
*/
/* 
const unsigned char ATCOMMAND[5] = {0x7E,0,0,8,1};
*/

#ifndef SEND_IN_LINE_CODE

void SEND_CHARACTER_TO_HOST(unsigned char sendThis){
     while (!SCI1S1_TDRE); 
     SCI1D = sendThis; 
  
}

void SEND_CHARACTER_TO_EM250(unsigned char sendThis){
     while (!SCI2S1_TDRE);
     SCI2D = sendThis;
}

#endif



/*F***************************************************************
*
*   NAME:   TxDestination64
*/
/*! \brief	Get the char pointer for the TX Destination address of the API frame
*
*/
char *getTxDestination64ptr(void) {
  return apibuf + TX_DEST64_OFFSET;
}

/*F***************************************************************
*
*   NAME:   getTxDataBuffer
*/
/*! \brief	Get the char pointer for the data of the API frame
*
*/

char *getTxDataBuffer(void) {
  return apibuf + TX_DATA_OFFSET;
}




/*F***************************************************************
*
*   NAME:   ucCalcChecksum
*/
/*! \brief	Calculate checksum for a string of bytes (API packet uses this)
*   255 bytes max.
*/
unsigned char ucCalcChecksum(unsigned char *bytes,unsigned char length){
   unsigned char checksum;
  checksum=0;
  do{//    for(;length;length--){
    checksum+=bytes[0];
    bytes++;
    WDR();    
  }while(--length);
  return (~checksum);//same as 0xFF - checksum

}

/*F***************************************************************
*
*   NAME:   vSetTxDestination64
*/
/*! \brief	Initializes the TX api buffer to the specific 64 bit address
*   
*/
void vSetTxDestination64(unsigned char *dest64bits){  
   char i;
   apibuf[0] = API_DELIMITER;
   apibuf[TX_FRAME_OFFSET] = 0x10;
   apibuf[TX_DEST16_OFFSET] = 0xFF;
   apibuf[TX_DEST16_OFFSET+1] = 0xFE;
   
   for(i=0; i<8; i++){
    apibuf[i+TX_DEST64_OFFSET] = dest64bits[i];
   } 
}


/*F***************************************************************
*
*   NAME:   iWriteToSci2Len
*/
/*! \brief	Send data stored in the TX_DATA_PTR Buffer
*
*/
void  vWriteTxDataSci2Len(unsigned char length) {

   unsigned char i;

    length =  length + TX_DATA_OFFSET  - 3; //Data+ Frame 
    apibuf[LENGTH_OFFSET] = length;
    apibuf[length+3] = ucCalcChecksum(apibuf+3,length);
    length +=3+1;//add in Delim, length[2], checksum
    WDR();
    for (i=0;i<= length;i++) 
    {
        SEND_CHARACTER_TO_EM250(apibuf[i]);
        readEM250();//make sure we don't miss a read
    }
}

/*F***************************************************************
*
*   NAME:   iWriteToSci2Len
*/
/*! \brief	Packet 1 Byte and send it.
*           txDestination64() can be set once for transmissions to the same destination.
*/
void  vWriteCharTxDataSci2(unsigned char data) {
   apibuf[TX_DATA_OFFSET] = data;//transfer byte
   vWriteTxDataSci2Len(1);//send byte
}

/*F***************************************************************
*
*   NAME:   cfixRX64BitAddress
*/
/*! \brief	Fixes the address for which packets will be accepted
*           The 8 byte address is Big Endian
*           Null pointer or address of 0x0000000000000000 clears the fixed address.
*/          
void fixRX64BitAddress(unsigned char *address) {  
    unsigned char i;
    
    isFixed = 0;
    if (0==address){
      //isFixed = 0;
    }else{
      for(i=0;i<8; i++){
        fixedSourceAddress[i]=address[i];
        isFixed = isFixed | fixedSourceAddress[i];
      }
    }//if
    //if the desired source address isn't 0x0000000000000000 then isFixed will != 0;
 }
 


/*F***************************************************************
*
*   NAME:   ucReadApiSci2
*/
/*! \brief	Checks RX buffer for an API packet. returns number of characters
*
*  Requirements
*  Must not be a broadcast
*  Must be from a specific address OR contain only 1 char of data
*  This is determined by whether or not the RX address is fixed.
*  The transmission source is copied to source64 pointer supplied unless it is null. 
*  
*/
unsigned char ucReadApiSci2(unsigned char *dataDest, unsigned char *source64) 
/* Returns number of characters read.
   Copies the source 64 bit address from the API data to source64 variable provided
   Copies the API Data to the dataDest variable provided.
*/
{    
    unsigned char len,i,j,stop,sum;

// when using a char, the compiler upconverts to a short for comparisons and pointers unless forced.(unsigned char)( Equation)
TRY_AGAIN:
    readEM250();
    
//    switch (state){
      
//      case STATE_DELIM:
      if (state == STATE_DELIM){        
              while(API_DELIMITER != ucSci2RcvBuf[tail] && tail != head){//find API delimiter
                tail++; //discard character
                WDR();        
              }
              if (tail != head) {
              state = STATE_LENGTH;
              goto TRY_AGAIN;
              }
              return 0;
              
//      case STATE_LENGTH:
      }else if (state == STATE_LENGTH){        
              if ((unsigned char)(head - tail) < 3) { //do we have the length?
                return 0; //no abort
              }
              lengthApi = ucSci2RcvBuf[(unsigned char)(tail+LENGTH_OFFSET)];
              i = tail + 1;
              if (0 != ucSci2RcvBuf[i] || lengthApi > MAX_RX_PACKET_LENGTH) { //do we have an invalid length > ?? characters?
                tail++; //invalid, Try Again;
                state = STATE_DELIM;
                goto TRY_AGAIN;
              }
              state = STATE_PACKET;
              return 0;
              
//      case STATE_PACKET:
      }else if (state == STATE_PACKET){
              if ((unsigned char)(head - tail) <= lengthApi + LENGTH_OFFSET +1) { //has the whole packet arrived
                return 0; // nope
              }
              state = STATE_VALID;
              goto TRY_AGAIN;//read serial port
              
//      case STATE_VALID:
      }else if (state == STATE_VALID){
              i = tail+RX_OPTIONS_OFFSET;
              j = tail+RX_FRAME_OFFSET;
              if (RX_BROADCAST == ucSci2RcvBuf[i] || RX_FRAME_TYPE!=ucSci2RcvBuf[j]) {//if it is a broadcast or wrong frame type
                tail+=lengthApi+4;//delete unwanted packet
                state = STATE_DELIM;//reset state machine
                goto TRY_AGAIN;
              }
              if (isFixed){
                for(i=0; i < 8 ; i++){
                  if (fixedSourceAddress[i] != ucSci2RcvBuf[(tail+RX_SOURCE64_OFFSET+i)&0xFF]){//Is the address correct
                    tail+=lengthApi+4;//delete unwanted packet
                    state = STATE_DELIM;//reset state machine
                    goto TRY_AGAIN;        
                  }
                }
              }else if (lengthApi != RX_ONE_CHAR_LENGTH){//must be 1 char long.
                tail+=lengthApi+4;//delete unwanted packet
                state = STATE_DELIM;//reset state machine
                goto TRY_AGAIN; 
              }
              state = STATE_CHECKSUM;
              goto TRY_AGAIN;
              
 //     case STATE_CHECKSUM:
      }else if (state ==  STATE_CHECKSUM){
              stop = tail + lengthApi + 3;//this may wrap so for loop must use != stop
              sum = 0;
              for (i=tail + LENGTH_OFFSET+1;i != stop; i++){
                sum += ucSci2RcvBuf[i];
                readEM250();    
                
              }
              if (ucSci2RcvBuf[stop] != (0xFF-sum)){
                state = STATE_DELIM;//invalid checksum
                goto TRY_AGAIN;
              }
              state = STATE_DATA;
              goto TRY_AGAIN;
              
//      case STATE_DATA:
      }else if (state == STATE_DATA){
              stop = tail + lengthApi + 3;//this may wrap
              len = 0;
              for(i = tail+ RX_DATA_OFFSET; i != stop; i++){//copy data to buffer
                dataDest[len] = ucSci2RcvBuf[i];     
                len++;
                readEM250();     
              }//for
              
              if (source64) { //if pointer isn't null     
                for(i=0; i <8; i++){//copy source address to buffer
                   source64[i] = ucSci2RcvBuf[(unsigned char)(tail+RX_SOURCE64_OFFSET+i)];
                }
              }
              tail += ucSci2RcvBuf[(unsigned char)(tail+LENGTH_OFFSET)] +4;
              state = STATE_DELIM; 
              return len;
      }//switch or if (state)
    
    return 0;    
}


/*F***************************************************************
*
*   NAME:   vInitApi
*/
/*! \brief	Initializes API variables
* 
*  
*/
void vInitApi(void){
  head = 0;
  tail = 0;
  state = STATE_DELIM;
  lengthApi = 0;
  isFixed = 0; //anyone sending unicast and 1 byte data packets can talk to us
  vSetTxDestination64(fixedSourceAddress); //initialize TX packets; The address doesn't matter
}


/*F***************************************************************
*
*   NAME:   readEM250
*/
/*! \brief	Restarts the watchdog and checks for data on UART #2
* 
*  
*/
void readEM250(void){
    WDR(); 
    if (SCI2S1_RDRF){
      ucSci2RcvBuf[head] = SCI2D; 
      head++;
    }
}

