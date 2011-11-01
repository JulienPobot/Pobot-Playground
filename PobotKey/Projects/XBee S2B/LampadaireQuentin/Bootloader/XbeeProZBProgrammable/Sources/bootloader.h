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


// these are for the divide clocks based on Clock/(2^(N+1))
#define DIVIDE_BY_2   0
#define DIVIDE_BY_4   1
#define DIVIDE_BY_8   2
#define DIVIDE_BY_16  3
#define DIVIDE_BY_32  4
#define DIVIDE_BY_64  5
#define DIVIDE_BY_128 6
#define DIVIDE_BY_256 7

/*****  DRS / DMX32 in ICSSC; Table for FLL
*
*   CPU_Clock should not exceed  60 MHz if VCC > 2.4v
*   CPU_Clock should not exceed  40 MHz if VCC > 2.4v and VCC < 2.1v
*
*  DRS  |  DMX32   | REF Multipy |  Range
*---------------------------------------     
*   0   |    0     |    512      |  Low
*   0   |    1     |    608      |  Low
*   1   |    0     |   1024      |  Mid
*   1   |    1     |   1216      |  Mid
*   2   |    0     |   1536      |  High
*   2   |    1     |   1824      |  High
*   3   |    x     |  Reserved   |  Reserved
* 
*/
/******** Bus Clock *******
*
*  Bus Clock = (CPU_Clock / 2) / (2^ICSC2_BDIV)
*  Bus Clock should not exceed 20 MHz
*
*/
 
//DRS in ICSSC These are for the FLL Reference Multiplier
#define MULTIPLY_BY_512  (0*ICSSC_DRST_DRS0_MASK) | (0*ICSSC_DMX32_MASK)
#define MULTIPLY_BY_608  (0*ICSSC_DRST_DRS0_MASK) | (1*ICSSC_DMX32_MASK)
#define MULTIPLY_BY_1024 (1*ICSSC_DRST_DRS0_MASK) | (0*ICSSC_DMX32_MASK)
#define MULTIPLY_BY_1216 (1*ICSSC_DRST_DRS0_MASK) | (1*ICSSC_DMX32_MASK)
#define MULTIPLY_BY_1536 (2*ICSSC_DRST_DRS0_MASK) | (0*ICSSC_DMX32_MASK)
#define MULTIPLY_BY_1824 (2*ICSSC_DRST_DRS0_MASK) | (1*ICSSC_DMX32_MASK)
#define FLL_REF

/********  CLKS in ICSC1; Clock Select *****
*
*  CLKS | Select
*  --------------
*    0   |  FLL
*    1   |  Internal Reference Clock 31250Hz
*    2   |  External Reference Clock 
*    3   |  Reserved
*/

#define CLOCK_SELECT_FLL      (0*ICSC1_CLKS0_MASK)
#define CLOCK_SELECT_INTERNAL (1*ICSC1_CLKS0_MASK)
#define CLOCK_SELECT_EXTERNAL (2*ICSC1_CLKS0_MASK)
//RDIV in ICSC1
#define REF_TO_FLL_DIVIDE_BY_1   (0*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_2   (1*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_4   (2*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_8   (3*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_16  (4*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_32  (5*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_64  (6*ICSC1_RDIV0_MASK)
#define REF_TO_FLL_DIVIDE_BY_128 (7*ICSC1_RDIV0_MASK)
//IREFS in ICSC1
#define FLL_REF_SELECT_INTERNAL  (1*ICSC1_IREFS_MASK)
#define FLL_REF_SELECT_EXTERNAL  (0*ICSC1_IREFS_MASK)
//IRCLKEN in ICSC1 enables clock for use with timers
#define ENABLE_ICSIRCLK  (1*ICSC1_IREFS_MASK)
#define DISABLE_ICSIRCLK  (0*ICSC1_IREFS_MASK)
//ENABLE_IREF_DURING_STOP in ICSC1 keeps internal clock running during stop for faster exit of sleep
#define ENABLE_IREF_DURING_STOP  (1*ICSC1_IREFSTEN_MASK)
#define DISABLE_IREF_DURING_STOP  (0*ICSC1_IREFSTEN_MASK)


//RANGE in ICSC2 affects the divide circuit for the External clock
#define RANGE_HIGH_EXTERNAL_DIVIDE_BY_32_FOR_FLL 1
#define RANGE_LOW 0


// System Setup 
//#define CLOCK_FREQ         (19029757)  //DCO Output with vInitClocks()    
/*
#define BUS_FREQ            (9491227)     //Bus_Clock with vInitClocks() & Bus_Clk /2
*/

#define INTERNAL_REFERENCE_FREQUENCY 31250
//#define EXTERNAL_REFERENCE_FREQUENCY 32768
#define FLL_MULTIPLIER 1216
#define BUS_CLOCK_DIVIDER 2



//#define BUS_FREQ          (19029757)//Reported Bus_Clock from Debugger with reading NVTRIM & NVFTRIM values
#define BUS_FREQ            (((0.+INTERNAL_REFERENCE_FREQUENCY)*FLL_MULTIPLIER)/BUS_CLOCK_DIVIDER)
#define UART_BAUD_9600      (BUS_FREQ/16)/9600+.5
#define UART_BAUD_19200     (BUS_FREQ/16)/19200+.5
#define UART_BAUD_38400     (BUS_FREQ/16)/38400+.5
#define UART_BAUD_115200    (BUS_FREQ/16)/115200+.5
/*
#define UART_BAUD_38400     (BUS_FREQ/16)/38400
#define UART_BAUD_9600      (BUS_FREQ/16)/9600
*/



#define FLASH_CLOCK         (BUS_FREQ / 8) 
// From Equation fFclk = fBus / (8 x (DIV + 1))
// fFclk must be between 150kHz & 200kHz
// choose fFclk = 180,000
#define FLASH_FCDIV_SETTING ( (unsigned char) (FLASH_CLOCK / 180000) - 1)


/*
#define UART_EM250  2
#define UART_HOST   1
*/
#define UART_EM250  1
#define UART_HOST   0


#define BL_VERSION_STRING_SIZE 18
#define BL_VERSION_HW_OFFSET    7

// Device ID defines.  
// IMPORTANT!!! Freescale uses the same ID numbers for multiple devices.
//              Digi defined IDs that can be used as indexes.
//              See idPart() for usage.
#define DEVID_32KB    0x00    // Typical part usage is 32KB so make it 0
#define DEVID_16KB    0x01
#define DEVID_64KB    0x02    //TODO
#define DEVID_96KB    0x03    //TODO
#define DEVID_128KB   0x04    //TODO

//-----------------------------------------------------------------------
typedef enum {
   OP_BYPASS = 0,
   OP_APP_COMMAND,    // DEBUG!!!
   OP_FLASH,
   OP_POST_FLASH,
   OP_IDLE,
   OP_NOP,
} OP_MODE;

