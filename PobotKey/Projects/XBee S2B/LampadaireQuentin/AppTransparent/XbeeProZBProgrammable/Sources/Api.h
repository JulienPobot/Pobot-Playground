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

#include "common.h"

#define MAX_TX_DATA_SIZE 80
#define TX_DEST64_OFFSET 5
#define TX_DEST16_OFFSET 13
#define TX_DATA_OFFSET 17
#define TX_FRAME_OFFSET 3
#define API_DELIMITER 0x7E


#define THROW_FIDO_A_BONE_SO_HE_DOESNT_RESET_US \
    __RESET_WATCHDOG();

/***************    variables               *******************************/

//use TX_DESTINATION_64_PTR
//    TX_DATA_PTR


/***************    Functions               ***********************/

char *GetTxDestination64ptr(void);
char *GetTxDataBuffer(void);

uint8 CalcChecksum(uint8 *bytes,uint8 length);

void SetTxDestination64(uint8 *dest64bits);
void SetTail(uint8 set_tail);

uint8 GetLastFrameType(void);
uint8 GetNumberOfCharsInBuff(void);
void  WriteTxDataRadioLen(uint8 length);
void  WriteCharTxDataRadio(uint8 data);

void  WriteAtCommandRadioLen(const uint8 *AtCommandPtr, const uint8 *dataPtr, uint8 length);
void  WriteAtCommandRadio(const uint8 *AtCommandPtr);
void  WriteAtCommandRadioByte(const uint8 *AtCommandPtr, uint8 data);
void  WriteAtCommandRadioUint16(const uint8 *AtCommandPtr, uint16 data);
void  CopyAtCommandToHost(void);

void FixRX64BitAddress(uint8 *address);

uint8 ReadApiRadio(uint8 *dataDest, uint8 *source64);

void InitApi(void);

//test
uint8 ReadApiRadio(uint8 *dataDest, uint8 *source64);
void readEM250(void);

uint8 WasBroadcast(void);
uint8 WasUnicast(void);
