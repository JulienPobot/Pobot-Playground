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

#include "MC9S08QE32.h"
#include "Api.h"
#include "System.h"
#include "build_defines.h"

#ifdef BL32
 #define MAX_TX_DATA_BUFFER_SIZE 70
 #define AT_COMMAND_BUFFER       80
#endif
#ifdef BL16
 #define MAX_TX_DATA_BUFFER_SIZE 50
 #define AT_COMMAND_BUFFER       35
#endif

 

#define MAX_PACKET_LENGTH (MAX_TX_DATA_BUFFER_SIZE + TX_DATA_OFFSET + 1)
#define LENGTH_OFFSET 2
#define COMMAND_OFFSET 5
#define COMMAND_DATA_OFFSET 7

//RX defines
#define RX_SOURCE64_OFFSET 4
#define RX_SOURCE16_OFFSET 12
#define RX_DATA_OFFSET 15
#define RX_OPTIONS_OFFSET 14
#define RX_FRAME_OFFSET 3
#define RX_FRAME_TYPE 0x90
#define RX_UNICAST 1
#define RX_BROADCAST 2
#define RX_ONE_CHAR_LENGTH 13
#define MAX_RX_PACKET_LENGTH 150

#define AT_FRAME_TYPE 0x88
#define AT_RX_DATA_OFFSET 8
#define AT_TX_DATA_OFFSET 7
#define AT_COMMAND_OFFSET 5

//state machine for API
#define STATE_DELIM 0
#define STATE_LENGTH 1
#define STATE_PACKET 2
#define STATE_VALID  3
#define STATE_CHECKSUM  4
#define STATE_DATA 5




// **********************************************************************************


static uint8 apibuf[MAX_TX_DATA_SIZE + 18];
static uint8 atbuf[AT_COMMAND_BUFFER];


static uint8 tail,head;//tail and head index of a wrapped buffer
extern volatile uint8 radioRxBuf[];
static uint8 state;
static uint8 lengthApi;
static uint8 fixedSourceAddress[8];
static uint8 isFixed;
extern volatile uint8 radioRxBufIndex;
static uint8 frameType, lastFrame;
static uint8 lastRxOption; 

/*
static const uint8 TXPACKET[17]  = {0x7E,0,0,0x10,0, 
                                     0,0,0,0,0,0,0xff,0xff, //64 bit address
                                     0xFF,0xFE,             //16 bit address (Unknown)
                                     0,                     //broadcast radius 0 =maximum
                                     0}  ;                  //Options 1 = disable ACK


*/
/*
const uint8 RXPACKET[15]  = {0x7E,0,0,0x90, 
                                     0,0,0,0,0,0,0xff,0xff, //64 bit address
                                     0xFF,0xFE,             //16 bit address (Unknown)
                                     0,                     //Options 1 = acknowledged, 2 = broadcast
                                     }  ;                  //
*/

const uint8 ATCOMMAND[5] = {0x7E,0,0,8,1};


/*F***************************************************************
*
*   NAME:   TxDestination64
*/
/*! \brief	Get the char pointer for the TX Destination address of the API frame
*
*/
char *GetTxDestination64ptr(void) {
  return apibuf + TX_DEST64_OFFSET;
}

/*F***************************************************************
*
*   NAME:   GetTxDataBuffer
*/
/*! \brief	Get the char pointer for the data of the API frame
*
*/

char *GetTxDataBuffer(void) {
  return apibuf + TX_DATA_OFFSET;
}

/*F***************************************************************
*
*   NAME:   SetTail
*/
/*! \brief sets the tail of the wrapping SCI2 buffer
*
*/
void SetTail(uint8 set_tail){
  tail = set_tail;
  state = STATE_DELIM;//reset state machine
}

/*F***************************************************************
*
*   NAME:   GetNumberOfCharsInBuff
*/
/*! \brief gets the current number of characters in the SCI2 buffer
*
*/
uint8 GetNumberOfCharsInBuff(void){
  return head - tail;
}


/*F***************************************************************
*
*   NAME:   CalcChecksum
*/
/*! \brief	Calculate checksum for a string of bytes (API packet uses this)
*   255 bytes max.
*/
uint8 CalcChecksum(uint8 *bytes,uint8 length){
   uint8 checksum;
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
*   NAME:   SetTxDestination64
*/
/*! \brief	Initializes the TX api buffer to the specific 64 bit address
*   
*/
void SetTxDestination64(uint8 *dest64bits){
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
*   NAME:   WriteTxDataRadioLen
*/
/*! \brief	Send data stored in the TX_DATA_PTR Buffer
*
*/

void  WriteTxDataRadioLen(uint8 length) {
   uint8 i;
   
    length =  length + TX_DATA_OFFSET  - 3; //Data+ Frame  
    apibuf[LENGTH_OFFSET] = length;
    apibuf[length+3] = CalcChecksum(apibuf+3,length);
    length +=3+1;//add in Delim, length[2], checksum
    WDR();
    for (i=0;i<= length;i++) 
    {
        WRITE_CHAR_TO_RADIO(apibuf[i]);
        readEM250();//make sure we don't miss a read
    }
   

}


/*F***************************************************************
*
*   NAME:   WriteCharTxDataRadio
*/
/*! \brief	Packet 1 Byte and send it.
*
*/
void  WriteCharTxDataRadio(uint8 data) {
   apibuf[TX_DATA_OFFSET] = data;//transfer byte
   WriteTxDataRadioLen(1);//send byte
}

/*F***************************************************************
*
*   NAME:   CopyAtCommandToHost
*/
/*! \brief	Send last AT Command with Data to Host. (API format)
*
*/

void  CopyAtCommandToHost(void){
  uint8 i;
  uint8 length = atbuf[LENGTH_OFFSET]+4; 
  for (i=0;i< length;i++) 
  {
      WRITE_CHAR_TO_HOST(atbuf[i]);
      readEM250();//make sure we don't miss a read
  }
}
/*F***************************************************************
*
*   NAME:   WriteAtCommandRadioLen
*/
/*! \brief	Send AT Command in API format with Data.
*
*/

void  WriteAtCommandRadioLen(const uint8 *AtCommandPtr, const uint8 *dataPtr, uint8 length) {
   uint8 i;
    
    for (i=0;i<AT_COMMAND_OFFSET;i++){//At Frame
      atbuf[i] = ATCOMMAND[i];
    }
    atbuf[AT_COMMAND_OFFSET]= AtCommandPtr[0];//set AT command
    atbuf[AT_COMMAND_OFFSET+1]= AtCommandPtr[1];
    
    for (i=0;i<length;i++){//copy data
      atbuf[i+AT_TX_DATA_OFFSET] = dataPtr[i];
    }
    
    length =  length + AT_TX_DATA_OFFSET  - 3; //Data+ Frame 
    
    atbuf[LENGTH_OFFSET] = length;
    atbuf[length+3] = CalcChecksum(atbuf+3,length);
    length +=3+1;//add in Delim, length[2], checksum
    WDR();
    for (i=0;i< length;i++) 
    {
        WRITE_CHAR_TO_RADIO(atbuf[i]);
        readEM250();//make sure we don't miss a read
    }  

}

/*F***************************************************************
*
*   NAME:   WriteAtCommandRadio
*/
/*! \brief	Send AT Command (API format)
*
*/

void  WriteAtCommandRadio(const uint8 *AtCommandPtr){
  WriteAtCommandRadioLen(AtCommandPtr, 0, 0);
}

/*F***************************************************************
*
*   NAME:   WriteAtCommandRadioByte
*/
/*! \brief	Send AT Command with byte (API format)
*
*/
void  WriteAtCommandRadioByte(const uint8 *AtCommandPtr, uint8 data) {
  WriteAtCommandRadioLen(AtCommandPtr, &data, 1);
}

/*F***************************************************************
*
*   NAME:   WriteAtCommandRadioByte
*/
/*! \brief	Send AT Command with byte (API format)
*
*/
void  WriteAtCommandRadioUint16(const uint8 *AtCommandPtr, uint16 data) {
  if (((uint8*)&data)[0] == (uint8)(data & 0xFF)){//little endian
    data = (data<<8) | (data>>8);//swap bytes for big endian
  }
  WriteAtCommandRadioLen(AtCommandPtr,(uint8*) &data, 2);//this only works for Big endian machines
}


/*F***************************************************************
*
*   NAME:   fixRX64BitAddress
*
* \brief    Makes the ReadApiRadio only accept transmissions from a specific source address
*           Void pointer = receive from anyone
*           Address 0x000000000000000 also means receive packets from anyone 
*/


void FixRX64BitAddress(uint8 *address) {

    uint8 i;
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
 
 uint8 GetLastFrameType(void){
  return lastFrame;  
 }
 uint8 WasBroadcast(void){
  return lastRxOption == RX_BROADCAST;  
 }
 uint8 WasUnicast(void){
  return lastRxOption == RX_UNICAST;  
 }
 


/*F***************************************************************
*
*   NAME:   ReadApiRadio
*
* \brief	Checks RX buffer for an API packet. returns number of characters
*
* \detail Returns number of characters read.
*         Copies the source 64 bit address from the API data to source64 variable provided
*         If source64 is null, the 64 bit address is discarded
*         Copies the API Data to the dataDest variable provided.
*  
*/
uint8 ReadApiRadio(uint8 *dataDest, uint8 *source64) 
{    
    uint8 len,i,stop,sum;

TRY_AGAIN:
      readEM250();
      head = radioRxBufIndex;
    
//    switch (state){
      
//      case STATE_DELIM:
      if (state == STATE_DELIM){        
              while(API_DELIMITER != radioRxBuf[tail] && tail != head){//find API delimiter
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
              if ((uint8)(head - tail) < 3) { //do we have the length?
                return 0; //no abort
              }
              lengthApi = radioRxBuf[(uint8)(tail+LENGTH_OFFSET)];
              i = tail + 1;
              if (0 != radioRxBuf[i] || lengthApi > MAX_RX_PACKET_LENGTH) { //do we have an invalid length > ?? characters?
                tail++; //yes... invalid, Try Again;
                state = STATE_DELIM;
                goto TRY_AGAIN;
              }
              state = STATE_PACKET;
              return 0;
              
//      case STATE_PACKET:
      }else if (state == STATE_PACKET){
              if ((uint8)(head - tail) <= lengthApi + LENGTH_OFFSET +1) { //has the whole packet arrived
                return 0; // nope
              }
              
              state = STATE_VALID;
              goto TRY_AGAIN;//read serial port
              
//      case STATE_VALID:
      }else if (state == STATE_VALID){
              i = tail+RX_OPTIONS_OFFSET;
              frameType = radioRxBuf[tail+RX_FRAME_OFFSET];
              if (RX_FRAME_TYPE==frameType) {//if it is an RX frame type
                if (isFixed){
                  for(i=0; i < 8 ; i++){
                    if (fixedSourceAddress[i] != radioRxBuf[(tail+RX_SOURCE64_OFFSET+i)&0xFF]){//Is the address correct
                      tail+=lengthApi+4;//delete unwanted packet
                      state = STATE_DELIM;//reset state machine
                      goto TRY_AGAIN;        
                    }
                  }
                }else {
                 //good
                }
                lastRxOption = radioRxBuf[(tail+RX_OPTIONS_OFFSET)&0xFF];
              }else if (AT_FRAME_TYPE== frameType){
              }else{
                tail+=lengthApi+4;//delete unwanted packet
                state = STATE_DELIM;//reset state machine
                goto TRY_AGAIN;
              }
              
              state = STATE_CHECKSUM;
              goto TRY_AGAIN;
              
 //     case STATE_CHECKSUM:
      }else if (state ==  STATE_CHECKSUM){
              stop = tail + lengthApi + 3;//this may wrap
              sum = 0;
              for (i=tail + LENGTH_OFFSET+1;i != stop; i++){
                sum += radioRxBuf[i];
                readEM250();    
                
              }
              if (radioRxBuf[stop] != (0xFF-sum)){
                state = STATE_DELIM;//invalid checksum
                goto TRY_AGAIN;
              }
              state = STATE_DATA;
              goto TRY_AGAIN;
              
//      case STATE_DATA:
      }else if (state == STATE_DATA){
              stop = tail + lengthApi + 3;//this may wrap
              len = 0;
              if (RX_FRAME_TYPE==frameType) {//if it is an RX frame type
                 lastFrame = frameType;
                 for(i = tail+ RX_DATA_OFFSET; i != stop; i++){//copy data to buffer
                    dataDest[len] = radioRxBuf[i];     
                    len++;
                    readEM250();     
                  }//for
                  
                  if (source64) { //if pointer isn't null     
                    for(i=0; i <8; i++){//copy source address to buffer
                       source64[i] = radioRxBuf[(uint8)(tail+RX_SOURCE64_OFFSET+i)];
                    }
                  }
              }else if (AT_FRAME_TYPE== frameType){
                lastFrame = frameType;
                if ('N' == radioRxBuf[(uint8) (tail+COMMAND_OFFSET)] && 
                    'I' == radioRxBuf[(uint8) (tail+COMMAND_OFFSET+1)]){
                  for(i = tail+ AT_RX_DATA_OFFSET; i != stop; i++){//copy data to buffer
                      dataDest[len] = radioRxBuf[i];
                      len++;
                      readEM250();     
                  }//for 
                }else{
                  for(i = tail+ AT_RX_DATA_OFFSET; i != stop; i++){//copy data to buffer
                      dataDest[len] = ((radioRxBuf[i]>>4)&0xF) + '0';
                      if (dataDest[len]>'9'){
                        dataDest[len]+='A'-'9'-1;
                      }
                      len++;
                      dataDest[len] = (radioRxBuf[i]&0xF) + '0';     
                      if (dataDest[len]>'9'){
                        dataDest[len]+='A'-'9'-1;
                      }
                      len++;
                      readEM250();     
                  }//for 
                }//if ATNI
                dataDest[len] ='\r';
                len++; 
              }
              
              tail += radioRxBuf[(uint8)(tail+LENGTH_OFFSET)] +4;
              state = STATE_DELIM; 
              return len;
      }//switch or if (state)
    
    return 0;    

}





void InitApi(void){
  head = 0;
  tail = 0;
  state = STATE_DELIM;
  lengthApi = 0;
  isFixed = 0; //any address can talk to us
  SetTxDestination64(fixedSourceAddress); //initialize TX packets; The address doesn't matter during initialization

}



void readEM250(void){
    WDR(); 
//This part of the function should be uncommented if the UART is polled instead of interupt driven.
    /*
    if (SCI2S1_RDRF){
      radioRxBuf[head] = SCI2D; 
      head++;
    } */
}

