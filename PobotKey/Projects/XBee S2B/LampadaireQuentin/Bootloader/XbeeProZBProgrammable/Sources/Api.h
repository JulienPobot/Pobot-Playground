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

#define SEND_IN_LINE_CODE

#ifdef SEND_IN_LINE_CODE
#define SEND_CHARACTER_TO_HOST(sendThis) \
          while (!SCI1S1_TDRE);   \
          SCI1D = sendThis;       \
          
#define SEND_CHARACTER_TO_EM250(sendThis) \
          while (!SCI2S1_TDRE);   \
          SCI2D = sendThis;       \

#else
void SEND_CHARACTER_TO_HOST(unsigned char sendThis);
void SEND_CHARACTER_TO_EM250(unsigned char sendThis);
#endif


#define MAX_TX_DATA_SIZE 50
#define TX_DEST64_OFFSET 5
#define TX_DEST16_OFFSET 13
#define TX_DATA_OFFSET 17
#define TX_FRAME_OFFSET 3



/***************    variables               *******************************/

//use TX_DESTINATION_64_PTR
//    TX_DATA_PTR


/***************    Functions               ***********************/

char *getTxDestination64ptr(void);
char *getTxDataBuffer(void);

unsigned char ucCalcChecksum(unsigned char *bytes,unsigned char length);

void vSetTxDestination64(unsigned char *dest64bits);
void  vWriteTxDataSci2Len(unsigned char length);
void  vWriteCharTxDataSci2(unsigned char data);
void fixRX64BitAddress(unsigned char *address);
unsigned char ucReadApiSci2(unsigned char *dataDest, unsigned char *source64);
void vInitApi(void);
//unsigned char ucReadApiSci1(unsigned char *dataDest, unsigned char *source64);
void readEM250(void);


