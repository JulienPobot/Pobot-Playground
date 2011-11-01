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

#define TICKS_1SECOND_H  0x48
#define TICKS_1SECOND_L  0x7AB7
#define TICKS_1mSECOND_L 0x4750


void SetTimer1uS(uint16 uSeconds);
void ResetTimer1(void);
char Timer1Expired(void);
void Wait1uS(uint16 uSeconds);

void SetTimer2Ticks(uint16 ticks);
void SetTimer2uS(uint16 uSeconds);
void ResetTimer2(void);
char Timer2Expired(void);

//based off Timer3
void SetTimer3AmS(uint16 mSeconds);
void ResetTimer3A(void);
char Timer3AExpired(void);
void SetTimer3BmS(uint16 mSeconds);
void ResetTimer3B(void);
char Timer3BExpired(void);
void WaitmS(uint16 mSeconds);


void InitTimers(void);

void InitKeyboardInterupt(void);
byte GetRadioType(void);
uint16 GetTimeInSeconds(void);






