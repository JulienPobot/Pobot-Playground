

#include <hidef.h>      /*!< for EnableInterrupts macro */
#include "derivative.h" /*!< include peripheral declarations */
#include "..\..\shared\common.h"     /*!< Common definitions for BL and APP */
#include "Serial.h"
#include "Timer.h"
#include "pin_mapping.h"
#include "System.h"

#pragma DATA_SEG SHARED_DATA
volatile APP_RESET_CAUSES AppResetCause @0x200;
volatile BL_RESET_CAUSES  BLResetCause  @0x204;
volatile uint16           AppString     @0x208;
volatile uint16           CmdMode       @0x20A;
#pragma DATA_SEG DEFAULT


void  InitGpio(void);
void  ResetEM250(void);
uint16 int GetADCVal(void);
void InitADC(void);
void vAdc(void);
void ResetHardware(APP_RESET_CAUSES Reset_Val);

uint16 lastADCvalue;

/************************************************************************
*
*   NAME: vInitGpio(void)   
*
*   \brief: Initializes appropriate MC9S08 GPIO that are connected to
*           to the EM250.
*      	
*   \details:  
*
*/
void InitGpio(void){
  char portE;
  
  PTADD = 0;//set all as inputs
  PTBDD = 0;//set all as inputs
  PTCDD = 0;//set all as inputs
  PTDDD = 0;//set all as inputs
  PTEPE = 0x70;//turn on Hardware detect pull-ups
  PTEDD = 0;//set all   as inputs
  portE = PTED & HARDWARE_MASK_PORTE;//read hardware detect lines
  
  PTAPE = 1<<6 | 1<<5 | 1<<4;//disable pullups except for no connects
  PTBPE = 0xC1;//3<<6+1;//disable pullups except for no connects and DIN
  PTCPE = 7<<2;//disable pullups except for no connects and RESET_XBEE
  PTDPE = 0xF;//disable pullups except for no connects
  PTEPE = portE | 0x8F;//disable pullups except for no connects
  
  IO_DIO7_CTS_HOST = 0;   //Assert CTS Line
  IO_DIO7_CTS_HOST_D = 1; //Set CTS as output to computer  
  
//SETUP DIO HERE  

  /*
  
  IO_DIO0_ADC0_COMMISSIONING_PE = 1;// Set pullups 
  //IO_DIO1_ADC1_PE = 1; 
  IO_DIO2_ADC2_PE = 1; 
  IO_DIO3_ADC3_PE = 1; 
  
  IO_DIO12_CD = 1;// Set IO Lines high for default 
  IO_DIO11_ADC11_PWM1 = 1; 
  
  IO_RTS_RADIO = 0;//assert RTS
  
  IO_DIO12_CD_D         = 1;// Set IO Lines as Outputs 
  IO_DIO11_ADC11_PWM1_D = 1; 
  
  IO_RTS_RADIO_D = 1;
  
  */
  
  // patte 19 en pull-down
  IO_DIO1_ADC1_PE = 0;
 
  // patte 19 en entrée
  IO_DIO1_ADC1_D = 0;
  
  // patte 19 à 0
  IO_DIO1_ADC1 = 0;
  
  
  
                       
  // patte 4 en sortie
  IO_DIO4_ADC4_D = 1; 
 
  // patte 4 en bas par défaut
  IO_DIO4_ADC4 = 0;
  


  
}
/****************************************************************
*
*   NAME: vResetEM250(void)   
*
*   \brief: Resets EM250
*
*   \details: Pulls RESET_EM250 line low for ~50ms and then releases	
*
*/
void ResetEM250(void)
{
  IO_RESET_RADIO_LOW();
  //wait ~52ms (EM250 spec says min 250nS...)
  WaitmS(52); 
  IO_RESET_RADIO_HIGH();
   
}

/****************************************************************
*
*   NAME: iGetADCVal(void)  
*
*  \brief:  Function returns the A/D value read	
*           0xFFFF is returned on error.
*
*
*/
uint16 GetADCVal(void){
  
  if (ADC_CONVERSION_ACTIVE){
     return 0xFFFF;//error
  }else{
     return lastADCvalue;
  }
}
/****************************************************************
*
*   NAME: iInitADC(void)   
*
*   \brief: Initializes the MC9S08 A/D converter	
*
*   \details: Configures MC9S08 A/D to use DIO0/ADP0, 12bit mode,
*             High Speed Mode, Long Sample Time, A/D Clock=BusClock,
*             Software Trigger, and Single Conversion.
*
*           
*/
void InitADC(void){
    //APCTLx Register Init
    APCTL1 = 0x01;           //Set PTA0/ADP0 to A/D Input, all other GPIO
    APCTL2 = 0x00;           //Set pins to GPIO
    //ADCCFG Register Init
    //With these settings input clock to A/D is ~5MHz = (Bus Clk/2)/4
    ADCCFG_ADLPC = 0;       //Set to High Speed 1=Low Power/Low Speed
    ADCCFG_ADIV  = 3;       //Divide clock by power(2,N);  3 = /8
    ADCCFG_ADLSMP = 1;      //Set to Long Sample Time
    ADCCFG_MODE =  1;      //MODE0,1 sets Resolution:0=8bit,1=12bit,2=10bit
    ADCCFG_ADICLK = 1;     //1 sets to Bus Clk/2
    
    
    ADCSC2 = 0;             //Select Software Trigger, Compare function disabled

}
    

/*******************************************************************************************
*
*   NAME: vResetHardware   
*
*   \brief:Resets the EM250 and passes control to the Bootloader. 
*
*   \details: After EM250 Reset writes to the shared memory variable(Bootloader/Application)
*            ,AppResetCause, to indicate to the Bootloader that it should do when control is 
*            passed to it after forcing a WDTimeout.
*	
*           
*/
void ResetHardware(APP_RESET_CAUSES Reset_Val){

    ResetEM250();
    DisableInterrupts;
    AppResetCause = Reset_Val;
    if (APP_CAUSE_REQUEST_SOPT1_NOT_WRITTEN ==  AppResetCause){
      BLResetCause = BL_CAUSE_NOTHING;
    }
    for(;;);           //Wait for WDTimeout
    
}

/****************************************************************
*
*   NAME: vAdc(void)   
*
*   \brief: A/D ISR	
*
*   \details: Stores 12bit A/D value in lastADCvalue
*           
*/
#pragma TRAP_PROC

void vAdc(void){
uint16 ADCVal;
     

     ADCVal = (int)ADCRH << 8; //read AD value
     ADCVal |= (int)ADCRL;
     
     lastADCvalue = ADCVal;     
}
