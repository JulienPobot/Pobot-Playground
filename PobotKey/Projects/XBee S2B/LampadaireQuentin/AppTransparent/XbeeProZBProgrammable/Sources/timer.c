
#include <hidef.h>      /*!< for EnableInterrupts macro */
#include "derivative.h" /*!< include peripheral declarations */
#include "..\..\shared\common.h"
#include "pin_mapping.h"
#include "common.h"

//Bit mask for TimerFlag
#define TIMER1   1
#define TIMER2   2
#define TIMER3A  4
#define TIMER3B  8
#define RADIO_TYPE_UNKNOWN     0x0
#define RADIO_TYPE_COORDINATOR 0x1
#define RADIO_TYPE_ROUTER      0x2


static volatile byte    timerFlag;  /* bits are set by ISR */
static volatile int16   timer2TimeoutH; 
static volatile int16   timer2TimeoutL; 
static volatile int16   timer2TimeH;
static volatile int16   timer2TimeL;
static volatile int16   timer3ATimeout; 
static volatile int16   timer3ATime;
static volatile int16   timer3BTimeout; 
static volatile int16   timer3BTime;


static volatile byte    radioType;//coordinator or router 
static volatile uint16  associateTicksL;
static volatile int16   timer3StartTick;
static volatile int16   timer2RollOverTicks;






/****************************************************************************
*
*   NAME: setTimer1uS(uSeconds)    
*
*   \brief:  Initializes and starts TPM1: Range = 8uS to 13107uS
*            Anything under 8uS will be set to 3uS	
*
*   \details: Parameter sets how many micro seconds before timer expires
*             the number of ticks of the clock is calculated and set.
*             ticks = uSeconds*5 - 38; (since it takes 38 ticks to execute code)
*             Tick time is .210526us; TickTime * 5 = 1.05263uS
*             Time is 5.2% more than time specified 
*
******************************************************************************/
void SetTimer1uS(uint16 uSeconds)//this code takes ~6uS to execute + 2uS interupt or 38 ticks
{
  char Temp1SC;

  Temp1SC = TPM1SC;//Save Configuration of the Timer
  TPM1SC  = 0;//clear SC so MOD can be changed
    TPM1CNTH = 0; //any write will clear counter
  timerFlag &= ~TIMER1;// clear Flag 
  if (uSeconds & 0xFFF8){    
    TPM1MOD = (uSeconds<<2) + uSeconds - (38);//uSeconds*5 - 38ticks= ticks left
    //TPM1MOD = 1;
  }else{
    TPM1MOD = 1;//minimum time takes 3us
  }
  TPM1SC = Temp1SC | TPM1SC_TOIE_MASK;//enable interupts
}

/****************************************************************************
*
*   NAME: ResetTimer1(void)    
*
*   \brief:  Sarts TPM1 over again
*            This function is convenient when setting the Timer1 to more than 20 uS
*            
*
*   \details: Timer must already be set by setTimer1uS
*             If using this function, you should add 8 microSeconds to 
*             setTimer1uS otherwise the timer will go off 8uS sooner
*             
*
******************************************************************************/
void ResetTimer1(void)
{
  TPM1CNTH = 0; //any write will clear counter
  timerFlag &= ~TIMER1;// clear Flag
  TPM1SC_TOIE = 1;//enable the interupt again  
}

/****************************************************************
*
*   NAME: Wait_uS(uSeconds)    
*
*   \brief: Provides Delay.  	
*
*   \details:  Uses Timer1
*
*/
void Wait1uS(uint16 uSeconds)
{
  SetTimer1uS(uSeconds);
  while(! (timerFlag&TIMER1)){
    WDR();//reset the watchdog timer
  }   
}

/****************************************************************************
*
*   NAME: char timer1Expired(void)    
*
*   \brief:  Checks if timer is expired	
*
*   \details: Returns 0 if timer is not expired
*             Returns 1 if timer is expired
*
*
******************************************************************************/
char Timer1Expired(void)
{
  return timerFlag &TIMER1;// skip setup for 0uS and 1uS 
}

/****************************************************************************
*
*   NAME: SetTimer2Ticks(ticks)    
*
*   \brief:  Initializes and starts TPM2: Range = 1 to 65535 ticks. (0 ticks = 65536)
*
*   \details: Parameter sets how many ticks before timer expires
*             1 tick = 0.210526 uS;
*             
*
******************************************************************************/
void SetTimer2Ticks(uint16 ticksH, uint16 ticksL)
{ 
  int16 timer2Ticks,timer2RollOver;
  
  TPM2SC_TOIE = 0;//disableInterupt
  timer2Ticks = ((int16)TPM2CNT);
  timer2RollOver = timer2RollOverTicks;
  if (TPM2SC_TOF){//if there was a roll over get new readings
    timer2Ticks = ((int16)TPM2CNT);
    timer2RollOver++;//the interupt hasn't been taken care of yet
  }
  TPM2SC_TOIE = 1;//let interupt run
  
  timer2TimeL = ticksL;//save the time
  timer2TimeH = ticksH;//
  
  timer2TimeoutL= timer2Ticks + timer2TimeL;
  timer2TimeoutH = timer2RollOver + timer2TimeH;
  if(timer2Ticks < timer2TimeoutL){//was there a carry?
    timer2TimeoutH++;
  }
 timerFlag &= ~TIMER2;// clear Flag
}
/****************************************************************************
*
*   NAME: SetTimer2uS(mSeconds)    
*
*   \brief:  Initializes and starts TPM2: Range = 8 to 13907 microSeconds	
*
*   \details: Parameter sets how many micro seconds before timer expires
*             Time accuracy is 2.4% high 
*             
*
******************************************************************************/
void SetTimer2uS(uint16 uSeconds)
{
  int16 timer2Ticks,timer2RollOver;
  
  timer2TimeoutH=0;
  if (uSeconds > 8){
    timer2TimeL = (uSeconds<<2) + uSeconds - (38);// set timer     
  }else{    
    timer2TimeL = 1;//set for minimum time = 1uS
  }
  
  
  TPM2SC_TOIE = 0;//disableInterupt
  timer2Ticks = ((int16)TPM2CNT);
  timer2RollOver = timer2RollOverTicks;
  if (TPM2SC_TOF){//if there was a roll over get new readings
    timer2Ticks = ((int16)TPM2CNT);
    timer2RollOver++;//the interupt hasn't been taken care of yet
  }
  TPM2SC_TOIE = 1;//let interupt run
  timer2TimeoutL= timer2Ticks + timer2TimeL; 
  if(timer2Ticks < timer2TimeoutL){//was there a carry?
    timer2TimeoutH=1;
  }
  timerFlag &= ~TIMER2;// clear Flag 
}

/****************************************************************************
*
*   NAME: ResetTimer2(void)    
*
*   \brief:  Sarts TPM2 over again
*
*   \details: Timer must already be set by setTimer2uS or setTimer2Ticks
*             
*
******************************************************************************/
void ResetTimer2(void)
{
  int16 timer2TimeoutTemp;
  timer2TimeoutTemp = timer2TimeoutL;
  timer2TimeoutL += timer2TimeL;
  if(timer2TimeoutTemp < timer2TimeoutL){//check for carry
   timer2TimeoutH++;
  }
  timer2TimeoutH += timer2TimeH;
  timerFlag &= ~TIMER2;// clear Flag 
}

/****************************************************************************
*
*   NAME: char timer2Expired(void)    
*
*   \brief:  Checks if timer is expired	
*
*   \details: Returns 0 if timer is not expired
*             Returns 1 if timer is expired
*
*
******************************************************************************/
char Timer2Expired(void)
{
  if (timerFlag&TIMER2){
    return 1;// timer is expired
  }else{
    return 0;
  }
  
}

/****************************************************************************
*
*   NAME: SetTimer3AmS(mSeconds)    
*
*   \brief:  Initializes and starts TPM3: Range = 1mS to 32750 miliSeconds	
*
*   \details: Parameter sets how many micro seconds before timer expires
*             ticks = mSeconds;
*             Time accuracy is 2.4% high
*             Timer might expire up to 1mS early
*             TimerExpired() must be called at least once every 32 seconds
*             
*
******************************************************************************/
void SetTimer3AmS(uint16 mSeconds)
{
  if (mSeconds > 1){
    timer3ATime = mSeconds;// set timer     
  }else{    
    timer3ATime = 1;//set for minimum time = 1mS 
  }
  timer3ATimeout= ((int16)TPM3CNT) + timer3ATime; 
  timerFlag &= ~TIMER3A;// clear Flag
}

/****************************************************************************
*
*   NAME: ResetTimer3A(void)    
*
*   \brief:  Sarts TPM3 over again
*
*   \details: Timer must already be set by setTIMER3AmS
*             
*
******************************************************************************/
void ResetTimer3A(void)
{
  timer3ATimeout =((int16)TPM3CNT) + timer3ATime; 
  timerFlag &= ~TIMER3A;// clear Flag
}

/****************************************************************************
*
*   NAME: char Timer3AExpired(void)    
*
*   \brief:  Checks if timer is expired	
*
*   \details: Returns 0 if timer is not expired
*             Returns 1 if timer is expired
*
*
******************************************************************************/
char Timer3AExpired(void)
{
  if (timerFlag&TIMER3A || timer3ATimeout - ((int16)TPM3CNT) <= 0){//has timer expired
    timerFlag|=TIMER3A;//set TIMER3AFlag as expired
    return 1;
  }
  return 0; 
}
/****************************************************************************
*
*   NAME: SetTimer3BmS(mSeconds)    
*
*   \brief:  Initializes and starts TPM3: Range = 2mS to 32750 miliSeconds	
*
*   \details: Parameter sets how many micro seconds before timer expires
*             ticks = mSeconds;
*             Time accuracy is 2.4% high
*             Timer might expire up to 1mS early
*             TimerExpired() must be called at least once every 32 seconds
*             
*
******************************************************************************/
void SetTimer3BmS(uint16 mSeconds)
{
  if (mSeconds > 1){
    timer3BTime = mSeconds;// set timer     
  }else{    
    timer3BTime = 1;//set for minimum time = 1mS (1mS may be inaccurate 
  }
  timer3BTimeout= ((int16)TPM3CNT) + timer3BTime; 
  timerFlag &= ~TIMER3B;// clear Flag
}

/****************************************************************************
*
*   NAME: ResetTimer3B(void)    
*
*   \brief:  Sarts TPM3 over again
*
*   \details: Timer must already be set by setTIMER3AmS
*             
*
******************************************************************************/
void ResetTimer3B(void)
{
  timer3BTimeout =((int16)TPM3CNT) + timer3BTime; 
  timerFlag &= ~TIMER3B;// clear Flag
}

/****************************************************************************
*
*   NAME: char Timer3BExpired(void)    
*
*   \brief:  Checks if timer is expired	
*
*   \details: Returns 0 if timer is not expired
*             Returns 1 if timer is expired
*
*
******************************************************************************/
char Timer3BExpired(void)
{
  if (timerFlag&TIMER3B || timer3BTimeout - ((int16)TPM3CNT) <= 0){//has timer expired
    timerFlag|=TIMER3B;//set TIMER3AFlag as expired
    return 1;
  }
  return 0; 
}

/****************************************************************
*
*   NAME: WaitmS(mSeconds)    
*
*   \brief: Provides Delay.  	
*
*   \details:  Uses TIMER3A, but is independent of SetTIMER3AmS
*
*/
void WaitmS(uint16 mSeconds)
{
  int timeout;
  
  timeout = ((int16)TPM3CNT) + mSeconds; 
  while(! (timeout - ((int16)TPM3CNT) <= 0)){
    WDR();//reset the watchdog timer
  }   
}

/****************************************************************************
*
*   NAME: InitTimers    
*
*   \brief:  Initializes Timers TPM1,TPM2,TPM3	
*
*   \details: User provides uint16 iTicksToWait which is loaded
*             into the TPM1MOD register.  This becomes the number of Ticks
*             that the TPM1 Timer counts up to.  With ~20MHz Bus_Clock the
*             Timer 1 Tick is ~0.211us
*             Timer 2 Tick is ~0.211us
*             Timer 3 Tick is ~1.024ms
*
*
******************************************************************************/
void InitTimers(void)
{
  ICSC1_IRCLKEN  = 1;//enable internal clock for timing
  
/***********  Timer1 will be used for short times   ********************/
 
  TPM1SC_PS    = 2;//prescale = 2^(N+1)  (prescale / Reference Clock)(4/19MHz = .210526us tick time)
  TPM1SC_CLKSx = 1;//Clock Select 0=disable,1=bus clock, 2=system 32k clock,3 = External Clock  
  TPM1SC_TOIE  = 0;//0 = disable interupt
  TPM1CNTH = 0; //clear counter (any write will clear counter);


/***********  Timer2 can be for long or short times  ********************/
  TPM2SC       = 0;//this allows MOD to be Written
  TPM2MOD      = 0;//Free running clock must be polled
  TPM2SC_PS    = 2;//prescale = 2^(N+1)  (prescale / Reference Clock)(4/19MHz = .210526us tick time)
  TPM2SC_CLKSx = 1;//Clock Select 0=disable,1=bus clock, 2=system 32k clock,3 = External Clock
  TPM2SC_TOIE  = 1;//1 = Enable interupt
  TPM2CNTL = 0; //clear counter (any write will clear counter);
  timer2RollOverTicks = 0;

/***********  TIMER3 will be used for longer times  ********************/
  TPM3SC       = 0;//this allows MOD to be Written
  TPM3MOD      = 0;//Free running clock must be polled
  TPM3SC_PS    = 4;//prescale = 2^(N+1)  (prescale / Reference Clock)(32/31.25khz = 1.024mS tick time)
  TPM3SC_CLKSx = 2;//Clock Select 0=disable,1=bus clock, 2=system 32k clock,3 = External Clock
  TPM3SC_TOIE  = 0;//0 = disable interupt
  TPM3CNTL = 0; //clear counter (any write will clear counter);

  timerFlag = 0;//clear timer expired flags
}

/****************************************************************************
*
*   NAME: InitAssociateInterupt    
*
*   \brief:  Initializes Keyboard Interupt	
*
*   \details: Associate line from Radio can be used for more accurate time
*             A Coordinator changes the Associate line at a 1 second cycle (Actually 1.024Hz minus some delay)
*             Router/EndDevice changes the Associate line at a 1/2 second cycle
*
******************************************************************************/

void InitKeyboardInterupt(void)
{
   KB_INTERUPT2_DETECT_EDGE();
   IO_DIO5_ASSOCIATE_RISING_EDGE();
   KB_INTERUPT2_DISABLE(0xFF);//disable all
   KB_INTERUPT1_DISABLE();//disable all
   KB_INTERUPT2_ENABLE(IO_DIO5_ASSOCIATE_INTERUPT2);//G
   
   associateTicksL = -1;//next tick will clear to 0;
   radioType = RADIO_TYPE_UNKNOWN;
}

/****************************************************************************
*
*   NAME: GetRadioType(void)    
*
*   \brief:  Uses Associate timing to determine radio type	
*            returns 0 = unknown
*                    1 = coordinator
*                    2 = router/endDevice
*
*   \details: 
*              returns 1 or Coordinator if ticks are 500ms appart
*              returns 2 or Router      if ticks are 250ms appart
*              returns 0 or Unknown     if associate ticks < 1              
*          
******************************************************************************/
byte GetRadioType(void)
{
   return radioType;
}

/****************************************************************************
*
*   NAME: GetTicks(void)    
*
*   \brief:  Uses Associate timing to determine radio type	
*            returns Total ticks running.
*
*   \details: 
*                             
*          
******************************************************************************/
uint16 GetTicks(void)
{
  return associateTicksL;
}

/****************************************************************************
*
*   NAME: SetTicks(number)    
*
*   \brief:  Sets the associate tick counter to specified number
*            Returns the tick counter being replaced. 
*
*   \details: 
*                             
*          
******************************************************************************/
uint16 SetTicks(uint16 number)
{
  uint16 tempTicks;
  
  DisableInterrupts;
  tempTicks = associateTicksL;
  associateTicksL = number;
  EnableInterrupts;
  return tempTicks;
}

/****************************************************************************
*
*   NAME: GetAssociateTicks(void)    
*
*   \brief:  Uses Associate timing to determine radio type	
*            returns Total Seconds running.
*
*   \details: 
*            returns the number of ticks running
*
*            Router ticks are      = 1/1023*250 Seconds
*            Coordinator ticks are = 1/1023*500 Seconds
*                             
*          
******************************************************************************/

uint16 GetAssociateTicks(void)
{
  return associateTicksL;
}



/*****************************************************************************
*
*   Interupt Handlers   
*
******************************************************************************/

//Timer1 Overflow Handler
#pragma TRAP_PROC
void vTpm1Ovf (void)
{
  TPM1SC_TOIE  = 0;// disable further interupts
  timerFlag |= TIMER1;  //mark timer1 as expired
  TPM1SC_TOF = 0;  //clear Overflow
}

//Timer2 Overflow Handler
#pragma TRAP_PROC
void vTpm2Ovf (void)
{
  timer2RollOverTicks++;
  TPM2SC_TOF = 0;  //clear Overflow
}

//TIMER3 Overflow Handler
#pragma TRAP_PROC
void vTpm3Ovf (void)
{
  TPM3SC_TOIE  = 0;// disable further interupts
  timerFlag |= TIMER3A;  //mark TIMER3A as expired
  TPM3SC_TOF = 0;  //clear Overflow
}

/****************************************************************************
*
*   InteruptHandler: vKeyboard    
*
*   \brief:   
*
*   \details: keeps a tick for every time the associate line changes
*
******************************************************************************/
#pragma TRAP_PROC
void vKeyboard (void)
{
  if (KB_INTERUPT2_FLAG()){
    //determining coordinator or router if module is sleeping is not handled
    associateTicksL++;
    if (0 == associateTicksL){//if first tick or roll over set reference time
       timer3StartTick = TPM3CNT;//rough time
    }else{//if this isn't the first tick
      if (RADIO_TYPE_UNKNOWN == radioType){
         if (((int16)TPM3CNT) - timer3StartTick > ((250+500)>>1)){//average(router=250ms, coordinator = 500ms) 
           radioType =  RADIO_TYPE_COORDINATOR;
         }else{
           radioType = RADIO_TYPE_ROUTER;
         }
      }
    }
    IO_DIO5_ASSOCIATE_TOGGLE_EDGE();//look for next edge
    KB_INTERUPT2_CLEAR();
  }

  if (KB_INTERUPT1_FLAG()){
    KB_INTERUPT1_CLEAR();
    KB_INTERUPT1_DISABLE();//this one should be disabled
  }
  
  
}



