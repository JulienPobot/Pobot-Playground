

//---------------------------------------------------------
bool GetCdsVal(u08 pos,u08 val)
{

  if(GetCds(pos) >= val){
    return TRUE;
  }else{
    return FALSE;
  }

}


//---------------------------------------------------------
bool GetRemoconVal(u08 repos,u08 nVal)
{

  u08 temp = 0;
  REPosition(repos);

  if(IRReg&IrRxdFLAG){
    temp = GetRemocon();
    RemoconValuedelayCnt = 3;
  }

  if(temp == nVal){
    return TRUE;
  }else{
    return FALSE;
  }

}


//---------------------------------------------------------
bool GetMicVal(u08 nLevel)
{

  static u08 tempMic;

  if(fMicSample){
    ADC_CHANNEL(MIC_IN);
    tempMic = GetADC();			// AD start
    fMicSample = 0;
  }

  if(tempMic >= nLevel)
    return TRUE;
  else
    return FALSE;

}

bool GetMicValLuca(u08 nLevel, u08 *ValMic)
{

  static u08 tempMic;

  if(fMicSample){
    ADC_CHANNEL(MIC_IN);
    tempMic = GetADC();			// AD start
    fMicSample = 0;
  }

  *ValMic = tempMic;

  if(tempMic >= nLevel)
    return TRUE;
  else
    return FALSE;

}


//---------------------------------------------------------
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


//---------------------------------------------------------
bool TactSWModuleCtrl(u08 pos,u08 val)
{

  static u08 swvalue = 0;
  // 	static u08 Nswvalue = 0;
  // 	u08 SwState = 0;

  outp(0x00,SFIOR);


  switch(pos)
	{
      case 1:
        cbi(DDRB , 0);	cbi(DDRB , 1);	cbi(DDRB , 2);
        sbi(PORTB, 0);	sbi(PORTB, 1);	sbi(PORTB, 2);
        delayms(1);

        if((0x01&inp(PINB)) == 0){
          swvalue = 0xfe;
        }else if((0x02&inp(PINB)) == 0){
          swvalue = 0xfd;
        }else if((0x04&inp(PINB)) == 0){
          swvalue = 0xfb;
        }else{
          swvalue = 0xff;
        }
        break;

      case 2:
        cbi(DDRD ,0 );	cbi(DDRD ,1 );	cbi(DDRD ,2 );
        sbi(PORTD,0 );	sbi(PORTD,1 );	sbi(PORTD,2 );
        delayms(1);

        if((0x01&inp(PIND)) == 0){
          swvalue = 0xfe;
        }else if((0x02&inp(PIND)) == 0){
          swvalue = 0xfd;
        }else if((0x04&inp(PIND)) == 0){
          swvalue = 0xfb;
        }else{
          swvalue = 0xff;
        }
        break;

      case 3:
        cbi(DDRD, 4);	cbi(DDRD ,5);	cbi(DDRD ,6);
        sbi(PORTD,4);	sbi(PORTD,5);	sbi(PORTD,6);
        delayms(1);

        if((0x10&inp(PIND)) == 0){
          swvalue = 0xfe;
        }else if((0x20&inp(PIND)) == 0){
          swvalue = 0xfd;
        }else if((0x40&inp(PIND)) == 0){
          swvalue = 0xfb;
        }else{
          swvalue = 0xff;
        }
        break;

      case 4:
        cbi(DDRE , 0);	cbi(DDRE , 1);	cbi(DDRE , 2);
        sbi(PORTE, 0);	sbi(PORTE, 1);	sbi(PORTE, 2);
        delayms(1);

        if((0x01&inp(PINE)) == 0){
          swvalue = 0xfe;
        }else if((0x02&inp(PINE)) == 0){
          swvalue = 0xfd;
        }else if((0x04&inp(PINE)) == 0){
          swvalue = 0xfb;
        }else{
          swvalue = 0xff;
        }
        break;

      case 5:
        cbi(DDRE ,7 );	cbi(DDRG ,3 );	cbi(DDRG ,4 );
        sbi(PORTE,7 );  sbi(PORTG,3 );	sbi(PORTG,4 );
        delayms(1);

        if((0x80&inp(PINE)) == 0){
          swvalue = 0xfe;
        }else if((0x08&inp(PING)) == 0){
          swvalue = 0xfd;
        }else if((0x10&inp(PING)) == 0){
          swvalue = 0xfb;
        }else{
          swvalue = 0xff;
        }
        break;

      default:
        return 0x00;

	}

  swvalue = (0x07& (~swvalue));

  if(swvalue == val){
    return TRUE;
  }else{
    return FALSE;
  }

}


//---------------------------------------------------------
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
        return FALSE;

	}

}


//---------------------------------------------------------
bool Bulldozer(u08 state)
{

  outp(0xfc, DDRC);
  outp(0xff, PORTC);

  if(inp(PINC)==state)
    return TRUE;
  else
    return FALSE;

}


//---------------------------------------------------------
void Buzzer(u08 pos,u08 num)
{

  int i;

  for(i=0;i<num;i++){
    Beep(pos);
    if(i==num-1) break;
    delayms(600);
  }

}


//---------------------------------------------------------
void LcdDisplay(char* msg1,char* msg2)
{

  unsigned char find1,find2,SaveVal1,SaveVal2,cnt;
  unsigned char cnt1 = 0, cnt2 = 0;

  SaveVal1 = text1;
  SaveVal2 = text2;

  for(cnt=0;cnt<9;cnt++){
    find1=msg1[cnt];
    text1 = cnt;
    if(find1 == 0) break;
  }

  for(cnt=0;cnt<9;cnt++){
    find2=msg2[cnt];
    text2 = cnt;
    if(find2 == 0) break;
  }

  for(cnt=0; cnt<text1; cnt++){
    if(string1[cnt] == msg1[cnt])	cnt1+=1;
    string1[cnt] = msg1[cnt];
  }

  for(cnt=0; cnt<text2; cnt++){
    if(string2[cnt] == msg2[cnt])	cnt2+=1;
    string2[cnt] = msg2[cnt];
  }

  if(cnt1==SaveVal1){
    lcd_str_out(1,1,msg1);
  }else{
    lcd_str_out(1,1,empty);
    lcd_str_out(1,1,msg1);
  }

  if(cnt2==SaveVal2){
    lcd_str_out(2,1,msg2);
  }else{
    lcd_str_out(2,1,empty);
    lcd_str_out(2,1,msg2);
  }

}


//---------------------------------------------------------
void DC_Control(u08 pos,u08 sel_motor,u08 pwm_value,u08 direction)
{
  //	m_nPwmPeriod = 0;

  if(pos == 0){
    DcMotor_temp(direction,sel_motor,pwm_value);
  }

  /*else if(pos == 1){
    Motor_1(direction,sel_motor,pwm_value);
	}else if(pos == 2){
    Motor_2(direction,sel_motor,pwm_value);
	}

	if(pos == 3){
    Motor_3(direction,sel_motor,pwm_value);
	}else if(pos == 4){
    Motor_4(direction,sel_motor,pwm_value);
	}else if(pos == 5){
    Motor_5(direction,sel_motor,pwm_value);
	}*/

}


//---------------------------------------------------------
void LEDModuleCtrl(u08 position,u08 index)
{

  // 	static u08 swvalue = 0;
  u08 LedState,i;

  switch(position){
    case 1:			//Module_1
      sbi(DDRB , 0);	sbi(DDRB , 1);	sbi(DDRB , 2);	sbi(DDRB , 3);	//LED port initialize

      for(i=0; i<4; i++)
        {
          LedState = index%2;
          index = index >> 1;
          if(LedState == 1) cbi(PORTB, i);
          else sbi(PORTB,i);
        }
      break;

    case 2:
      sbi(DDRD ,0 );	sbi(DDRD ,1 );	sbi(DDRD ,2 );	sbi(DDRD ,3 );

      for(i=0; i<4; i++)
        {
          LedState = index%2;
          index = index >> 1;
          if(LedState == 1) cbi(PORTD, i);
          else sbi(PORTD,i);
        }
      break;

    case 3:
      sbi(DDRD, 4);	sbi(DDRD ,5);	sbi(DDRD ,6);	sbi(DDRD ,7);

      for(i=4; i<8; i++)
        {
          LedState = index%2;
          index = index >> 1;
          if(LedState == 1) cbi(PORTD, i);
          else sbi(PORTD,i);
        }
      break;

    case 4:
      sbi(DDRE , 0);	sbi(DDRE , 1);	sbi(DDRE , 2);	sbi(DDRE , 3);

      for(i=0; i<4; i++)
        {
          LedState = index%2;
          index = index >> 1;
          if(LedState == 1) cbi(PORTE, i);
          else sbi(PORTE,i);
        }
      break;

    case 5:
      sbi(DDRE ,7 );	sbi(DDRG ,3 );	sbi(DDRG ,4 );	sbi(DDRF ,5 );

      for(i=0; i<4; i++)
        {
          LedState = index%2;
          index = index >> 1;
          switch(i)
            {
              case 0:		if(LedState == 1) cbi(PORTE, 7);
              else sbi(PORTE,7);
                break;
              case 1:		if(LedState == 1) cbi(PORTG, 3);
                else sbi(PORTG,3);
                break;
              case 2:		if(LedState == 1) cbi(PORTG, 4);
                else sbi(PORTG,4);
                break;
              case 3:		if(LedState == 1) cbi(PORTF, 5);
                else sbi(PORTF,5);
                break;
            }

        }
      break;

    default:
      break;

  }

}




//---------------------------------------------------------
void Composition(u08 pos,u08 *pTone,u08 *pRhythm)
{

  Mel = pTone;
  Kon = pRhythm;

  m_nPos_Speaker = pos;
  pM_ = pM_refrash = Mel;
  pK_ = pK_refrash =  Kon;

  OvfCount 	= PRG_RDB(&Melody_Tof_count[PRG_RDB(pM_ )]);
  SubTimeValue= PRG_RDB(&Melody_Sub_Time[PRG_RDB(pM_)]);
  MelImpect = PRG_RDB(pK_);
  KongCount 	= PRG_RDB(&Melody_Kongnamul[(0x0f&(PRG_RDB(pK_)))]);

  PLAYMELODY();
  MelodyRegister &= ~EEPPLAYFLAG;

  while(MelodyRegister & MelodyOnOffFLAG);

}


//---------------------------------------------------------
void fnd_display(u08 pos,u08 val)
{

  u08 i,data;
  data = number_fnd[val];

  switch(pos)
	{
      case 1:
		sbi(DDRB , HC595_DATA_1);	sbi(DDRB , HC595_CLK_1);	sbi(DDRB , HC595_LATCH_1);	//fnd port IO setup
		cbi(PORTB,HC595_LATCH_1);		//LATCH clear

		for(i=0;i<8;i++){
          if((data<<i) & 0x80){		//left shift
            sbi(PORTB,HC595_DATA_1);
            NOP();
          }else{
            cbi(PORTB,HC595_DATA_1);
            NOP();
          }

          sbi(PORTB,HC595_CLK_1);
          cbi(PORTB,HC595_CLK_1);

		}

		sbi(PORTB,HC595_LATCH_1);	 //LATCH set
		break;

      case 2:
		sbi(DDRD , HC595_DATA_2);	sbi(DDRD , HC595_CLK_2);	sbi(DDRD , HC595_LATCH_2);	//fnd port IO setup
		cbi(PORTD,HC595_LATCH_2);		//LATCH clear

		for(i=0;i<8;i++){
          if((data<<i) & 0x80){		//left shift
            sbi(PORTD,HC595_DATA_2);
            NOP();
          }else{
            cbi(PORTD,HC595_DATA_2);
            NOP();
          }

          sbi(PORTD,HC595_CLK_2);
          cbi(PORTD,HC595_CLK_2);

		}

		sbi(PORTD,HC595_LATCH_2);	 //LATCH set
		break;

      case 3:
		sbi(DDRD , HC595_DATA_3);	sbi(DDRD , HC595_CLK_3);	sbi(DDRD , HC595_LATCH_3);	//fnd port IO setup
		cbi(PORTD,HC595_LATCH_3);		//LATCH clear

		for(i=0;i<8;i++){
          if((data<<i) & 0x80){		//left shift
            sbi(PORTD,HC595_DATA_3);
            NOP();
          }else{
            cbi(PORTD,HC595_DATA_3);
            NOP();
          }

          sbi(PORTD,HC595_CLK_3);
          cbi(PORTD,HC595_CLK_3);

		}

		sbi(PORTD,HC595_LATCH_3);	 //LATCH set
		break;

      case 4:
		sbi(DDRE , HC595_DATA_4);	sbi(DDRE , HC595_CLK_4);	sbi(DDRE , HC595_LATCH_4);	//fnd port IO setup
		cbi(PORTE,HC595_LATCH_4);		//LATCH clear

		for(i=0;i<8;i++){
          if((data<<i) & 0x80){		//left shift
            sbi(PORTE,HC595_DATA_4);
            NOP();
          }else{
            cbi(PORTE,HC595_DATA_4);
            NOP();
          }

          sbi(PORTE,HC595_CLK_4);
          cbi(PORTE,HC595_CLK_4);

		}

		sbi(PORTE,HC595_LATCH_4);	 //LATCH set
		break;

      case 5:
		sbi(DDRE , HC595_DATA_5);	sbi(DDRG , HC595_CLK_5);	sbi(DDRG , HC595_LATCH_5);	//fnd port IO setup
		cbi(PORTG,HC595_LATCH_5);		//LATCH clear

		for(i=0;i<8;i++){
          if((data<<i) & 0x80){		//left shift
            sbi(PORTE,HC595_DATA_5);
            NOP();
          }else{
            cbi(PORTE,HC595_DATA_5);
            NOP();
          }

          sbi(PORTG,HC595_CLK_5);
          cbi(PORTG,HC595_CLK_5);

		}

		sbi(PORTG,HC595_LATCH_5);	 //LATCH set
		break;

      default:
		break;

	}

}

