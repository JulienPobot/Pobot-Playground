/******************************************************************************

MA-VIN header file for winavr.

URL - www.hitecrobotics.de
mail - info@hitecrobotics.de

all rights reservered. hitec robotics/multiplex modellsport GmbH & Co. KG 2007
             
******************************************************************************/


#ifndef  __NEWBEETLE_H__
#define  __NEWBEETLE_H__

#include <avr/signal.h> 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>


#define cbi(sfr, bit)      (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit)      (_SFR_BYTE(sfr) |= _BV(bit))
#define inb(sfr)           _SFR_BYTE(sfr)
#define outb(sfr, val)   (_SFR_BYTE(sfr) = (val))
#define inw(sfr)           _SFR_WORD(sfr)
#define outw(sfr, val)   (_SFR_WORD(sfr) = (val))
#define outp(val, sfr)   outb(sfr, val)
#define inp(sfr)           inb(sfr)
#define BV(bit)            _BV(bit)
#define PRG_RDB	 	  pgm_read_byte


//타입정의
typedef unsigned char  u08;
typedef          char  s08;
typedef unsigned short u16;
typedef          short s16;
typedef u08            bool;


/////////////////////////////////////////////////////////////////////////////
// DEFINE

#define TRUE    1
#define FALSE   0

#define SELECT       1
#define UNSELECT     0


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
//	
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Remocon

#define IRRxPORT		PINF		
//#define IR_RX_PIN		7			

#define IRTxPIN			PINF				
#define IRTxPORT		PORTF
//#define IR_TX_PIN		7			
 
 
unsigned char IR_RX_PIN;
unsigned char IR_TX_PIN;

/////////////////////////////////////////////////////////////////////////////
//	Bulldozer  


#define Right 0xfe
#define Left 0xfd
volatile unsigned char ControlPortc = 0;			// volatile                               
								

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////
char string1[8];		// 2007.4.18
char string2[8];		// 2007.4.18
static unsigned char text1,text2;


#define LCD_RS 0  //Register Selector
#define LCD_RW 1  //Read, Write
#define LCD_E  2  //Enalble signal

#define LCD_CONTROL PORTG
#define LCD_DATA    PORTA
#define LCD_C_INOUT DDRG
#define LCD_D_INOUT DDRA

//LCD_Control_Instruction
#define LCD_SET_8 0x38 //8bit, 2Lines, 5*8
#define LCD_SET_4 0x28 //4bit, 2Lines, 5*8
#define LCD_D_ON  0x0c //Display On, Cusor, Blink Off
#define LCD_D_CLR 0x01 //Clear Display
#define LCD_E_N   0x06 //Entry mode Set(Normal)
#define LCD_R_SHF 0x1c //Display Right Shift
#define LCD_L_SHF 0x18 //Display Left Shift

//LCD_Parameter
#define LCD_SIZE  8    //8*2 LCD
#define LCD_C_NUM 8    //LCD CG Ram Num
#define LCD_DOT   8    //LCD dot 5*8
#define LCD_RIGHT 0
#define LCD_LEFT  1


////////////////////////////////////////////////////////////////////////////////

/*
#define ILLUMINANCE_LV1	4		
#define ILLUMINANCE_LV2	6
#define ILLUMINANCE_LV3	8
#define ILLUMINANCE_LV4	15
*/

#define ILLUMINANCE_LV1	30		
#define ILLUMINANCE_LV2	80		
#define ILLUMINANCE_LV3	120		
#define ILLUMINANCE_LV4	150	


//마이크
#define MIC_LEVEL_0   200
#define MIC_LEVEL_1   210
#define MIC_LEVEL_2   220
#define MIC_LEVEL_3   230
#define MIC_LEVEL_4   240
#define MIC_LEVEL_5   250

//ADC channel
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//PORTF
#define AD_0			0	
#define AD_1			1	
#define AD_2			2	
#define AD_3			3	
#define AD_4			4	
#define M4_4_AD			5	
#define MIC_IN			6	
#define V_TEST			7

#define DDRF_OUTPUT	((1<<AD_0)|(1<<AD_1)|(1<<AD_2)|(1<< AD_3)|(1<<AD_4)|(1<<M4_4_AD)|(1<<MIC_IN)|(1<<V_TEST))		
#define DDRF_INPUT	~DDRF_OUTPUT	

#define ADC_CHANNEL(ch)		outp(ch,ADMUX)		//ADC Channel Select

volatile u08 ADCComplete;						
u08 ADCData[2], AD_Value;						


u08 fMicSample;    
u08 fSpeakerOut;   
u08 fcdsSample;		

//포토센서
//////////////////////////////////////////////////////////
#define SENS_FR		0
#define SENS_FC		1
#define SENS_FL		2
#define SENS_BR		3
#define SENS_BC		4
#define SENS_BL		5
#define SENS_TRIG	6
#define IR_RX		7

#define FORWARD     1   
#define DOWNWARD    0   

u08 nSensorVal;         
u08 SensorDir;          
u08 PhotoSensor;        

u08 ForwardPhotoSensor;
u08 DownwardPhotoSensor;	

//전방센서
#define PHOTO_ALL_OFF				0	//
#define PHOTO_RIGHT                 1	//
#define PHOTO_CENTER                2	//
#define PHOTO_CENTER_RIGHT          3	//
#define PHOTO_LEFT                  4	//
#define PHOTO_LEFT_RIGHT            5	//
#define PHOTO_CENTER_LEFT           6	//
#define PHOTO_LEFT_CENTER_RIGHT     7	//

////////////////////////////////////////////////////////////////////////////
//FND Display
//1번
#define HC595_DATA_1	0
#define HC595_CLK_1		1
#define HC595_LATCH_1	2

//2번
#define HC595_DATA_2	0
#define HC595_CLK_2		1
#define HC595_LATCH_2	2
//3번
#define HC595_DATA_3	4
#define HC595_CLK_3		5
#define HC595_LATCH_3	6

//4번
#define HC595_DATA_4	0
#define HC595_CLK_4		1
#define HC595_LATCH_4	2

//5번
#define HC595_DATA_5	7
#define HC595_CLK_5		3
#define HC595_LATCH_5	4

//////////////////////////////////////////////////////////////////////////////////////////
//DC motor

#define CW		1
#define CCW		0

#define MOTOR_RIGHT		1
#define MOTOR_LEFT		2
#define MOTOR_BOTH		3
////////////////////////////////////////< 20060921 >
volatile u08 cur_pwm[2] = {0,0};
volatile u08 org_pwm[2] = {0,0};
volatile u08 pwm_cycle = 17; 
volatile u08 pwm_cnt = 0;
////////////////////////////////////////////////

u16 tempCounter;

#define DEFAULT		0
#define POSITION_1	1
#define POSITION_2	2
#define POSITION_3	3
#define POSITION_4	4
#define POSITION_5	5

 
u08 IrKeyValue[15];
u08 ir_trans_temp[4];

volatile u08 OnlyIrCnt = 0;

volatile u08 IRReg = 0x10;
#define IrTimeFLAG				0x40

//-------------------------------------------------
volatile u08  	T_count, T_data ;
volatile u08	ir_data[4];
volatile u08	MultiCommData;

#define SET_DETECT		0x10
#define CLEAR_DETECT	0x00

#define set_d_FLAG		0x10
#define clear_d_FLAG 	0x20

#define IrRxdFLAG		0x80

#define MULTIGAMEFLAG	0x02
#define MULTIRXDFLAG	0x01

#define VolumCtrlFLAG	0x08

volatile u08 ir_tran_data[4] = {0xcc,0x33,0xf0,0x0f};
volatile u08 RComFlagReg = 0;

#define flag_16T			0x01
#define flag_8T				0x02
#define flag_Trans_32		0x04
#define flag_bit_high		0x08
#define flag_bit_low		0x10
#define flag_ir_trans_bsy	0x20
#define ExclRxFLAG			0x40
#define ExclTxFLAG			0x80

#define flag_ir_trans_start	(flag_16T|flag_ir_trans_bsy)

#define RE_FRONT_KEY	 0x02
#define RE_REAR_KEY		 0x10
#define RE_RIGHT_KEY	 0x20
#define RE_LEFT_KEY		 0x08
#define RE_STOP_KEY		 0xa0
#define RE_SEND_KEY		 0x81
#define RE_EXIT_KEY		 0x82
#define RE_SET_KEY		 0x84
#define RE_SOUND_KEY	 0x88
#define RE_GAME_KEY		 0x90
#define RE_L_KEY		 0x01
#define RE_R_KEY		 0x04
#define RE_CUSTOMSET_KEY (0xa0|0x84)
#define CUSTOMSET	(0xa0|0x84)

u08 InfSenValue, IDmemory, TempExCustom, RobotComState, RCommCnt, PinniCmd;

////////////////////////////////////////////////////////

//스피커 정의부
#define ON		1
#define OFF		0

//스피커 모듈...1번 
//////////////////////////////////////////////////////////////////
//PORTD
#define L4816S_D	1		//LM4861 chip select
#define L4816S_D_2	5		//LM4861 chip select
//PORTB	
#define DacSYNC		0		//SYNC
#define DacSCLK		1		//SCK 사용
#define DacDIN		2		//MOSI 사용

//
#define SOUND_CLOSE()		sbi(PORTD,L4816S_D)
#define SOUND_OPEN()		cbi(PORTD,L4816S_D)

#define SOUND_CLOSE_2()		sbi(PORTD,L4816S_D_2)
#define SOUND_OPEN_2()		cbi(PORTD,L4816S_D_2)

////////////////////////////////////////////

#define DacTriSTATE			0x30
#define Dac1KToGND			0x10
#define Dac100KToGND		0x20
#define DacNORMAL			0x00

#define DAC_CLOSE(pos)				DacDigToAlog(pos,0, DacTriSTATE)
#define DAC_OUTDATA(pos,dacdata)	DacDigToAlog(pos,dacdata, DacNORMAL)


#define NOP()	asm volatile("nop"::)

#define M_DAC_H		125				// 멜로디 음량조절...
#define M_DAC_L		105
#define M_DAC_M		115

#define PLAYMELODY()	MelodyRegister |=  MelodyOnOffFLAG
#define ENDMELODY()		MelodyRegister &= ~MelodyOnOffFLAG

#define Speaker			7
#define InvertFLAG		0x01
#define SpFLAG			0x02
#define BREAKTIME		0x04
#define MelodyOnOffFLAG	0x08

#define EEPPLAYFLAG		0x10
#define EEPREPLAYNUMBERFLAG	0x20


#define Kong_32_		5
#define Kong_16_		10
#define Kong_8_			20
#define Kong_4_			41
#define Kong_p4_		62
#define Kong_2_			83
#define Kong_p2_		124

#define ENDCODE				0x80
#define END_number			30
#define REPLAYCODE			0x40
#define REPLAYNUMBERCODE	0x20


u08 m_nPos_Speaker;	//스피커 위치의 상태

volatile u08 MelodyRegister;
u08 Kong_ovf_count, KongCount, OvfCount, SubTimeValue;
u08 *pM_, *pK_, *pM_refrash, *pK_refrash, *Mel,*Kon;

volatile u08 MelVolCtrl = 10;

volatile u08 MelImpect = 0;


//Timer Overflow Count
const prog_char Melody_Tof_count[34] =
//솔  솔#  라  라# 시  도 도#  레  레#   미,파
{ 20, 19, 19, 17, 17, 16, 15, 14, 14, 13, 12,
//  파#, 솔 솔#  라  라# 시  도  도#  레
    11, 11, 10, 10,  9,  9,  9,  8,  8,
//  레#  미  파  파#  솔  솔# 라  라#  시  도 
    7,   7,  6,  6,  6,  6,  5,  5,  5,  5, 0x80, 0x40,0xc0,0x20,};

const prog_char Melody_Sub_Time[34] =

//솔  솔#  라  라# 시  도 도#  레  레#  미
{0, 48, 192, 14, 202, 46, 232, 76, 178, 202,
//파  파#  솔  솔#  라   라#  시  도   도#  레
  34, 0, 114, 8, 152, 32, 158, 230, 136, 216,
//레#  미  파   파#  솔  솔#   라  라#  시  도  
 90, 184, 0, 20, 104, 224,    0,  20, 104, 224, 0, 0, 0, 0};

const prog_char Melody_Kongnamul[10] = 
{   5, 		10,	 20,	 41,	 63,	 83,	 125, 		8,		 16,	 32,	};
// 32분   16분   8분    4분     점4분    2분      점2분	  *점32분    *점16분    *점8분
//==---------------------------------------------------------------------------------------------------

u08 m_bMelodyEnd;

//tone 음정
const prog_char T1SOL       [2] = {0,30};    //솔    0 
const prog_char T1SOLS		[2] = {1 ,30};   //솔#    1 
const prog_char T1RA        [2] = {2 ,30};   //라     2 
const prog_char T1RAS		[2] = {3 ,30};   //라#    3 
const prog_char T1SI        [2] = {4 ,30};   //시     4 
const prog_char T2DO        [2] = {5 ,30};   //도     5 
const prog_char T2DOS		[2] = {6 ,30};   //도#    6 
const prog_char T2RE        [2] = {7 ,30};   //레     7 
const prog_char T2RES		[2] = {8 ,30};   //레#    8 
const prog_char T2MI        [2] = {9 ,30};   //미     9 
const prog_char T2PA        [2] = {10,30};   //파     10
const prog_char T2PAS		[2] = {11,30};   //파#    11
const prog_char T2SOL       [2] = {12,30};   //솔     12
const prog_char T2SOLS		[2] = {13,30};   //솔#    13
const prog_char T2RA        [2] = {14,30};   //라     14
const prog_char T2RAS		[2] = {15,30};   //라#    15
const prog_char T2SI        [2] = {16,30};   //시     16
const prog_char T3DO        [2] = {17,30};   //도     17
const prog_char T3DOS		[2] = {18,30};   //도#    18
const prog_char T3RE        [2] = {19,30};   //레     19
const prog_char T3RES		[2] = {20,30};   //레#    20
const prog_char T3MI        [2] = {21,30};   //미     21
const prog_char T3PA        [2] = {22,30};   //파     22
const prog_char T3PAS		[2] = {23,30};   //파#    23
const prog_char T3SOL       [2] = {24,30};   //솔     24
const prog_char T3SOLS		[2] = {25,30};   //솔#    25
const prog_char T3RA        [2] = {26,30};   //라     26
const prog_char T3RAS		[2] = {27,30};   //라#    27
const prog_char T3SI        [2] = {28,30};   //시     28
const prog_char T4DO        [2] = {29,30};   //도     29

const prog_char CRES [2] = {32,30};    //쉼표                                

//Rhythm 리듬
const prog_char RHYTHM_32    [1] = { 0 };   //32분
const prog_char RHYTHM_16    [1] = { 1 };   //16분 
const prog_char RHYTHM_8_2   [1] = { 2 };   //8분
const prog_char RHYTHM_4_2   [1] = { 3 };   //4분
const prog_char RHYTHM_DOT_4 [1] = { 4 };   //점4분
const prog_char RHYTHM_2_2   [1] = { 5 };   //2분
const prog_char RHYTHM_DOT_2 [1] = { 6 };   //점2분
const prog_char RHYTHM_DOT_32[1] = { 7 };   //*점32분
const prog_char RHYTHM_DOT_16[1] = { 8 };   //*점16분
const prog_char RHYTHM_DOT_8 [1] = { 9 };   //*점8분


//Melody data ---------------------------------------------------------------------
//Melody1
const prog_char MELODY_1[14]	={9, 12, 9, 30, 14, 10, };
const prog_char RHYTHM_EX_1[13]	={2|0x10, 2|0x10, 2|0x10, 2|0x10, 2|0x10, };

//Melody2
const prog_char MELODY_2[15]	={9, 12, 9, 14, 30,10, 7, 0, 2, 4, 5, 7, 4, 5, 30};
const prog_char RHYTHM_EX_2[14]	={2, 2, 2, 2, 2, 2, 1,
								  1, 1, 1, 1, 1, 3};
//Melody3
const prog_char MELODY_3[4] 	={ 2, 3, 2, 30};			
const prog_char RHYTHM_EX_3[3] 	={ 1|0x10, 1|0x30, 1|0x10, };			

//Melody4
const prog_char MELODY_4[26]	={	 0,  2,  4,  5,  7,  8,  9,  4,  7,  2,   
									 5, 32,  4,  3,  4,	32, 10,  6,  9,  4, 
									 7, 32,  5,  4,  5, 30 };
const prog_char RHYTHM_EX_4[25]	={	 7,  7,  7,  7,  7,  7,  9,  7,  9,  7, 
									 9,  7,  8,  8,  8,  1,  9,  7,  9,  7,
									 9,  7,  8,  8,  8 };	//키 입력할 때 나는 소리
//Melody5
const prog_char MELODY_5[4] 	={ 2, 3, 2, 30};			
const prog_char RHYTHM_EX_5[3] 	={ 1|0x10, 1|0x30, 1|0x10, };				

//Melody6
const prog_char MELODY_6[5] 	={ 2, 7, 2, 32, 30};			
const prog_char RHYTHM_EX_6[4] 	={ 2, 2, 1, 4};	

//Melody7
const prog_char MELODY_7[4] ={	29, 24, 29, 30};
const prog_char RHYTHM_EX_7[3] ={	 8,  8,  8 };

//Melody8
const prog_char MELODY_8[5] ={ 11, 3, 11, 32, 30};
const prog_char RHYTHM_EX_8[4] ={ 2, 1, 2, 8};	

//Melody9
const prog_char MELODY_9[3]    ={7, 20, 30};				
const prog_char RHYTHM_EX_9[2]    ={1, 0};

//Melody10
const prog_char MELODY_10[5] ={	29, 24, 29, 32,30};
const prog_char RHYTHM_EX_10[4] ={	 8,  8,  8, 5 };
//-------- ---------------------------------------------------------------------

volatile u16 delay_count;
volatile u16 delayms_count;
volatile u16 delay32us;		 	// 07.03.29
								//2006-09-20 3:07오후
volatile u08 EepMelAddr;
volatile u08 Eep_Kon[33];
volatile u08 Eep_Mel[33];
///
// remocon value 유지 테스트
volatile u08 RemoconValuedelayCnt = 0;


//LCD로 디스플레이할 문자
//unsigned char degree[]= {0x02,0x05,0x02,0x00,0x00,0x00,0x00,0x00};  //도


//FND 테스트
////////////////////////////////////////////////////////////
char number[16]={0xde,0x18,0xcd,0x5d,0x1b,0x57,0xd7,0x1e,0xdf,0x5f,0x9f,0xd3,0xc1,0xd9,0xcf,0x87};

char number_fnd[16]={0x48,0xeb,0x8c,0x89,0x2b,0x19,0x18,0x4b,0x08,0x09,0x0a,0x38,0x5c,0xa8,0x1c,0x1e};

unsigned char empty[]="        ";	

 
void LEDModuleCtrl(u08 index,u08 position);

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
//
//	함수 선언부
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//	2007.03.13 추가 소스 (Function Define)
////////////////////////////////////////////////////////////

// Remocon 
void REPosition(u08 repos);

////////////////////////////////////////////////////////////

//LCD_Member_function
void lcd_port_init(void);
void lcd_init(void);
void command(unsigned char cmd);
void lcd_out(unsigned char data);
void lcd_position(unsigned char row, unsigned char col);
void lcd_char_out(unsigned char row, unsigned char col, unsigned char data);
void lcd_str_out(unsigned char row, unsigned char col, unsigned char *msg);
void lcd_line_clr(unsigned char line);
void lcd_shift(unsigned char RL);
void lcd_put_font(void);
void lcd_sensor_display(unsigned char sensor_value);

void LCD_os_init(void);

void PhotoSensorUpdate();

//Delay Function
void delay_cho(void);
void delay(unsigned int delay_count);
void delayms(unsigned int delay_ms);

void mSecDelay(unsigned int delay_ms);

void Beep(int position);

void Init_PWM_T1_for_DCmotor(void);


bool GetPhotoSensor(u08 sel,u08 nPhotoVal);

void Initialize();

u08 GetADC();

///멜로디 부분
void MelFunc();
void MelodyGnrate(u08 select_melody);
void EepMelGnrate();
void DacDigToAlog(u08 pos,u08 DacDigData, u08 DacOpcode);
void DacSetUp(void);

void SetSpeaker(u08 pos,u08 *pMel,u08 *pKong);
void Composition(u08 pos,u08 *pTone,u08 *pRhythm);

void FndDisplay(u08 pos,u08 val);

//리모콘 부분
void IrExCmdAct(u08 irExdata);
void IrRxTranslation(void);
void IrRecv(void);
void IrComm(void);
u08 GetRemocon();
bool GetRemoconVal(u08 repos,u08 nVal);


void ClkTickFunc(void);
bool Bulldozer(u08 state);

///////////////////////////////////////////////////////////

void isr_pwm(void);
void DcMotor_temp(u08 direction, u08 sel_motor, u08 pwm_value);//


////////////////////////////////////////////////////////////
/////- Function -///////////////////////////////////////////
////////////////////////////////////////////////////////////

SIGNAL(SIG_ADC)
{
    ADCData[0] = inp(ADCL);
    ADCData[1] = inp(ADCH);
    ADCComplete = 1;	//ADC 완료 플래그
}

/******************************************************************************
 타이머 인터럽트는 0.125us 마다 발생한다. 
******************************************************************************/
SIGNAL(SIG_OVERFLOW0)
{						//32uS 마다 진입
	
	static u08 mux2 = 0;
  	static u08 cntTick = 4;
  	static u08 cntMotor = 0;
  	static u08 Pwmloopcnt = 0;  //
 			 		 	 	
 	if(mux2){      //64uS 마다 진입
 	    mux2 = 0;
     	IrComm(); 
     	MelFunc();         	
		   
		if(delay_count != 0){	
			delay_count--;
		}
		if(delayms_count != 0){
			delayms_count--;
		}
			
 	}else{	
		mux2 = 1;					///// ControlPortc == 1 포토센서.
		if(ControlPortc == 1)  PhotoSensorUpdate();     ///// 2007.03.13 추가. (불도저 or 포토센서)

		if(Pwmloopcnt == 0){	// PWM 루틴	
		    Pwmloopcnt = 2;	    
			isr_pwm();
		}

		--Pwmloopcnt;		
		ClkTickFunc();		// 	CLK Genarate Zone 	   	
	}

	if(delay32us != 0) delay32us--;
	if(delay32us < 0 ) delay32us =0;
	
}

//------------------------------------------------------------------------------------

void ClkTickFunc(void)
{		// 진입주기 64uSec
	
	static u08 tickcnt  = 0;
	static CLKTickCnt = 0;	
  	static u08 sec6cnt = 0;
  	
 
  	tickcnt++;
   	
	if(tickcnt == 16)
	{									// 64 * 16 = 1024uSec
		tickcnt = 0;			
		CLKTickCnt++;
	    
	    
		fMicSample = 1;
        
		if(CLKTickCnt == 2)
		{					// 1024 *	2	=	2,048
		    fcdsSample = 1;
			CLKTickCnt = 0;
		}
		
	}
}

//---------------------------------------------------------
////////////////////////////////////////////////////////////
//	2007.03.13 추가 소스 (Function)
////////////////////////////////////////////////////////////

// Remocon

void REPosition(u08 repos)
{
	switch(repos)
	{
		case 1:		IR_RX_PIN = IR_TX_PIN = 0;
				break;
		case 2:		IR_RX_PIN = IR_TX_PIN = 1;
				break;
		case 3:		IR_RX_PIN = IR_TX_PIN = 2;
				break;
		case 4:		IR_RX_PIN = IR_TX_PIN = 3;
				break;
		case 5:		IR_RX_PIN = IR_TX_PIN = 4;
				break;
	}
}


///////////////////////////////////////////////////////
//멜로디 관련함수

void SetSpeaker(u08 pos,u08 *pMel,u08 *pKong)
{
    Mel = pMel;
    Kon = pKong;
	
	m_nPos_Speaker = pos;		
    
	pM_ = pM_refrash = Mel;			
	pK_ = pK_refrash =  Kon;		
	OvfCount 	= pgm_read_byte(&Melody_Tof_count[pgm_read_byte(pM_ )]);
	SubTimeValue= pgm_read_byte(&Melody_Sub_Time[pgm_read_byte(pM_)]);	
	
	MelImpect = pgm_read_byte(pK_);
	KongCount 	= pgm_read_byte(&Melody_Kongnamul[(0x0f&(pgm_read_byte(pK_)))]);
    
    
	PLAYMELODY();		
	MelodyRegister &= ~EEPPLAYFLAG;   

    //메인 루틴에서 계속해서 돌도록         
    while(MelodyRegister & MelodyOnOffFLAG);     
}

//---------------------------------------------------------
void MelFunc(void){						
	
	static u08  l_mel_high = 2;
	static u08  l_mel_low  = 2;
	//u08 MelImpect = 0;

	//MelImpect = MelImpect>>4;
	
	if(MelImpect&0xf0){
		u08 shi_impect;
		shi_impect = MelImpect>>4;
		l_mel_high = 115 + (shi_impect);
		l_mel_low  = 115 - (shi_impect);
	} else {
		l_mel_high = M_DAC_H;
		l_mel_low  = M_DAC_L;
	}
	
	l_mel_high += (MelVolCtrl );
	l_mel_low  -= (MelVolCtrl );

	if(MelodyRegister&MelodyOnOffFLAG){
	
	 	if(( -- Kong_ovf_count)==0)	KongCount -= 1;  
		
		if(MelodyRegister & EEPPLAYFLAG){
			
			if(MelodyRegister & InvertFLAG){ 
		
				if(!(MelodyRegister & BREAKTIME)){	
		      		if(MelodyRegister& SpFLAG){		// 스피커 포트 인버트		      			
						DAC_OUTDATA(m_nPos_Speaker,l_mel_low);
						MelodyRegister &= ~SpFLAG;
					} else {						
						DAC_OUTDATA(m_nPos_Speaker,l_mel_high);						
						MelodyRegister |=SpFLAG;
					}
				}
				
				if(KongCount==0){ 	//다음 멜로디 호출					
			     	MelodyRegister &= ~BREAKTIME;
			
					if(!(EepMelAddr>60)){
						pM_++;		pK_++;
						OvfCount 	= pgm_read_byte(&Melody_Tof_count[*pM_]);
						SubTimeValue= pgm_read_byte(&Melody_Sub_Time[ *pM_]);	
						MelImpect = *pK_;		
						KongCount 	= pgm_read_byte(&Melody_Kongnamul[0x0f&(*pK_)]);	
					} else {
						MelodyRegister &= ~EEPPLAYFLAG;
					}
		            
		            
		  		    if(OvfCount== ENDCODE){
		  		       
		  		    	MelodyRegister &= ~EEPPLAYFLAG;
		  				ENDMELODY();
						
	  				
						DAC_OUTDATA(m_nPos_Speaker,M_DAC_M);
	
    	                m_bMelodyEnd = 1;   //멜로디 끝났다.   
	  				
		 			}else if(OvfCount== REPLAYCODE){
						//EepMelGnrate();
						
					} else if(OvfCount== REPLAYNUMBERCODE){
						
						static u08 replycnt=0;
		
						if(MelodyRegister&EEPREPLAYNUMBERFLAG){
							replycnt = *pK_;
							replycnt &= 0x0f;
							MelodyRegister&= ~EEPREPLAYNUMBERFLAG;					
						}
						if(!(replycnt == 0)){
							replycnt--;
							//EepMelGnrate();
		
						} else {
							ENDMELODY();
							
	
							DAC_OUTDATA(m_nPos_Speaker,M_DAC_M);
							
						}
		  			} else if(OvfCount == 0xc0)	MelodyRegister |= BREAKTIME;
				} else {			// 이전 음(주파수) 생성
					OvfCount 	= pgm_read_byte(&Melody_Tof_count[*pM_]);
				}				
				MelodyRegister &= ~ InvertFLAG;	
			}			
			
		}
		else {		
			if(MelodyRegister & InvertFLAG){ 		
				if(!(MelodyRegister & BREAKTIME)){	
		      		if(MelodyRegister& SpFLAG){		// 스피커 포트 인버트
						DAC_OUTDATA(m_nPos_Speaker,l_mel_low);
						MelodyRegister &= ~SpFLAG;
					} else {
						DAC_OUTDATA(m_nPos_Speaker,l_mel_high);
						MelodyRegister |=SpFLAG;
					}
				}
				if(KongCount==0){ 	//다음 멜로디 호출
			     	pM_++;	pK_++;	MelodyRegister &= ~BREAKTIME;
		
					OvfCount 	= pgm_read_byte(&Melody_Tof_count[pgm_read_byte(pM_)]);
					SubTimeValue= pgm_read_byte(&Melody_Sub_Time[pgm_read_byte(pM_)]);
					MelImpect = pgm_read_byte(pK_);
					KongCount 	= pgm_read_byte(&Melody_Kongnamul[(0x0f &(pgm_read_byte(pK_)))]);
		
		  		    if(OvfCount== ENDCODE){
		  		    	MelodyRegister &= ~EEPPLAYFLAG;
		  				ENDMELODY();
						
						DAC_OUTDATA(m_nPos_Speaker,M_DAC_M);						
		 			} else if(OvfCount== REPLAYCODE){
						pM_ = pM_refrash;	pK_ = pK_refrash;
						OvfCount 	= pgm_read_byte(&Melody_Tof_count[pgm_read_byte(pM_)]);
						SubTimeValue= pgm_read_byte(&Melody_Sub_Time[pgm_read_byte(pM_)]);
						MelImpect = pgm_read_byte(pK_);
						KongCount 	= pgm_read_byte(&Melody_Kongnamul[(0x0f&(pgm_read_byte(pK_)))]); 
		  			} else if(OvfCount == 0xc0)	MelodyRegister |= BREAKTIME;
		  			
				} else {			// 이전 음(주파수) 생성
					OvfCount 	= pgm_read_byte(&Melody_Tof_count[pgm_read_byte(pM_)]);
				}				
				MelodyRegister &= ~ InvertFLAG;	
			}	    		
		}  
		
		    
		if((--OvfCount)==1){ 				//overflow 횟수가 맞으면 시간 조정
			MelodyRegister |= InvertFLAG;
			outp(SubTimeValue,TCNT0);
		}    
		
	}	
}



void DacDigToAlog(u08 pos, u08 DacDigData, u08 DacOpcode)
{
	u08 DigTemp;
	
	DigTemp = (u08)(DacOpcode | (DacDigData >>4));
	

	if(pos == 1){
		u08 i, j;
		
		//port setup
		sbi(DDRB,1);
		
		for(j = 0; j < 2; j++){
			for(i=0;i<8;i++){
				if((DigTemp<<i) & 0x80){
					sbi(PORTB,1);		//Data input
					NOP();
				}else{
					cbi(PORTB, 1);		//Data input
					NOP();
				}
			}
			DigTemp = (u08)(DacDigData << 4 );
		}	
	}else if(pos == 2){		//2번위치에 있을때
		u08 i2, j2;
				
		//port setup
		sbi(DDRD,1);
		
		for(j2 = 0; j2 < 2; j2++){
			for(i2=0;i2<8;i2++){
				if((DigTemp<<i2) & 0x80){
					sbi(PORTD,1);		//Data input				
					NOP();
				}else{
					cbi(PORTD, 1);		//Data input
					NOP();
				}				
			}
			DigTemp = (u08)(DacDigData << 4 );
		}		
	}else if(pos == 3){		//3번위치에 있을때
		u08 i3, j3;
		
		//port setup
		sbi(DDRD,5);
				
		for(j3 = 0; j3 < 2; j3++){
			for(i3=0;i3<8;i3++){
				if((DigTemp<<i3) & 0x80){
					sbi(PORTD,5);		//Data input				
					NOP();
				}else{
					cbi(PORTD, 5);		//Data input
					NOP();
				}				
			}
			DigTemp = (u08)(DacDigData << 4 );
		}		
	}	
	else if(pos == 4){		//2번위치에 있을때
		u08 i4, j4;
				
		//port setup
		sbi(DDRE,1);
		
		for(j4 = 0; j4 < 2; j4++){
			for(i4=0;i4<8;i4++){
				if((DigTemp<<i4) & 0x80){
					sbi(PORTE,1);		//Data input				
					NOP();
				}else{
					cbi(PORTE, 1);		//Data input
					NOP();
				}				
			}
			DigTemp = (u08)(DacDigData << 4 );
		}		
	}	
	else if(pos == 5){		//2번위치에 있을때
		u08 i5, j5;
				
		//port setup
		sbi(DDRG,3);
		
		for(j5 = 0; j5 < 2; j5++){
			for(i5=0;i5<8;i5++){
				if((DigTemp<<i5) & 0x80){
					sbi(PORTG,3);		//Data input				
					NOP();
				}else{
					cbi(PORTG, 3);		//Data input
					NOP();
				}				
			}
			DigTemp = (u08)(DacDigData << 4 );
		}		
	}	

}


//////////////////////////////////////////////////////
//리모콘 함수들

u08 GetRemocon()
{
	
	u08 temp[6],i,ird1,ird2,ir_1;
	u08* pData;
    
	ir_1 = ir_data[0];
	temp[0] = ~ir_data[1];	
	ird1 = ir_data[2];	//데이타값 리턴		
	ird2 = ir_data[3];	
	
	for(i=1;i<6; i++)
	{			
		temp[i]  = temp[0]>>i;		
		temp[i] |= temp[0]<<(8-i);	
	}
   	if(ir_1 == temp[1])
	{	            // 	C ^C<<1 D ^D
	
		temp[1] = ~ird2;			//
		if(ird1 == temp[1]){
       		pData = &ird1;
			return *pData;
		}
	} 	
	
	return 0;
}

//---------------------------------------------------------
void IrComm(void)
{
	if(IRReg&IrTimeFLAG){//timer0 ovf(주기 64 *2) = 128usec 생성
		IrRecv();
		IRReg &= ~IrTimeFLAG; 	
	} else {
		//IrTransmit();
		IRReg |= IrTimeFLAG;
	}
}

//---------------------------------------------------------
void IrRecv(void)
{

	static u08	c_count = 0, s_count = 0;

// DETECT SET	
	if((1<<IR_RX_PIN)&inp(IRRxPORT)){		

			//이전 상태가 low 면	// set_FLAG   <===SET	// clear_FLAG <=== CLEAR
		if(IRReg & clear_d_FLAG){	IRReg |= set_d_FLAG;	IRReg &= ~clear_d_FLAG;	

			if((2< c_count)&&(c_count <12 )){	T_count += 1;		//T
			
			} else if(c_count >29){									//4T +
				
				
				//NONFLASHSENSBF=1;	
				
				cbi(PORTC, SENS_TRIG);	

				u08 kk;
				for(kk =0;kk<4;kk++)ir_data[kk] =0;//ir_data
				OnlyIrCnt = 0;	T_count=0;
			}
			s_count = 0;
		}

		if(s_count >250);	else  s_count++;	// over flow 방지 

		c_count = 0;	
			
// DETECT LOW		
	} else {		
			//이전 상태가 high면	// clear_FLAG <===SET	// set_FLAG   <=== CLEAR
		if(IRReg & set_d_FLAG){	
			IRReg |= clear_d_FLAG;
			IRReg &= ~set_d_FLAG;

			if     ((2< s_count)&&(s_count <6 )){		// T 
														// 송신 완료...
				if((T_count ==32 )){
					T_data = ir_data[2];	IRReg |= IrRxdFLAG;	

				}
				
			} else if((5< s_count)&&(s_count <30)){		//3T
				u08 tmmp;

				if(T_count<=8){	
					tmmp = T_count-1;				ir_data[0] = ir_data[0] |(1<<tmmp);
					
					if(T_count == 8){					// 다자간 게임모드 데이터...
						MultiCommData = ir_data[0];
						IRReg |= MULTIRXDFLAG;
					}

				} else if((T_count>8)&&(T_count<=16)){
					tmmp = T_count-9;				ir_data[1] = ir_data[1] |(1<<tmmp); 
					
				} else if((T_count>16)&&(T_count<=24)){
					tmmp = T_count-17;				ir_data[2] = ir_data[2] |(1<<tmmp); 
					
				} else if((T_count>24)&&(T_count<=32)){
					tmmp = T_count-25;				ir_data[3] = ir_data[3] |(1<<tmmp); 
														// 송신완료
					if((T_count ==32 )){
						T_data = ir_data[2];	IRReg |= IrRxdFLAG;	
//						RComFlagReg &= ~ExclRxFLAG;
					}
				}


			} else if((29< s_count)/*&&(s_count<45)*/){	//4T
				T_count=0;							
			}
			c_count = 0;
		} 
		
		if(!(c_count > 250)) c_count++ ;
		s_count = 0;	
	}// else
	if(RemoconValuedelayCnt){
		if(RemoconValuedelayCnt == 1){
			IRReg&= ~IrRxdFLAG;
		}
		RemoconValuedelayCnt--;
	}
}


/////////////////////////////////////////////////////////////
//모터함수

void DcMotor_temp(u08 direction ,u08 sel_motor, u08 pwm_value){
	
// cur_pwm[2], org_pwm[2], pwm_cycle, pwm_cnt

		sbi(DDRB,4);
		sbi(DDRB,5);
		sbi(DDRE,4);
		sbi(DDRB,6);
		sbi(DDRB,7);
		sbi(DDRE,5);
	
	if(sel_motor == 1) 			// right_motor
	{
		if(direction == 1){		// 정방향		
			cbi(PORTB,4);
			sbi(PORTB,5);
		}else if(direction == 0){// 역방향
			cbi(PORTB,5);
			sbi(PORTB,4);
		}
		org_pwm[0] = pwm_value;
		if(org_pwm[0]){
			org_pwm[0] = pwm_value;
		} else {
			cbi(PORTB,4);
			cbi(PORTB,5);
		}
	}
	else if(sel_motor == 2)		// left_motor
	{	
		if(direction == 1){		// 정방향
			cbi(PORTB,7);
			sbi(PORTB,6);
		}else if(direction == 0){// 역방향
			cbi(PORTB,6);
			sbi(PORTB,7);
		}
		org_pwm[1] = pwm_value;
		if(org_pwm[1]){
			org_pwm[1] = pwm_value;		
		}else {
			cbi(PORTB,6);
			cbi(PORTB,7);
		}
	}	
	else if(sel_motor == 3)		// 양쪽모터
	{
		if(direction == 1){		// 정방향
			
			cbi(PORTB,4);
			sbi(PORTB,5);
		}else if(direction == 0){// 역방향
			cbi(PORTB,5);
			sbi(PORTB,4);
		}
		org_pwm[0] = pwm_value;
		if(org_pwm[0]){
			org_pwm[0] = pwm_value;
		} else {
			cbi(PORTB,4);
			cbi(PORTB,5);
		}
	
		if(direction == 1){		// 정방향
			cbi(PORTB,7);
			sbi(PORTB,6);
		}else if(direction == 0){// 역방향
			cbi(PORTB,6);
			sbi(PORTB,7);
		}
		org_pwm[1] = pwm_value;
		if(org_pwm[1]){
			org_pwm[1] = pwm_value;		
		}else {
			cbi(PORTB,6);
			cbi(PORTB,7);
		}
	}	
	
}

//---------------------------------------------------------
void isr_pwm(void)
{
	// cur_pwm[2], org_pwm[2], pwm_cycle, pwm_cnt

	pwm_cnt++;
		
	if(pwm_cnt == pwm_cycle){
		pwm_cnt = 0;	
		cur_pwm[0] = org_pwm[0];
		cur_pwm[1] = org_pwm[1];
		
		if(cur_pwm[0]){		// right 동작
			sbi(PORTE,4);
		}
		if(cur_pwm[1]){		// left 동작
			sbi(PORTE,5);
		}
	}
	
	if(pwm_cnt == cur_pwm[0]){		// right_정지
			cbi(PORTE,4);
	}	
	if(pwm_cnt == cur_pwm[1]){		// left_정지
			cbi(PORTE,5);
	}
	
}

//---------------------------------------------------------
void PhotoSensorUpdate()
{
    static u08 timingcnt;	//읽은 센서값을 카운터 시간 지연을 줌
    
	timingcnt++;
	
	if(timingcnt == 16){
	    timingcnt = 0;	
	
	}else if(timingcnt == 14){
		// 센서 발광 시작
		sbi(PORTC, SENS_TRIG);				
		
	}else if(timingcnt == 15){
		//센서수광, 값읽음	
		
		//
		if(SensorDir == FORWARD){
  		    ForwardPhotoSensor = inp(PINC)& 0x07;			//전방센서	
  		}else{
  		    DownwardPhotoSensor = //(inp(PINC)>>3)&0x07;
  		     (~((inp(PINC)&( 0x38))>> 3))&0x07;	//하방센서			
  		} 
  		  		  		
  		cbi(PORTC, SENS_TRIG);	//센서 발광 끝	  			  		
  		
	}
}

//Delay Function...... 
//////////////////////////////////////////////////////////////////////////////////////
//@@@@@@@@@@@@@@@ 07.03.28 @@@@@@@@@@@@@@@@@@@@//

void delayms(unsigned int delay_ms)
{
    for(;delay_ms>0;delay_ms--)
	{
        while(delayms_count > 0){}
		delayms_count = 16;
    }
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//


void delay_cho(void)
{
    while(delay_count > 0){
    }
	delay_count = 16;
}

//---------------------------------------------------------
void delay(unsigned int delay_cnt)
{
	
	delay32us = 1;
	
	for(;delay_cnt>0;delay_cnt--)
	{
		while(delay32us>0);
		delay32us = 1;
	}   


}

//---------------------------------------------------------
void mSecDelay(unsigned int delay_ms)  
{
	
    unsigned int delayms;
    
    delay_count = 16;
		
    delayms=delay_ms*100;
		
    for(;delayms>0;delayms--){
        delay_cho();		//delay(920);
    }
	
}

//LCD 관련함수
//////////////////////////////////////////////////////////////////////////////////////
void lcd_port_init(void)
{
    //Port Initialization
    outp(0xFF, LCD_C_INOUT);
    outp(0xFF, LCD_D_INOUT);
    outp(0x00, LCD_CONTROL);
    outp(0x00, LCD_DATA);
}

//---------------------------------------------------------
void lcd_init(void)		
{
    delay(625);       // Wait for more than 30ms
	
        command(LCD_SET_8); // Function Set
	delay(63);			// 2ms 지연
	command(LCD_SET_8);
	delay(63);
	command(LCD_D_ON);  // Display On/Off Control
        command(LCD_E_N);   // Entry Mode Set
	command(LCD_D_CLR); // Display Clear
        command(LCD_D_CLR);     // There must be!
}

//---------------------------------------------------------
void command(unsigned char cmd)
{
    delay(5);
    outp(0x04, PORTG);  
    delay(5);
    outp(cmd,LCD_DATA);
    delay(5);
    outp(0x00, PORTG);
    delay(5);
}

//---------------------------------------------------------
void lcd_out(unsigned char data)
{
    delay(5);
    outp(0x05, PORTG);
    delay(5);
    outp(data,LCD_DATA);
    delay(5);
    outp(0x01, PORTG);
    delay(5);
}

//---------------------------------------------------------
void lcd_position(unsigned char row, unsigned char col)
{
    char addr;
    addr=0x80+0x40*(row-1)+(col-1);
    command(addr);
}

//---------------------------------------------------------
void lcd_char_out(unsigned char row, unsigned char col, unsigned char data)
{
    lcd_position(row,col);
    lcd_out(data);
}

//---------------------------------------------------------
void lcd_str_out(unsigned char row, unsigned char col, unsigned char *msg)
{
    lcd_position(row,col);
    while(*msg){
        lcd_out(*(msg++));
    }
}

//---------------------------------------------------------
void lcd_line_clr(unsigned char line)
{
    unsigned char i;
    switch(line){
	case 1:
		for(i=0;i<LCD_SIZE;i++)
			lcd_char_out(1,i+1,' ');
		break;
	case 2:
		for(i=0;i<LCD_SIZE;i++)
			lcd_char_out(2,i+1,' ');
		break;
	case 3:
		for(i=0;i<LCD_SIZE;i++)
			lcd_char_out(1,i+1,' ');
		for(i=0;i<LCD_SIZE;i++)
			lcd_char_out(2,i+1,' ');
		break;
    }
}

//---------------------------------------------------------
void lcd_shift(unsigned char RL)
{
    if(RL==LCD_RIGHT)
        command(LCD_R_SHF);
    else
        command(LCD_L_SHF);
}

//---------------------------------------------------------
u08 GetCds(int pos)
{
    u08 tempCds;
   
	switch(pos){
		case 1:
			ADC_CHANNEL(AD_0);				// Cds value => ADC
			break;
		case 2:
			ADC_CHANNEL(AD_1);				// Cds value => ADC 
			break;
		case 3:
			ADC_CHANNEL(AD_2);				// Cds value => ADC
			break;
		case 4:
			ADC_CHANNEL(AD_3);				// Cds value => ADC
			break;
		
		case 5:
			ADC_CHANNEL(AD_4);				// Cds value => ADC
			break;
		
		default:
			break;	
	}
		
	tempCds = GetADC();				// AD start

	if(tempCds < 20){								//2007.06.27 수정
	    return 0;   //Cds Sensor Read Failed
	}else if(tempCds <= ILLUMINANCE_LV1){
	    return 1;   
	}else if(tempCds <= ILLUMINANCE_LV2){
	    return 2;   
	}else if(tempCds <= ILLUMINANCE_LV3){
	    return 3;
	}else if(tempCds <= ILLUMINANCE_LV4){
	    return 4;   
	}else{
		return 5;					//2007.06.27 수정
	}
}

//buzzer
//////////////////////////////////////////////////////////////////////////////////////////////
void Beep(int position)
{
	switch(position)
	{
	case 1:
		sbi(DDRB,0);
		sbi(DDRB,1);
		sbi(DDRB,2);
		sbi(DDRB,3);
				
		for(int i=0;i<300;i++){
			sbi(PORTB,1);	//buzzer ON
			sbi(PORTB,0);	//LED ON			
			delay(16);
				
			cbi(PORTB,1);	//buzzer OFF
			cbi(PORTB,0);	//LED OFF
			delay(16);
		}	

		break;
		
	case 2:
		sbi(DDRD,0);
		sbi(DDRD,1);
		sbi(DDRD,2);
		sbi(DDRD,3);
		
		for(int i=0;i<300;i++){
			sbi(PORTD,1);	//buzzer ON
			sbi(PORTD,0);	//LED ON
			delay(16);
			
			cbi(PORTD,1);	//buzzer OFF
			cbi(PORTD,0);	//LED OFF
			delay(16);
		}

		break;
		
	case 3:
		sbi(DDRD,4);
		sbi(DDRD,5);
		sbi(DDRD,6);
		sbi(DDRD,7);
		
		for(int i=0;i<300;i++){
			sbi(PORTD,5);	//buzzer ON
			sbi(PORTD,4);	//LED ON			
			delay(16);

			cbi(PORTD,5);	//buzzer OFF
			cbi(PORTD,4);	//LED OFF
			delay(16);
		}

		break;
		
	case 4:	
		sbi(DDRE,0);
		sbi(DDRE,1);
		sbi(DDRE,2);
		sbi(DDRE,3);

		for(int i=0;i<300;i++){
			sbi(PORTE,1);	//buzzer ON
			sbi(PORTE,0);	//LED ON
			delay(16);

			cbi(PORTE,1);	//buzzer OFF
			cbi(PORTE,0);	//LED OFF
			delay(16);
		}

		break;
		
	case 5:
		sbi(DDRG,3);
		cbi(PORTG,3);
		sbi(DDRE,7);
		cbi(PORTE,7);
		
		for(int i=0;i<300;i++){
			sbi(PORTG,3);	//buzzer ON
			sbi(PORTE,7);	//LED ON
			delay(16);
	
			cbi(PORTG,3);	//buzzer OFF
			cbi(PORTE,7);	//LED OFF
			delay(16);
		}

		break;
		
	default:
		break;
	
	}
	
}


void InitADC(void)
{
	
    outp(BV(ADIF)|BV(ADEN)|BV(ADIE)|BV(ADPS0),ADCSRA);     
    outp(0,ADMUX);      //default channel 선택	
    ADCComplete = 1;	
}

//---------------------------------------------------------
//Get Analog to Digital Converter
u08 GetADC()
{
	u08 temp;
    while(!ADCComplete); 	
    ADCComplete = 0;
    
    temp = inp(ADCSRA);		
    outp(temp|0x40,ADCSRA);	

    while(!ADCComplete); 
	
	temp = ((u08)(ADCData[0]>>2)) | ((u08)(ADCData[1]<<6));
    return temp;
}

//초기화 루틴
/////////////////////////////////////////////////////////////////////////////////////////////////
void Initialize()
{	
	
	fMicSample = 0;
	fcdsSample = 0;
	
	
	outp(0x40, DDRC);
	outp(0xff, PORTC);
	
	
	outp(0x00, TCNT0);
	outp(0x01, TCCR0);  		
	outp(BV(TOIE0), TIMSK);		
	
	sei();
	
	// LCD Initialize
    lcd_port_init();
    lcd_init();
    
/*	// 한글 폰트 로드
	 for (int i=0; i<8; i++){
	
		command(i | 0x40);	
		lcd_out(degree[i]);					
	}
*/	
	outp(DDRF_INPUT,DDRF);	
	InitADC();				
	
	//lcd Initialize	
	lcd_str_out(1,1,empty);
	lcd_str_out(2,1,empty);
	
    delay(100);
    
}


#endif//#ifndef  __NEWBEETLE_H__