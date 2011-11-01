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

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A
#define FW_DOWNLOAD_CHAR 0x4F

#define PACKET_SIZE_BIG  128
#define PACKET_SIZE_SMALL 64

#include "build_defines.h"
#ifdef BL32
#define BUFFER_SIZE   PACKET_SIZE_BIG+3+2+100//SOH + PKTNUM + ~PKTNUM + CRC16 + 100 extra
#endif
#ifdef BL16
#define BUFFER_SIZE   PACKET_SIZE_BIG+3+2+1//SOH + PKTNUM + ~PKTNUM + CRC16 + 100 extra
#endif


#define MAXRETRANS 50


/*
#define SEND_CHARACTER_TO_EM250(sendThis) \
          while (!SCI2S1_TDRE);   \
          SCI2D = sendThis;       \
          
#define SEND_CHARACTER_TO_HOST(sendThis) \
          while (!SCI1S1_TDRE);   \
          SCI1D = sendThis;       \
*/
void setTimeoutShift(unsigned char timeoutS);
long xmodemReceive(unsigned short address, unsigned char uart);
void OUT_BYTE(unsigned char sendThis);

