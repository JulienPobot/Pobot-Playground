#include "newbeetle.h"

void fnd_display(u08 pos,u08 val)
{

	u08 i,data;
	
	data = number_fnd[val];
	
	switch(pos)
	{
	case 1:
		sbi(DDRB , HC595_DATA_1);	sbi(DDRB , HC595_CLK_1);	sbi(DDRB , HC595_LATCH_1);	//fnd port IO setup
		
		cbi(PORTB,HC595_LATCH_1);		//래치를 0으로 설정
		
		//데이타 전송
		for(i=0;i<8;i++){
			if((data<<i) & 0x80){		//왼쪽으로 값을 시프트
				sbi(PORTB,HC595_DATA_1);
				NOP();
			}else{
				cbi(PORTB,HC595_DATA_1);
				NOP();
			}
					
			//클럭 생성
			sbi(PORTB,HC595_CLK_1);	
			cbi(PORTB,HC595_CLK_1);
		
		}
		
		sbi(PORTB,HC595_LATCH_1);	 //래치를 1로 설정
		
		break;
		
	case 2:
		sbi(DDRD , HC595_DATA_2);	sbi(DDRD , HC595_CLK_2);	sbi(DDRD , HC595_LATCH_2);	//fnd port IO setup
		
		cbi(PORTD,HC595_LATCH_2);		//래치를 0으로 설정
		
		//데이타 전송
		for(i=0;i<8;i++){
			if((data<<i) & 0x80){		//왼쪽으로 값을 시프트
				sbi(PORTD,HC595_DATA_2);
				NOP();
			}else{
				cbi(PORTD,HC595_DATA_2);
				NOP();
			}
					
			//클럭 생성
			sbi(PORTD,HC595_CLK_2);	
			cbi(PORTD,HC595_CLK_2);
		
		}
		
		sbi(PORTD,HC595_LATCH_2);	 //래치를 1로 설정
		break;
	case 3:
		sbi(DDRD , HC595_DATA_3);	sbi(DDRD , HC595_CLK_3);	sbi(DDRD , HC595_LATCH_3);	//fnd port IO setup
		
		cbi(PORTD,HC595_LATCH_3);		//래치를 0으로 설정
		
		//데이타 전송
		for(i=0;i<8;i++){
			if((data<<i) & 0x80){		//왼쪽으로 값을 시프트
				sbi(PORTD,HC595_DATA_3);
				NOP();
			}else{
				cbi(PORTD,HC595_DATA_3);
				NOP();
			}
					
			//클럭 생성
			sbi(PORTD,HC595_CLK_3);	
			cbi(PORTD,HC595_CLK_3);
		
		}
		
		sbi(PORTD,HC595_LATCH_3);	 //래치를 1로 설정
		break;
	case 4:
		sbi(DDRE , HC595_DATA_4);	sbi(DDRE , HC595_CLK_4);	sbi(DDRE , HC595_LATCH_4);	//fnd port IO setup
		
		cbi(PORTE,HC595_LATCH_4);		//래치를 0으로 설정
		
		//데이타 전송
		for(i=0;i<8;i++){
			if((data<<i) & 0x80){		//왼쪽으로 값을 시프트
				sbi(PORTE,HC595_DATA_4);
				NOP();
			}else{
				cbi(PORTE,HC595_DATA_4);
				NOP();
			}
					
			//클럭 생성
			sbi(PORTE,HC595_CLK_4);	
			cbi(PORTE,HC595_CLK_4);
		
		}
		
		sbi(PORTE,HC595_LATCH_4);	 //래치를 1로 설정
		break;
	case 5:
		sbi(DDRE , HC595_DATA_5);	sbi(DDRG , HC595_CLK_5);	sbi(DDRG , HC595_LATCH_5);	//fnd port IO setup
		
		cbi(PORTG,HC595_LATCH_5);		//래치를 0으로 설정
		
		//데이타 전송
		for(i=0;i<8;i++){
			if((data<<i) & 0x80){		//왼쪽으로 값을 시프트
				sbi(PORTE,HC595_DATA_5);
				NOP();
			}else{
				cbi(PORTE,HC595_DATA_5);
				NOP();
			}
					
			//클럭 생성
			sbi(PORTG,HC595_CLK_5);	
			cbi(PORTG,HC595_CLK_5);
		
		}
		
		sbi(PORTG,HC595_LATCH_5);	 //래치를 1로 설정
		break;
	
	default:
		break;
			
	}


}


bool TouchSensor(u08 pos)
{
	u08 temp = 0;	
	
	switch(pos)
	{
		case 1:
			cbi(DDRB,0);	cbi(PORTB,0);		
			sbi(DDRB,3);	sbi(PORTB,3);
		
			temp = (0x01 & inp(PINB));
			
			if(temp == 0x01)
			{
				cbi(PORTB,3);						
				delayms(1);	
				return TRUE;
			}
		
			sbi(PORTB,3);
			return FALSE;
			
			break;
		case 2:
			cbi(DDRD,0);	cbi(PORTD,0);		
			sbi(DDRD,3);	sbi(PORTD,3);
		
			temp = (0x01 & inp(PIND));
			
			if(temp == 0x01)
			{
				cbi(PORTD,3);						
				delayms(1);	
				return TRUE;
			}
		
			sbi(PORTD,3);
			return FALSE;
			break;
		case 3:
			cbi(DDRD,4);	cbi(PORTD,4);		
			sbi(DDRD,7);	sbi(PORTD,7);
		
			temp = (0x10 & inp(PIND));
			
			if(temp == 0x10)
			{
				cbi(PORTD,7);						
				delayms(1);	
				return TRUE;
			}
		
			sbi(PORTD,7);
			return FALSE;
			break;
		case 4:
			cbi(DDRE,0);	cbi(PORTE,0);		
			sbi(DDRE,3);	sbi(PORTE,3);
		
			temp = (0x01 & inp(PINE));
			
			if(temp == 0x01)
			{
				cbi(PORTE,3);						
				delayms(1);	
				return TRUE;
			}
		
			sbi(PORTE,3);
			return FALSE;
			
			break;
		case 5:
			cbi(DDRE,7);	cbi(PORTE,7);		
			sbi(DDRF,5);	sbi(PORTF,5);
		
			temp = (0x80 & inp(PINE));
			
			if(temp == 0x80)
			{
				cbi(PORTF,5);						
				delayms(1);	
				return TRUE;
			}
		
			sbi(PORTF,5);
			return FALSE;
			break;
			
		default:
			break;	
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


bool GetPhotoSensor(u08 sel,u08 nPhotoVal)
{
	ControlPortc = 1;
	SensorDir = sel;
	
	if(sel == FORWARD){
	
		if(ForwardPhotoSensor == nPhotoVal){
			return TRUE;
		}else{
			return FALSE;		
		}		
	}else{
	
		if(DownwardPhotoSensor == nPhotoVal){
			return TRUE;
		}else{
			return FALSE;		
		}		
	
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
			fnd_display(1,1);
			flow_Num = 2;
		}
		if(flow_Num == 2)
		{
			if(TouchSensor(3) == TRUE )
				flow_Num = 21;
			else
				flow_Num = 7;
		}
		if(flow_Num == 3)
		{
			flow_Num = 2;
		}
		if(flow_Num == 4)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,0,CW);
			flow_Num = 3;
		}
		if(flow_Num == 5)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,8,CW);
			flow_Num = 4;
		}
		if(flow_Num == 6)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_LEFT) == TRUE )
				flow_Num = 5;
			else
				flow_Num = 9;
		}
		if(flow_Num == 7)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_CENTER) == TRUE )
				flow_Num = 8;
			else
				flow_Num = 6;
		}
		if(flow_Num == 8)
		{
			DC_Control(DEFAULT,MOTOR_BOTH,8,CW);
			flow_Num = 3;
		}
		if(flow_Num == 9)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_CENTER_LEFT) == TRUE )
				flow_Num = 19;
			else
				flow_Num = 10;
		}
		if(flow_Num == 10)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_RIGHT) == TRUE )
				flow_Num = 17;
			else
				flow_Num = 11;
		}
		if(flow_Num == 11)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_CENTER_RIGHT) == TRUE )
				flow_Num = 15;
			else
				flow_Num = 12;
		}
		if(flow_Num == 12)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_ALL_OFF) == TRUE )
				flow_Num = 3;
			else
				flow_Num = 13;
		}
		if(flow_Num == 13)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,8,CW);
			flow_Num = 14;
		}
		if(flow_Num == 14)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,8,CW);
			flow_Num = 3;
		}
		if(flow_Num == 15)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,8,CW);
			flow_Num = 16;
		}
		if(flow_Num == 16)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,6,CW);
			flow_Num = 3;
		}
		if(flow_Num == 17)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,8,CW);
			flow_Num = 18;
		}
		if(flow_Num == 18)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,0,CW);
			flow_Num = 3;
		}
		if(flow_Num == 19)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,8,CW);
			flow_Num = 20;
		}
		if(flow_Num == 20)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,6,CW);
			flow_Num = 3;
		}
		if(flow_Num == 21)
		{
			if(TouchSensor(3) == TRUE )
				flow_Num = 22;
			else
				flow_Num = 2;
		}
		if(flow_Num == 22)
		{
			fnd_display(1,2);
			flow_Num = 23;
		}
		if(flow_Num == 23)
		{
			if(GetPhotoSensor(DOWNWARD,PHOTO_ALL_OFF) == TRUE )
				flow_Num = 29;
			else
				flow_Num = 46;
		}
		if(flow_Num == 24)
		{
			flow_Num = 23;
		}
		if(flow_Num == 25)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,5,CCW);
			flow_Num = 24;
		}
		if(flow_Num == 26)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,8,CW);
			flow_Num = 25;
		}
		if(flow_Num == 27)
		{
			if(GetPhotoSensor(FORWARD,PHOTO_LEFT) == TRUE )
				flow_Num = 26;
			else
				flow_Num = 31;
		}
		if(flow_Num == 28)
		{
			if(GetPhotoSensor(FORWARD,PHOTO_LEFT_CENTER_RIGHT) == TRUE )
				flow_Num = 30;
			else
				flow_Num = 27;
		}
		if(flow_Num == 29)
		{
			if(GetPhotoSensor(FORWARD,PHOTO_CENTER) == TRUE )
				flow_Num = 30;
			else
				flow_Num = 28;
		}
		if(flow_Num == 30)
		{
			DC_Control(DEFAULT,MOTOR_BOTH,8,CW);
			flow_Num = 24;
		}
		if(flow_Num == 31)
		{
			if(GetPhotoSensor(FORWARD,PHOTO_CENTER_LEFT) == TRUE )
				flow_Num = 40;
			else
				flow_Num = 32;
		}
		if(flow_Num == 32)
		{
			if(GetPhotoSensor(FORWARD,PHOTO_RIGHT) == TRUE )
				flow_Num = 38;
			else
				flow_Num = 33;
		}
		if(flow_Num == 33)
		{
			if(GetPhotoSensor(FORWARD,PHOTO_CENTER_RIGHT) == TRUE )
				flow_Num = 36;
			else
				flow_Num = 34;
		}
		if(flow_Num == 34)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,5,CW);
			flow_Num = 35;
		}
		if(flow_Num == 35)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,5,CW);
			flow_Num = 24;
		}
		if(flow_Num == 36)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,8,CW);
			flow_Num = 37;
		}
		if(flow_Num == 37)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,5,CCW);
			flow_Num = 24;
		}
		if(flow_Num == 38)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,8,CW);
			flow_Num = 39;
		}
		if(flow_Num == 39)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,5,CCW);
			flow_Num = 24;
		}
		if(flow_Num == 40)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,8,CW);
			flow_Num = 41;
		}
		if(flow_Num == 41)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,5,CCW);
			flow_Num = 24;
		}
		if(flow_Num == 42)
		{
			mSecDelay(10);
			flow_Num = 24;
		}
		if(flow_Num == 43)
		{
			DC_Control(DEFAULT,MOTOR_LEFT,6,CCW);
			flow_Num = 42;
		}
		if(flow_Num == 44)
		{
			DC_Control(DEFAULT,MOTOR_RIGHT,6,CW);
			flow_Num = 43;
		}
		if(flow_Num == 45)
		{
			mSecDelay(10);
			flow_Num = 44;
		}
		if(flow_Num == 46)
		{
			DC_Control(DEFAULT,MOTOR_BOTH,5,CCW);
			flow_Num = 45;
		}
		if(flow_Num == 47)
		{
			if(TouchSensor(3) == TRUE )
				flow_Num = 48;
			else
				flow_Num = 23;
		}
		if(flow_Num == 48)
			break;
	}
}

