/**************************************************************************************************
*  Application Transparent
*
*  Brief: Shows examples for API-Frames, and sampling  DIOs and ADCs.
*         Push the development board buttons and see transmissions of the data to the coordinator
*         Special menu shows examples of using the bootloader functions
*
*
*  Detailed: At startup the IO lines and UART are initialized
*            UART to HOST is polled
*            UART to RADIO is interrupt driven (256 byte buffer used)
*
*            (1) Transparent mode is the default mode.
*                In Transparent mode data from the HOST is converted into a API transmission
*                to the destination located on the RADIO parameters ATDH and ATDL
*                            
*            (2) Command mode
*                Command mode is entered by sending "+++" to the radio while in transparent mode
*                Command mode is similar to XBEE command mode, except all the API frames sent to
*                and from the radio are sent to the HOST.
*                ATCN\r needs to be sent to return to Transparent mode.(There is no timeout)
*                all other AT commands will be turned into an API command frame and sent to the RADIO
*                
*            (3) DIO and ADC
*                When a DIO line changes state the DIO lines are sent over the air to the destination
*                stored in the RADIO parameters ATDH and ATDL.
*                Additionally 2 ADC measurements are made and transmitted to the same destination
*                In order to sample an ADC one of the DIO lines must toggle to a different state.
*                
*            (4) Special Menu
*                The menu is reached by typing the "Exit\r" command in command mode
*                Or asserting a Break on the DIN line during a reset.
*                This menu shows how to:
*                      1. Call back into the bootloader
*                      2. Start an Over the Air update from a specific 64bit address
*                      3. Call bypass mode for the Local_UART to connect directly to the Radio_UART
*                         This is primarily to allow XCTU to update the firmware of the RADIO.
*                      4. Request that the bootloader not write a write once register so that Reset can
*                         disabled and used as an interrupt line, or Watchdog can be disabled etc.
*                      5. And More
*
*******************************************************************************************************/
#include <hidef.h>      /*!< for EnableInterrupts macro */
#include "derivative.h" /*!< include peripheral declarations */
#include "..\..\shared\common.h"   /*!< Common definitions for BL and APP */
#include <string.h>
#include <stdio.h>
#include "Serial.h"
#include "Timer.h"
#include "ExampleFuncs.h"
#include "Api.h"
#include "UARTandPassThru.h"
#include "sharedRAM.h"
#include "build_defines.h"
#include "pin_mapping.h"
#include "system.h"
#include "flashLT.h"


#define LENGTH_OFFSET  2
#define COMMAND_OFFSET 5
#define DATA_OFFSET    7
#define DEST64_OFFSET  5
#define DEST16_OFFSET  13
#define AT_FRAME_TYPE  0x88
#define APP_RESET_CUSTOM_ENTER_MENU 8
#define BUFFER_NOT_OVER_RUN 0xFF
#define OVER_THE_AIR_UPDATE_PASSWORD        "~OTA~" 
#define OVER_THE_AIR_UPDATE_PASSWORD_LENGTH  5
//ALREADY_API_PACKET uses first 2 characters of TXPACKET
#define ALREADY_API_PACKET TXPACKET

//the maximum buffer size is dependant on the Bootloader/Part being used
//definition BL32 means that this will be compiled for a 32k flash/2k RAM part using the Bootloader compiled for 32k
#ifdef BL32
 #define HOST_BUFFER_SIZE     182
 #define RX_RADIO_BUFFER_SIZE 100
 #define STRING_BUFFER_SIZE   30  
#endif
#ifdef BL16
 #define HOST_BUFFER_SIZE     50
 #define RX_RADIO_BUFFER_SIZE 55
 #define cPrintStrBuf hostUARTRxBuffer  
#endif
 

// Trying to keep the Application Version string in the same location
// pragma is set up to always have the string in the same locaion 
#pragma CONST_SEG APPLICATION_VERSION 
#ifdef BL32
 static const uint8 version[] = "APP32_Transparent_09.5";
#endif
#ifdef BL16
 static const uint8 version[] = "APP16_Transparent_09.4";
#endif


#pragma CONST_SEG DEFAULT
//these are in a fixed location for the bootloader and Application to reference
static const unsigned long pAppVersion @0x0000F1BC = (unsigned long)version;
#define iOtaHostAddr64 ((uint8*)0x20E)





//These are used to build API frames sent to the RADIO
//const uint8 ATCOMMAND[5] = {0x7E,0,0,8,1}  ;
const uint8 TXPACKET[17]  = {0x7E,0,0,0x10,2, 
                                     0,0,0,0,0,0,0xff,0xff, //64 bit address
                                     0xFF,0xFE,             //16 bit address (Unknown)
                                     0,                     //broadcast radius 0 =maximum
                                     0}  ;                  //Options 1 = disable ACK
const uint8 ATDH_ATDL[16] = {0x7E,0,4,8,3,'D','H',0x68,       //ATDH api request                                          
                                     0x7E,0,4,8,3,'D','L', 0x64 };    //ATDL api request
const uint8 ATDH_RESP[8] = {0x7E,0,9,0x88,3,'D','H',0};       //ATDH Response                                    
const uint8 ATDL_RESP[8] = {0x7E,0,9,0x88,3,'D','L',0}  ;      //ATDL Response
const uint8 BROADCAST[8] = {0,0,0,0,0,0,0xff,0xff};                                     

const uint8 DIO_STATE[3] = {'I','O','='}  ;      //Used to send and receive IO states
const uint8 ADC_STATE[3] = {'A','D','C'}  ;      //Used to send ADC states
const uint8 CR[] = "\r";
const uint8 ILLEGAL_INSTRUCTION[] = { 0x8d,0x8d,0x8d,0x8d };
const uint8 COORDINATOR_ADDRESS_0[] = {0,0,0,0,0,0,0,0};
const uint8 NOTHING_MESSAGE[] = "NOTHING";  

static bool  atDHDLRequestTimeout;
static uint8 hostRxBufferIndex;
static uint8 hostUARTRxBuffer[HOST_BUFFER_SIZE]; //buffer used for characters received from the HOST
static uint8 *radioUARTRxApiDataBuf;             //data buffer from API.c used for sending transmissions
static uint8 radioUARTRxDataBuf[RX_RADIO_BUFFER_SIZE];//takes the data after the API frame is removed
static uint8 radioRxBufferTail;
static bool  echoOn;
static bool  showApi;
static bool  cmdMode;
static bool  dioEnabled;
static uint8 dioState;//holds DIO readings
static uint8 dioPreviousState;//stores the previous DIO reading
//static uint8 ioStateBuf[8];//use to send DIO and ADC readings (8 bytes leaves 1 extra) 
static uint8 dest64[8];    //holds the 64 bit destination address (thus 8 bytes)
#ifndef BL16
volatile uint8 cPrintStrBuf[STRING_BUFFER_SIZE];
#endif
static CONFIG_FLASH_STRUCT *flashStorage;

//rrm local funcs
void  PrintAppVersion(void);
void  CheckResetCause(void);
void  ProcessReceivedRadioData(void);
void  CheckForBufferOverflow(void);
void  ProcessHostDataForTransmission(void);
void  ProcessCommand(void);
void  InitMain(void);
void  PrintBLResetCause(void);
void  PrintInHexToHost(const char* ptr,uint8 length);//Print the new Destination Address(void); 
void  InitClocks(void);
long  GetApplicationVersionString(void);
void  TransmitData(uint8 *ptr, uint8 len, uint8* dest64);
void  GetDHDL(void);
void  CheckDIO(void); 
uint8 GetDioStates(void);
void  SetDioStates(uint8 states);
char  CmpText(const uint8 *ptr1,const uint8 *ptr2,uint8 len);
char  AsciiHexToNumber(const uint8 *sourcePtr,uint8 *destPtr, uint8 length);
void  SpecialMenu(void);
void  PrintMenu(void);
void  UpperCase(char *ptr,uint8 length);
bool  WriteReservedFlashBlock(const char *blockPtr);


/****************************************************************
*
*   NAME:  main() 
*
* \brief:  Main Entry into program	
*
* \note:   // Characters from Host can be dropped since it is Polled
*
*****************************************************************/
void main(void) {
  WDR();

  InitClocks();//change uP speed to 39.85Mhz
  InitTimers();
  InitGpio();
  InitADC();
  InitUARTs();
  DisableHostUARTIsr();
  EnableRadioUARTIsr();
  InitKeyboardInterupt();
  EnableInterrupts; 

  PrintAppVersion();
  CheckResetCause();
  InitMain();
  PrintMenu();
  for(;;){ //*************************************** Loop Forever *************************
    // When Timer3B expires in transparent mode a transmission is sent 
    WDR();
    if (0==hostRxBufferIndex){ // if no characters are in the buffer 
      ResetTimer3B();
    }
    
    if (dioEnabled){
      CheckDIO();
    }
    
    // programme spécifique
    
    
    
    // afficher des leds
    
    
    
    
    
    if ( HOST_UART_HAS_CHAR() || (Timer3BExpired()  && !cmdMode && hostRxBufferIndex>0)){//check for byte from computer
      if (HOST_UART_HAS_CHAR()){
        hostUARTRxBuffer[hostRxBufferIndex] = HOST_UART_READ();
        if (echoOn){
          WRITE_CHAR_TO_HOST(hostUARTRxBuffer[hostRxBufferIndex]); 
        }
      }else{  
        hostRxBufferIndex--;//skip read
      }   
      WDR();
      //if carrage return or timer timeouts then process command or transmission
      if ((hostUARTRxBuffer[hostRxBufferIndex] == CR[0]) ||  
          (Timer3BExpired() && !cmdMode) ||    
          (hostRxBufferIndex > 4 && CmpText(ALREADY_API_PACKET,hostUARTRxBuffer,2)==0 && hostRxBufferIndex >= hostUARTRxBuffer[LENGTH_OFFSET]+3) ){//Already an api packet? just pass it through
    
        if(API_DELIMITER != hostUARTRxBuffer[0] && cmdMode){
          ProcessCommand();
        }else{
          ProcessHostDataForTransmission();
        }  
      }else{
        hostRxBufferIndex++;
      }//if
    }//if
    CheckForBufferOverflow();
    ProcessReceivedRadioData();  
  }//end forever loop
}

/****************************************************************
*
*   NAME:  PrintAppVersion(void) 
*
* \brief:  Prints the version of this App.	
*
* \note:   // 
*
*****************************************************************/
void PrintAppVersion(){
  if( GetApplicationVersionString() != 0){
    if(sprintf(cPrintStrBuf, "\r\rApp Version: %s\r",(char*)(unsigned long*)pAppVersion ) != '0'); 
    WriteToHostUART((char*)cPrintStrBuf);
  }
}
  
/****************************************************************
*
*   NAME:  CheckResetCause(void) 
*
* \brief:  Checks for the reset reason	
*
* \note:   // 
*
*****************************************************************/
void CheckResetCause(void)
{
  uint16 time = 0;
  PrintBLResetCause();
  if (SRS_COP) { 
    WriteToHostUART("Watchdog Timeout Reset\r");
  }
  if (SRS_PIN) { 
    WriteToHostUART("Reset Button Pushed\r");
  }
  if (SRS_POR) { 
    WriteToHostUART("Power On Reset\r");
    time = 500;//wait 500m seconds for Ember to start from power on
  } 
  if (SRS_ILOP) { 
    WriteToHostUART("Illegal Instruction Reset\r");
  }
  if (APP_CAUSE_REQUEST_SOPT1_NOT_WRITTEN==AppResetCause){//Special Menu Option 'C' was selected
    WriteToHostUART("\rByPass Mode Started.");
    WriteToHostUART("\rUse RESET Button/Line to reset EMBER radio.");
    WriteToHostUART("\rTurn power off and on to reset Freescale.");
    //Bootloader normally sets SOPT1 to ensure watchdog timeout and the Reset Pin resets the module
    //We will disable Reset Pin for this Bypass mode so APP_CAUSE_REQUEST_SOPT1_NOT_WRITTEN was requested
    SOPT1 = SOPT1_BKGDPE_MASK | SOPT1_STOPE_MASK | SOPT1_COPE_MASK | SOPT1_COPT_MASK;//disable Reset pin
    DoPassThroughMode();
  }
  if (APP_RESET_CUSTOM_ENTER_MENU==AppResetCause || !IO_DIN_HOST){//then run special menu
    SpecialMenu();
    dioEnabled = 0;//disable  DIO
  }
  WaitmS(time);
}

/****************************************************************
*
*   NAME:  OutputRxDataToHost(void) 
*
* \brief:  Outputs Data Received from Radio to Host	
*
* \note:   // 
*
*****************************************************************/
void OutputRxDataToHost(uint8 length)
{
  char i;
  
  for(i=0;i<length;i++){
    WRITE_CHAR_TO_HOST(radioUARTRxDataBuf[i]);
  }
  if (GetLastFrameType() == AT_FRAME_TYPE && GetNumberOfCharsInBuff()){
    if (!atDHDLRequestTimeout){//if DHDL request failed last time wait before trying again
      GetDHDL();//update the DHDL if any AT command was sent and no more characters are left in the buffer.
    }else{
      atDHDLRequestTimeout++;
      if (atDHDLRequestTimeout>2){
         GetDHDL();//try again 
      }
    }
  }
}

/****************************************************************
*
*   NAME:  ProcessReceivedRadioData(void) 
*
* \brief:  Checks on received packets and responses from Radio	
*
* \note:   // 
*
*****************************************************************/
void ProcessReceivedRadioData(void)
{
  uint8 length;
  
  WDR();
  
  //this section displays the API packet if showAPI is enabled and in command mode
  if (radioRxBufferTail != (uint8)radioRxBufIndex){//if characters received from radio       
    if (showApi && cmdMode){
      WriteToHostUARTLen((uint8 *)(radioRxBuf+radioRxBufferTail),1); // just pass it on
    }
    radioRxBufferTail++;
  }//if
  
  
  
  //this section translates the API packet into just data
  length = ReadApiRadio(radioUARTRxDataBuf, 0);
  if (length){
    if (5==length && CmpText(DIO_STATE,radioUARTRxDataBuf,3)==0){//DIO readings
      WDR();
      SetDioStates(radioUARTRxDataBuf[3]);
      OutputRxDataToHost(length);//send DIO readings received out UART
    }else if (length>=7 && CmpText(ADC_STATE,radioUARTRxDataBuf,3)==0){//ADC readings
      OutputRxDataToHost(length);//send ADC readings received out UART
    }else if (OVER_THE_AIR_UPDATE_PASSWORD_LENGTH==length && WasUnicast() && CmpText(OVER_THE_AIR_UPDATE_PASSWORD,radioUARTRxDataBuf,OVER_THE_AIR_UPDATE_PASSWORD_LENGTH)==0){
      ResetHardware(APP_CAUSE_BOOTLOADER_MENU);//Return to bootloader
    }else{
      OutputRxDataToHost(length);                  
    }
    
  
    // montrer qu'on a reçu quelque chose
    
    WriteToHostUARTLen("allume",6);
    
    IO_DIO4_ADC4 = 1;
    
    // attendre
    WaitmS(3000);
    
    IO_DIO4_ADC4 = 0;
                     
    WriteToHostUARTLen("eteint",6);
    
  
  }
}

/****************************************************************
*
*   NAME:  CheckForBufferOverflow(void) 
*
* \brief:  Makes sure that the Host Rx buffer didn't overflow	
*          If it does the buffer is purged.
* \note:   // 
*
*****************************************************************/
void CheckForBufferOverflow(void)
{ 
  if (hostRxBufferIndex > HOST_BUFFER_SIZE-2) {
    WriteToHostUART("RX buffer overflow\rExit Command Mode\r");
    hostRxBufferIndex = 0;
    cmdMode = 0;
  }//if
  if (radioUARTRxDataBuf[RX_RADIO_BUFFER_SIZE-1] != BUFFER_NOT_OVER_RUN){
    WriteToHostUART("\rradioUARTRxDataBuff data buffer overflow!!!\r");
    WriteToHostUART("Increase RX_RADIO_BUFFER_SIZE!!!\r");
    for(;;);//watchdog reset radio to reinit variables
  }
}

/****************************************************************
*
*   NAME:  ProcessHostDataForTransmission(void) 
*
* \brief:  Takes bytes sent in from Host and creates an API 	
*          Transmission packet to send to the Radio.
* \note:   // 
*
*****************************************************************/
void ProcessHostDataForTransmission(void)
{
  WDR();
  if (CmpText(hostUARTRxBuffer, "+++",3) == 0){
    cmdMode = 1;//enter command mode
    WriteToHostUART("OK\r");
    PrintMenu();
  }else if (hostRxBufferIndex>4 && CmpText(ALREADY_API_PACKET,hostUARTRxBuffer,2)==0  && cmdMode){
    WriteToRadioUARTLen(hostUARTRxBuffer, hostRxBufferIndex+1);//send api packet straight to Radio
  }else{
    //Start the creation of TX packet
    if (cmdMode){
      TransmitData(&hostUARTRxBuffer[1], hostRxBufferIndex-1,dest64);//exclude CR for command mode transmissions
    }else{
      TransmitData(&hostUARTRxBuffer[0], hostRxBufferIndex+1,dest64);//include CR for transparent mode
    }
  }
  hostRxBufferIndex = 0;//clear buffer
}

/****************************************************************
*
*   NAME:  UpperCase(char *ptr,uint8 length) 
*
* \brief:  Converts characters to Upper Case
*          This function modifies the original data
*
*
*****************************************************************/
void UpperCase(char *ptr,uint8 length){
  char i;
  
  for(i = 0; i <= length; i++){
    if(ptr[i]>='a' && ptr[i]<='z'){
      ptr[i] = ptr[i] - ('a' - 'A');//Change all characters to upper case.
    }
  }
}

/****************************************************************
*
*   NAME:  PrintInHexToHost(const char *ptr,uint8 length) 
*
* \brief:  Converts each nibble to Ascii Hex and sends it to Host
*
*
*****************************************************************/
void PrintInHexToHost(const char *ptr,uint8 length){
  char i;
  
  for(i=0;i<length;i++){
    if ((ptr[i]>>4)>=0xA){
      WRITE_CHAR_TO_HOST((ptr[i]>>4)-0xA+'A');
    }else{
      WRITE_CHAR_TO_HOST((ptr[i]>>4)+'0');
    }
    if ((ptr[i]&0xF)>=0xA){
      WRITE_CHAR_TO_HOST((ptr[i]&0xF)-0xA+'A');
    }else{
      WRITE_CHAR_TO_HOST((ptr[i]&0xF)+'0');
    }
  } 
}

/****************************************************************
*
*   NAME:  ProcessCommand(void) 
*
* \brief:  Takes characters sent from Host and executes the command 	
*          ATcommands are converted to an API packet
*
*          All API packets are sent to both the Host and Radio
*          This way the Host user can see the packet that was formed
*          And the response from the Radio.
*
*****************************************************************/
void ProcessCommand(void)
{
  char i;
 
  WDR();
  UpperCase(hostUARTRxBuffer,hostRxBufferIndex);
  WDR();
  if (hostRxBufferIndex>=4 && CmpText("ATCN", hostUARTRxBuffer,4)==0){
    GetDHDL();
    cmdMode=0;//exit command mode
    PrintInHexToHost(dest64,8);//Print the new Destination Address
    WriteToHostUART("\rTransparent mode started\r");
  }else if (CmpText("AT",hostUARTRxBuffer,2)==0 && hostRxBufferIndex >=4 ){//process AT command
    if ((hostRxBufferIndex > 5) || (hostRxBufferIndex > 4 && hostUARTRxBuffer[4] != ' ' )){
      if   (((CmpText("NI", &hostUARTRxBuffer[2],2)==0) || 
             (CmpText("DN", &hostUARTRxBuffer[2],2)==0)
            )&& hostRxBufferIndex>5 )
      { //if ATNI or ATDN then don't translate hex values, send straight Data
        if (' ' == hostUARTRxBuffer[4] ){
           WriteAtCommandRadioLen(&hostUARTRxBuffer[2],&hostUARTRxBuffer[5],hostRxBufferIndex-5);
        }else{
           WriteAtCommandRadioLen(&hostUARTRxBuffer[2],&hostUARTRxBuffer[4],hostRxBufferIndex-4);
        }
      }else{//translate hex data
        i = 4;
        if (hostUARTRxBuffer[4] == ' ') i++;//adjust for space
        WriteAtCommandRadioLen(&hostUARTRxBuffer[2],&hostUARTRxBuffer[i],AsciiHexToNumber(&hostUARTRxBuffer[i],&hostUARTRxBuffer[i], hostRxBufferIndex-i));
      }//if ATNI
    }else{//AT command without extra data
      WriteAtCommandRadioLen(&hostUARTRxBuffer[2],0,0);
    }//if
    if (showApi){
      CopyAtCommandToHost();
    }
  }else{ //*********************************************************** Other commands  **************
    WDR();
    if (4 == hostRxBufferIndex && ( CmpText("EXIT", hostUARTRxBuffer, 4) == 0 || CmpText("MENU", hostUARTRxBuffer, 4) == 0)){
      SpecialMenu();
    }else if ('E' == hostUARTRxBuffer[0] && (1 == hostRxBufferIndex)){
      echoOn = !echoOn;
      if (echoOn){
         WriteToHostUART("\rEcho On\r");
      }else{
         WriteToHostUART("\rEcho Off\r");
      }
    }else if ('A' == hostUARTRxBuffer[0] && (1 == hostRxBufferIndex)){
      showApi = !showApi;
      if (showApi){
         WriteToHostUART("\rShow Api On\r");
      }else{
         WriteToHostUART("\rShow Api Off\r");
      }
    }else if ('D' == hostUARTRxBuffer[0] && (1 == hostRxBufferIndex)){
      dioEnabled = !dioEnabled;
      if (dioEnabled){
         WriteToHostUART("\rDIO Passing On\r");
      }else{
         WriteToHostUART("\rDIO Passing Off\r");
      }       
    }else if (('?' == hostUARTRxBuffer[0] && hostRxBufferIndex>=1) || (CmpText("HELP",hostUARTRxBuffer,4)==0 && hostRxBufferIndex>=4)){ 
      hostRxBufferIndex = 0;
      PrintMenu();   
    }else{
      if (hostRxBufferIndex>0) {
        WriteToHostUART("Invalid AT command\r");
      }else{
      
      }
    }
  }//if
  hostRxBufferIndex = 0;//clear buffer
}
/****************************************************************
*
*   NAME:  ValidateFlash(void) 
*
* \brief:  Initializes global variables for main 	
*
*****************************************************************/
bool EraseReservedFlashBlock(void)
{
  bool error = 0;
  DisableInterrupts;
  error |= erase(APP_RESERVED_FLASH_START_ADDRESS);//erase first block of reserved flash  (512bytes)
  error |= erase(APP_RESERVED_FLASH_START_ADDRESS+0x200);//erase second block of reserved flash if needed(512bytes)
  WDR();
  EnableInterrupts;
  
  return error;
}
/****************************************************************
*
*   NAME:  ValidateFlash(void) 
*
* \brief:  Initializes global variables for main 	
*
*****************************************************************/
bool WriteReservedFlashBlock(const char *blockPtr)
{
  bool error = 0;
  DisableInterrupts;
  WDR();
  error |= erase(APP_RESERVED_FLASH_START_ADDRESS);//erase first block of reserved flash  (512bytes)
  WDR();
  //error |= erase(APP_RESERVED_FLASH_START_ADDRESS+0x400);//erase second block of reserved flash if needed(512bytes)
  error |= program((uint16)&(flashStorage->message),(uint16)blockPtr,MESSAGE_SIZE);
  error |= programByte((uint16)&(flashStorage->zero),0);//program zero = 0;
  error |= programByte((uint16)&(flashStorage->checksum),CalcChecksum((char*)flashStorage,sizeof(CONFIG_FLASH_STRUCT)-1));//program checksum  
  EnableInterrupts;
  return error;
}
/****************************************************************
*
*   NAME:  ValidateFlash(void) 
*
* \brief:  Initializes global variables for main 	
*
*****************************************************************/
void ValidateFlash(void)
{
  if (CalcChecksum((char*)flashStorage,sizeof(CONFIG_FLASH_STRUCT)-1) != flashStorage->checksum ||    //if bad checksum
      flashStorage->zero != 0                                                               ) {//or invalid zero marker
    //Erase Corrupt App Storage Area
    WriteToHostUART("Reserved Flash is Erased or Corrupted\r");
    WriteToHostUART("Reserved Flash is now set to default\r");
    if(memcpy(cPrintStrBuf,NOTHING_MESSAGE,20));//make a copy in RAM (can't access Flash during erase and writes)
    if(WriteReservedFlashBlock((char*)cPrintStrBuf)){//put a default message in the reserved flash
      WriteToHostUART("ERROR during Flash Write\r");
    }
    
  }
}
                        
/****************************************************************
*
*   NAME:  InitMain(void) 
*
* \brief:  Initializes global variables for main 	
*
*****************************************************************/
void InitMain(void)
{
  char i;

  WDR();
  SetTail(radioRxBufIndex); //init Api.c
  radioRxBufferTail = radioRxBufIndex;
  showApi = 1;//show API packets in Transparent mode
  cmdMode = 0;//transparent mode
  echoOn = 0;//echo off
  dioEnabled = 1;   //default is to have DIO example enabled
  dioPreviousState = (~GetDioStates()) & 0xF;
  SetTimer3BmS(500);//set timeout for 1/2 second

  
  for(i=0;i<6;i++){//initialize destination to broadcast
    dest64[i]=0;
  }
  dest64[6]=0xff;
  dest64[7]=0xff;
  GetDHDL();
  radioUARTRxApiDataBuf = GetTxDataBuffer();
  radioUARTRxDataBuf[RX_RADIO_BUFFER_SIZE-1]=BUFFER_NOT_OVER_RUN;
  atDHDLRequestTimeout = FALSE;
  flashStorage = (CONFIG_FLASH_STRUCT*)APP_RESERVED_FLASH_START_ADDRESS;
  ENABLE_FLASH_ERASE_WRITE_UNTIL_RESET();
  CopyInRAMFlashFunctions();//make the flash read and write functions available
  ValidateFlash();
}    
    
    
/****************************************************************
*
*   NAME:  PrintBLResetCause(void) 
*
* \brief:  After any reset, the Bootloader will increment or change
*           BLResetCause. This displays the lower byte of that number
*           in Hex.
*          
*          Any reset will increment the number unless cleared by the
*          Bootloader or the App.
*            	
*          After 16 unauthorized watchdog resets the Bootloader
*          will consider the App BAD and stay in the Bootloader.
*
*****************************************************************/
void PrintBLResetCause(void) 
{
    WriteToHostUART("BLResetCause = ");
    PrintInHexToHost((char*)&BLResetCause,2);
    WriteToHostUART(CR);  
}    



/*************************************************************************************
*
*   NAME: NumberToAsciiHex   
*
*   \brief:  converts Big Endian Number to Ascii
*            returns the number of bytes in the Ascii Hex number.
*            
*
*   \details:
*            For every 1 byte 2 ascii bytes are formed
*             
*                         
**************************************************************************************/
char NumberToAsciiHex(const uint8 *sourcePtr,uint8 *destPtr, uint8 length){
  uint8 i;
  uint8 len=0;  
  
  for(i = 0; i < length; i++){//copy data to buffer
    destPtr[len] = ((sourcePtr[i]>>4)&0xF) + '0';
    if (destPtr[len]>'9'){
      destPtr[len]+='A'-'9'-1;
    }
    len++;
    destPtr[len] = (sourcePtr[i]&0xF) + '0';     
    if (destPtr[len]>'9'){
      destPtr[len]+='A'-'9'-1;
    }
    len++;     
  }//for 
  
  return len;
  
}

/*************************************************************************************
*
*   NAME: AsciiHexToNumber()   
*
*   \brief:  converts ASCII typed Hex number to Hex Big Endian Number
*            returns the number of bytes in the Hex number.
*            
*
*   \details:
*            For every 2 characters of ascii forms into 1 character of Big Endian Number
*            This function does not force the size to standard number sizes like 1, 2, 4.
*                 3,5,6,7 byte numbers are valid as well.
*            Hex numbers 'A' to 'F' must be upper case
*	           Stops conversion on the first invalid char
*             
*                         
**************************************************************************************/
char AsciiHexToNumber(const uint8 *sourcePtr,uint8 *destPtr, uint8 length){
  uint8 i=0;
  char count;
  uint8 ch;
  
  for(count=0;count<length;count++){
    ch = sourcePtr[0];
    if ((ch >= '0') && (ch<= '9')){ 
    }else if ((ch >= 'A') && (ch<= 'F')){
    }else{
      break;//bad character encountered
    }
  }
  
  length = count;
  count = 0;//count will now count the number of bytes in the returned number
  
  if (length & 1){//take care of odd number of hex digits
    ch = sourcePtr[0];
    if ((ch >= '0') && (ch<= '9')){
      destPtr[0] = ch & 0xF; 
    }else if ((ch >= 'A') && (ch<= 'F')){
      destPtr[0] = (ch & 0xF) + 9;
    }
    sourcePtr++;
    destPtr++;
    count++;
  }
  
  length &= 0xFE;
  for(i = 0;i<length;i+=2){
    ch = sourcePtr[i];
    if ((ch >= '0') && (ch<= '9')){
      destPtr[i>>1] = ch & 0xF; 
    }else if ((ch >= 'A') && (ch<= 'F')){
      destPtr[i>>1] = (ch & 0xF) + 9;
    }else{
      break;//terminate loop
    }//if 
    ch = sourcePtr[i+1];
    destPtr[i>>1] = destPtr[i>>1]<<4;
    if ((ch >= '0') && (ch<= '9')){
      destPtr[i>>1] += (ch & 0xF); 
    }else if ((ch >= 'A') && (ch<= 'F')){
      destPtr[i>>1] += (ch & 0xF) + 9;
    }else{
      break;//terminate loop
    }//if
    count++;//2 nibbles = 1 byte
  }
  
  return count;
  
}


/*************************************************************************************
*
*   NAME: CmpText()   
*
*   \brief:  compares text, returns the subtraction of the two if not equal
*            returns 0 if equal
*            returns >0 if ptr1>ptr2
*            returns <0 if ptr1<ptr2
*                         
**************************************************************************************/
char CmpText(const uint8 *ptr1,const uint8 *ptr2,uint8 len){
  uint8 i;
  char retValue =0;
  
  for(i=0;i<len;i++){
    retValue =(char)(ptr2[i]-ptr1[i]);
    if (retValue){
      break;
    }
  } 
  return retValue;
}

/*************************************************************************************
*
*   NAME: TransmitData()   
*
*   \brief:  Takes Data from parameter and creates an API packet and sends
*            it to the Destination Radio.	
* 
*   \details:   
*                         
**************************************************************************************/
void TransmitData(uint8 *ptr, uint8 len, uint8 *dest64){
  uint8 i;          

  WDR();
 //Start the creation of TX packet
 //the front of the TX API packet is 17 characters
  for(i = 0;i<17;i++){
    radioUARTRxApiDataBuf[i] = TXPACKET[i];
  }//for
  for(i = 0;i<len;i++){ 
    radioUARTRxApiDataBuf[i+17] = ptr[i];
  }//for

  radioUARTRxApiDataBuf[LENGTH_OFFSET] = 17 - 3 + len;//-3 because the length does not include Delim, Length[2]
  
  for(i=0; i<8; i++){
    radioUARTRxApiDataBuf[DEST64_OFFSET+i] = dest64[i];
  }

  WDR();
  radioUARTRxApiDataBuf[radioUARTRxApiDataBuf[LENGTH_OFFSET]+3] = CalcChecksum(radioUARTRxApiDataBuf+3,radioUARTRxApiDataBuf[LENGTH_OFFSET]);//calculate checksum
  if (showApi && cmdMode){
    //Send to the User a copy of the API frame Created
    WriteToHostUARTLen(radioUARTRxApiDataBuf,radioUARTRxApiDataBuf[LENGTH_OFFSET]+4);//+4 because the delim[1] + length[2] + checksum[1]
  }
  WDR();
  //Send transmission to the EMBER RADIO            
  WriteToRadioUARTLen(radioUARTRxApiDataBuf,radioUARTRxApiDataBuf[LENGTH_OFFSET]+4);//+4 because the delim[1] + length[2] + checksum[1]
}

/*************************************************************************************
*
*   NAME: GetDHDL()   
*
*   \brief:  sends 2 API packets to Radio to determine Destination address	
* 
*   \details:   
*                         
**************************************************************************************/
void GetDHDL(void){ 
  uint8 i,temp;
  
  atDHDLRequestTimeout = FALSE;//clear timeout flag
  temp = cmdMode;
  cmdMode = 1;
  WriteToRadioUARTLen(ATDH_ATDL,16);
   
  WaitmS(10);//at 9600 baud that should be 10 characters
  SetTimer3AmS(100);//set timeout to 100mS
  while (radioRxBufferTail == (uint8)radioRxBufIndex){//wait for first char
    WDR();
    if(Timer3AExpired()){
      goto EXIT_DHDL;
    }
  }
  SetTimer3AmS(400);//set timeout
  for(;;){//do forever until break           
    while (radioRxBuf[radioRxBufferTail] != API_DELIMITER){//continue if in the middle of a packet
      if(Timer3AExpired()){
        atDHDLRequestTimeout = TRUE;
        WriteToHostUART("Timeout on DHDL response\r");
        goto EXIT_DHDL;
      }
      if (radioRxBuf[radioRxBufferTail] != API_DELIMITER){
        if (showApi && temp){
         WriteToHostUARTLen((uint8 *)radioRxBuf+radioRxBufferTail,1); //just pass it on       
        }
        radioRxBufferTail++;
        //radioRxBufferTail &= 0xFF;  
      }//if
       WDR();
    }//while
    //Grab response from ATDH and ATDL
    WDR();

    while (((uint8)((int8)radioRxBufIndex) -(int8)radioRxBufferTail) <26){//the response should be 26 characters
      if(Timer3AExpired()){
        atDHDLRequestTimeout = TRUE;
        WriteToHostUART("Timeout on DHDL response_\r");
        goto EXIT_DHDL;
      }
      WDR();
    }

    for(i=0;i<8;i++){
        if (radioRxBuf[(radioRxBufferTail+i)&0xFF] != ATDH_RESP[i]) break;
    }//for
    if (8 == i ) { // good packet get data
      for (;i<12;i++){
        dest64[i-8] = radioRxBuf[(radioRxBufferTail+i)&0xFF]; 
      }
      radioRxBufferTail +=13;//move to next api frame
      for(i=0;i<8;i++){
          if (radioRxBuf[(radioRxBufferTail+i)&0xFF] != ATDL_RESP[i]) break; 
      }//for
      if (8 == i ) { // good packet get data
        for (;i<12;i++){
          dest64[i-4] = radioRxBuf[(radioRxBufferTail+i)&0xFF]; 
        }
        radioRxBufferTail +=13;//move to next api frame
      }else{
        radioRxBufferTail+=radioRxBuf[LENGTH_OFFSET]+4;
      }
       
    }else{
      radioRxBufferTail++;
      if (radioRxBufferTail == (uint8)radioRxBufIndex){
        break;
      }else{
        continue;//try again
      }
    }//if
    break;
  }
EXIT_DHDL:            
  SetTail(radioRxBufferTail);//this makes the ATDH/ATDL not be pushed out to the UART HOST
  cmdMode = temp;
}

/****************************************************************
*
*   NAME:  PrintMenu() 
*
* \brief:  displays Command Menu	
*
*****************************************************************/
void PrintMenu(void)
{
  hostRxBufferIndex = 0;
  WriteToHostUART("\rHELP MENU:");
  WriteToHostUART((uint8 *)version);
  WriteToHostUART("\r'?' or 'HELP' displays HELP MENU in CMD Mode");
  WriteToHostUART("\rType 'AT' before any AT commands");
  WriteToHostUART("\rType 'ATCN' to exit Freescale CMD Mode");
  WriteToHostUART("\rType '+++' to enter CMD Mode");
  WriteToHostUART("\r");
  WriteToHostUART("\rCommands for CMD Mode:");    
  WriteToHostUART("\r'~'    Use the character to transmit a packet.");
  WriteToHostUART("\r       while still in command mode.(~Hello\\r)");
  WriteToHostUART("\r       The packet will be transmitted to the");
  WriteToHostUART("\r       address stored in ATDH,ATDL");
  WriteToHostUART("\r'E'    turns character echo on or off.");
  WriteToHostUART("\r'A'    turns RX Api display on or off.");
  WriteToHostUART("\r'D'    turns DIO passing on or off.");
  WriteToHostUART("\r'EXIT' or 'MENU' leaves command mode and");
  WriteToHostUART("\r       starts special menu for updates etc.");
  WriteToHostUART("\r");
  WriteToHostUART("\r'~OTA~' sent in 1 unicast packet causes");
  WriteToHostUART("\r        the App to return to the bootloader");
  WriteToHostUART("\r        so an OTA update can be initiated.");
  WriteToHostUART("\r        This command is case sensitive.");

  if (!cmdMode){
     WriteToHostUART("\rTransparent mode started");
     WriteToHostUART("\rType +++ to start using the menu.");
  }
  WriteToHostUART("\r");
  
  GetDHDL();
  PrintInHexToHost(dest64,8);//Print the Destination Address;
  WriteToHostUART("\r");
}

/****************************************************************
*
*   NAME:  GetHexAddressForOTAUpdate() 
*
* \brief:  Requests the 64bit address from the Host
*          Verifies that it is the correct address
*          Then starts an OTA update from that specific address.	
*
*****************************************************************/
void GetHexAddressForOTAUpdate(void)
{
  char i,RxCharHost;
  do{
    WriteToHostUART("\rEnter Hex 64bit Address sending Update\r>");
    for(i = 0;i<16;){
      while(SCI1S1_RDRF == 0){
        WDR();
      }//Wait to Receive UART Char
      RxCharHost = SCI1D;
      UpperCase(&RxCharHost,1);
      if ((RxCharHost>='0' && RxCharHost<='9') ||
          (RxCharHost>='A' && RxCharHost<='F') ){
        WRITE_CHAR_TO_HOST(RxCharHost);
        iOtaHostAddr64[i>>1] = iOtaHostAddr64[i>>1]<<4;//shift previous nibble out
        if (RxCharHost>='0' && RxCharHost<='9'){
          iOtaHostAddr64[i>>1] += RxCharHost - '0'; //add nibble if 0 to 9
        }else{
          iOtaHostAddr64[i>>1] += RxCharHost - 'A'+ 0xA; //add nibble if A to F
        }
        i++;//increment i
        if (i==8){
          WRITE_CHAR_TO_HOST(':');
        }
      }//if
    }//for
    if (CmpText(iOtaHostAddr64,COORDINATOR_ADDRESS_0,8)!=0){//address 0 isn't supported            
      WriteToHostUART("\rIs 64bit address 0x");
      PrintInHexToHost(iOtaHostAddr64,8);
      WriteToHostUART(" correct?(y=yes,q=quit)");
      while(SCI1S1_RDRF == 0){//Wait to Receive Usr Char
        WDR();
      }
      RxCharHost = SCI1D;
      UpperCase(&RxCharHost,1);

    }else{
      WriteToHostUART("\r64 bit address 0x0 is not supported.");
      WriteToHostUART("\rPlease type in the actual coordinator address.");
      WriteToHostUART("\rUse ATSH and ATSL to determine the address.");
      RxCharHost = 'Q';
    }
  }while(RxCharHost != 'Y' && RxCharHost != 'Q');
  
  if ('Y' == RxCharHost){
    WriteToHostUART("\rStarting Update with XMODEM\r ");  
    ResetHardware(APP_CAUSE_FIRMWARE_UPDATE);
  }
}

/****************************************************************
*
*   NAME:  SleepModule() 
*
* \brief:  This puts the Freescale into a low power state.
*          It waits for the Associate Line to toggle.
*	
*          The Ember can be placed in Pin Sleep mode "ATSM 1\rATAC\r"
*          This will allow the DTR line to sleep the Ember.
*          The freescale can then enter Sleep until the DTR line wakes
*          the Ember.  The Ember will then Wake the Freescale.
*
*****************************************************************/
void SleepModule(void){
  const char RADIO_SM_1[] = {0x1};

  WriteAtCommandRadioLen("SM",RADIO_SM_1,1);//Send ATSM1 to Ember (Pin Sleep)
  //WriteAtCommandRadio("AC");//Send ATAC to Ember (Apply Changes)
  WriteToHostUART("\rDe-assert DTR line to sleep the Radio\r");
  
  while(!IO_DIO8_DTR){//wait for DTR to go High = De-asserted
    WDR();//make sure we don't reset
  }
  WriteToHostUART("Waiting for Ember to sleep.\r");
  while(IO_DIO9_ADC9_ON_SLEEP){//when ON/SLEEP line goes low the Ember is sleeping
    WDR();//make sure we don't reset
  }
  WriteToHostUART("Raise DTR line to Wake Ember.\r");
  WriteToHostUART("Ember will wake Freescale.\r");
  
  IO_DIO7_CTS_HOST = 1;//De-assert CTS
  SPMSC1 = 0;//turn off low voltage detection
  
  //an ASSOCIATE pin interrupt will wake the module
  STOP_MODE();
  IO_DIO7_CTS_HOST = 0;//assert CTS to show Freescale is awake   
  WriteToHostUART("\r\r\r\rFreescale is now awake.\r");
  WriteToHostUART("DTR line will still sleep Ember.\r");
  WriteToHostUART("Menu option 'S' will sleep Freescale.\r");
  WaitmS(500);
}
/****************************************************************
*
*   NAME:  GetNewMessageForFlash() 
*
* \brief:  reads from the user a new string to store in Flash                  	
*
*****************************************************************/
void GetNewMessageForFlash(void){
  uint8 i;
  
  WriteToHostUART("Enter a new message to store:\r");
  //get new string to store
  for(i=0;i<MESSAGE_SIZE;i++){
    while(!HOST_UART_HAS_CHAR()){
      WDR();
    }
    hostUARTRxBuffer[i] = HOST_UART_READ();
    if ('\r' == hostUARTRxBuffer[i]) break;
  }
  hostUARTRxBuffer[i] = 0;//terminate string
  
  //write new message in Reserved Flash Area
  if(WriteReservedFlashBlock(hostUARTRxBuffer)){
    WriteToHostUART("\r ERROR during Flash Write:");
  }
  WriteToHostUART("\rNew Message is NEVER erased by the BOOTLOADER\r");
  
}

/****************************************************************
*
*   NAME:  SpecialMenu() 
*
* \brief:  Enters a special menu to show how to interact
*          with the bootloader and show more advanced functions
*          that were not implemented in command mode                    	
*
*****************************************************************/
void SpecialMenu(void){
  char RxCharHost,exitMenu;

  exitMenu = 0;
  AppResetCause = 0; 
  while(!exitMenu)
  {
    WriteToHostUART("\rMENU:");
    WriteToHostUART("\rA  to enter Bootloader");
    WriteToHostUART("\rB  to enter Bootloader ByPass mode");
    WriteToHostUART("\r      (RESET will Reset Freescale)");
    WriteToHostUART("\rC  to enter Custom ByPass Mode");
    WriteToHostUART("\r      (RESET Button Resets EMBER)");
    WriteToHostUART("\r      (Power Off to reset Freescale)");
    WriteToHostUART("\rF  for an OTA Flash Update");
    WriteToHostUART("\rI  to cause an Illegal Instruction Reset");
    WriteToHostUART("\rR  to force a Watchdog Reset");
    WriteToHostUART("\rS  to sleep module");
    WriteToHostUART("\rV  to Read APP Reserved Flash");
    WriteToHostUART("\rW  to Write APP Reserved Flash");
    WriteToHostUART("\rE  to Erase APP Reserved Flash");
    WriteToHostUART("\rQ  to Quit this menu\r>");

    while(SCI1S1_RDRF == 0){//Wait to Receive Usr Char
      WDR();
    }
    RxCharHost = SCI1D;
    if(RxCharHost>='a' && RxCharHost<='z'){
      RxCharHost = RxCharHost - ('a' - 'A');//Change to upper case.
    }
    WRITE_CHAR_TO_HOST (RxCharHost);
    
    switch (RxCharHost){
      case 'A' : ResetHardware(APP_CAUSE_BOOTLOADER_MENU);//never returns
      case 'B' : ResetHardware(APP_CAUSE_BYPASS_MODE);//never returns
      case 'C' : ResetHardware(APP_CAUSE_REQUEST_SOPT1_NOT_WRITTEN);//never returns
      case 'R' : WriteToHostUART("\rCausing Watchdog Reset");
                 for(;;);//watchdog reset
      case 'I' : __asm JMP ILLEGAL_INSTRUCTION;//never returns
      case 'F' : GetHexAddressForOTAUpdate();
                 break;
      case 'S' : SleepModule();
                 break;                      
      case 'V' : WriteToHostUART(CR);
                 WriteToHostUART(flashStorage->message);
                 WriteToHostUART(CR);
                 break;                      
      case 'W' : GetNewMessageForFlash();
                 break;
      case 'E' : EraseReservedFlashBlock();                                            
                 break;
      case 'Q' : exitMenu = 1;//exit menu
                 break;           
      default  : //exitMenu = 1;//exit menu
                 break;
    }//switch
  }//while
  
}

/*************************************************************************************
*
*   NAME: InitClocks(void)   
*
*   \brief:  Configures MC9S08 Internal Clocks 	
* 
*   \details: Reads factory calibrated values stored in NVTRIM and NVFTRIM to ensure 
*             accurate 32kHz Reference Clock.  Otherwise Clock can vary as much as 30%
*             Produces a 38MHz DCO output which in turn provides ~19MHz Bus Clock.   
*                         
**************************************************************************************/
void InitClocks(void){
  
//-----------------------------------------
// Internal clock is calibrated to 31.25kHz NVICSTRM and NVFTRIM
// Internal clock can drift around 1% with temperature
//
    
  ICSTRM = NVICSTRM;//set internal clock to default 31.25kHz

/*****  DRS / DMX32 Table for FLL
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
*/ 

//System Clock
  ICSSC = MULTIPLY_BY_1216 | NVFTRIM ;//Set FLL for Mid range value 31.25kHz*1216=38MHz
  ICSC1 = CLOCK_SELECT_FLL | FLL_REF_SELECT_INTERNAL | ENABLE_ICSIRCLK | DISABLE_IREF_DURING_STOP;
//Bus clock  
  ICSC2_BDIV = DIVIDE_BY_2;//Clock divide by 2 for bus clock = 38MHz/2 = 19MHz
   
}

/*************************************************************************************
*
*   NAME: GetApplicationVersionString(void)   
*
*   \brief:  Prints out the Application Version String	
* 
*   \details: 
*                         
**************************************************************************************/
long GetApplicationVersionString(void){
  long length;
 
  length = strlen((char*)(unsigned long *)pAppVersion);
  return length;
}

/*************************************************************************************
*
*   NAME: CheckDIO(void)   
*
*   \brief:  Checks for a change in DIO states and sends a transmission if needed	
* 
*   \details: 
*                         
**************************************************************************************/
void CheckDIO(void) 
{
//  char i;
//  uint16 result;
  
  WDR();
  dioState = GetDioStates();//read all the dio lines
   
  
  
  if (dioState != dioPreviousState){//has there been a change?
  
    // il s'est passé quelque chose
    // je préviens les voisins          
  
       
    if (IO_DIO1_ADC1) {
      
      // prévient le ou les autres
      
      TransmitData("salut",5,dest64);
    
      // et s'allume lui-même
      
      IO_DIO4_ADC4 = 1;
    
       // attendre
       WaitmS(3000);
      
       IO_DIO4_ADC4 = 0;
    
    
    
    }
  
  
    //DIO Transmission
    dioPreviousState = dioState;//make new state the dioPreviousState
    
    
    
    /*
    
    for(i=0;i<3;i++){//prepare Characters to transmit  
      ioStateBuf[i] = DIO_STATE[i];
    }
    ioStateBuf[3] = dioState;
    ioStateBuf[4] = CR[0];
    TransmitData(ioStateBuf,5,dest64);//Transmit DIO reading

    //ADC1 reading and transmission
    StartADC(IO_ADC1); //start ADC sample
    //other things can be done while we wait
    while(ADC_CONVERSION_ACTIVE);//wait for conversion to finish
    result = GetADCVal();
    ioStateBuf[4]=(uint8) (result>>8);//put ADC reading into buffer
    ioStateBuf[5]=(uint8) (result & 0xFF);

    for(i=0;i<3;i++){//put "ADC" in string  
      ioStateBuf[i] = ADC_STATE[i];
    }
    ioStateBuf[3]='1';//string now equals "ADC1xx" xx is the ADC reading
    ioStateBuf[6]=CR[0];
    TransmitData(ioStateBuf,7,dest64);
    
    //ADC2 reading and transmission
    StartADC(IO_ADC2);//Start another sample
    while(ADC_CONVERSION_ACTIVE);//wait for conversion to finish
    result = GetADCVal();
    ioStateBuf[4]=(uint8) (result>>8);//put ADC reading into buffer
    ioStateBuf[5]=(uint8) (result & 0xFF);
    ioStateBuf[3]='2';//"ADC2xx"
    TransmitData(ioStateBuf,7,dest64);  
    
    */
      
  }
}


/*************************************************************************************
*
*   NAME: GetDioStates(void)   
*
*   \brief:  Prints out the Application Version String	
* 
*   \details: 
*                         
**************************************************************************************/
uint8 GetDioStates(void){
  uint8 newDioState=0;
  
  // que la patte 19
  if (IO_DIO1_ADC1){
    newDioState++;
  }
  
  /*
  newDioState<<=1;
  if (IO_DIO2_ADC2){
    newDioState++;
  }
  newDioState<<=1;
  if (IO_DIO3_ADC3){
    newDioState++;
  }
  newDioState<<=1;
  if (IO_DIO6_RTS_HOST){
    newDioState++;
  }
  */
  return newDioState;     
}

/*************************************************************************************
*
*   NAME: SetDioStates()   
*
*   \brief:  sets the IO lines that go to the LEDs
* 
*   \details: 
*                         
**************************************************************************************/
void SetDioStates(uint8 states){
  uint8 mask=0x8;  
  if (states&mask){
    IO_DIO12_CD = 1;
  }else{
    IO_DIO12_CD = 0;
  }
  mask >>=1;
  if (states&mask){
    IO_DIO11_ADC11_PWM1= 1;
  }else{
    IO_DIO11_ADC11_PWM1 = 0;
  }
  mask >>=1;
  if (states&mask){
    IO_DIO4_ADC4 = 1;
  }else{
    IO_DIO4_ADC4 = 0;
  }
  mask >>=1;
  if (states&mask){
    IO_DIO7_CTS_HOST = 1;
  }else{
    IO_DIO7_CTS_HOST = 0;
  }
}




