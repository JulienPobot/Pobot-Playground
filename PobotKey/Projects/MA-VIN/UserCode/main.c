#include "newbeetle.h"

void LcdDisplay(char* msg1,char* msg2)
{
	unsigned char find1,find2,SaveVal1,SaveVal2,cnt;
	unsigned char cnt1 = 0, cnt2 = 0; 
		
		
		SaveVal1 = text1;
		SaveVal2 = text2;
	
	
		for(cnt=0;cnt<9;cnt++)                      
		{
			find1=msg1[cnt];
			text1 = cnt;
			if(find1 == 0) break;   
		}

		for(cnt=0;cnt<9;cnt++)                      
		{
			
			find2=msg2[cnt];
			text2 = cnt;
			if(find2 == 0) break;
		} 
			                             
		for(cnt=0; cnt<text1; cnt++)   
		{
			if(string1[cnt] == msg1[cnt])	cnt1+=1;  
			string1[cnt] = msg1[cnt];
		}  
		
		for(cnt=0; cnt<text2; cnt++)  
		{
			if(string2[cnt] == msg2[cnt])	cnt2+=1;   
			string2[cnt] = msg2[cnt];
		}
	
	if(cnt1==SaveVal1) 
	{
		lcd_str_out(1,1,msg1);
	}
	else 
	{
		lcd_str_out(1,1,empty);
		lcd_str_out(1,1,msg1);
	}
	
	if(cnt2==SaveVal2) 
	{
		lcd_str_out(2,1,msg2);
		
	}
	else 
	{
		lcd_str_out(2,1,empty);
		lcd_str_out(2,1,msg2);
	}    
    
	
	
}


bool GetMicVal(u08 nLevel)
{

 u08 tempMic;
    
    if(fMicSample){
       
        ADC_CHANNEL(MIC_IN);				// Mic를 ad 컨버터로 연결 
        tempMic = GetADC();				// AD start	
        fMicSample = 0;	 	          
    }
	
	if(tempMic >= nLevel)    //
		return TRUE;
    else
		return FALSE;
       
}



void Buzzer(u08 pos,u08 num)
{
	int i,k;

	for(i=0;i<num;i++){
		Beep(pos);	
		if(i==num-1) break;
		delayms(600);
	}		
}	


void DC_Control(u08 pos,u08 sel_motor,u08 pwm_value,u08 direction)
{
	m_nPwmPeriod = 0;

	if(pos == 0){
		DcMotor_temp(direction,sel_motor,pwm_value);
	}
	else if(pos == 1){
		Motor_1(direction,sel_motor,pwm_value);
	}

	else if(pos == 2){
		Motor_2(direction,sel_motor,pwm_value);
	}

	if(pos == 3){
		Motor_3(direction,sel_motor,pwm_value);
	}

	else if(pos == 4){
		Motor_4(direction,sel_motor,pwm_value);
	
	}
	else if(pos == 5){
		Motor_5(direction,sel_motor,pwm_value);
	}
}


void main(void)
{
	Initialize();
	int flow_Num = 0;
	int nRepeat[32];
	int Register = 0;
	memset(nRepeat,0,sizeof(nRepeat));
	while(1)
	{
		if(flow_Num == 0)
			flow_Num = 1;
		if(flow_Num == 1)
		{
			LcdDisplay("START","WAIT...");
			flow_Num = 2;
		}
		if(flow_Num == 2)
		{
			LcdDisplay("!!!","");
			flow_Num = 3;
		}
		if(flow_Num == 3)
		{
			if(GetMicVal(MIC_LEVEL_3) == FALSE )
				flow_Num = 5;
			else
				flow_Num = 6;
		}
		if(flow_Num == 4)
		{
			flow_Num = 3;
		}
		if(flow_Num == 5)
		{
			LcdDisplay("555","");
			flow_Num = 4;
		}
		if(flow_Num == 6)
		{
			Buzzer(2,50);
			flow_Num = 7;
		}
		if(flow_Num == 7)
		{
			DC_Control(DEFAULT,MOTOR_BOTH,10,CW);
			flow_Num = 8;
		}
		if(flow_Num == 8)
			break;
	}
}

