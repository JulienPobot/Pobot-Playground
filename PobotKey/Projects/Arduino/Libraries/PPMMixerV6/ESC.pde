#define unitus 2 //cycle 

void setupEsc(int freq)
{
  // Timer 1
  TCCR1A = ((1<<WGM11) | (1<<COM1B1) | (1<<COM1A1)); //Fast PWM: ICR1=TOP, OCR1x=BOTTOM,TOV1=TOP
  TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11); // Clock scaler = 8, 2,000,000 counts per second
  OCR1A = 1500*unitus;	
  OCR1B = 1500*unitus; 	
  ICR1 = 16000000L/8/limit(50,freq,500);
  
  TIMSK1 |= _BV(ICIE1);	// Timer/Counter1, Input Capture Interrupt Enable //PB0 - output throttle
  
  pinMode(9, OUTPUT); // OSC1A
  pinMode(10, OUTPUT); // OSC1B
  
  pinMode(3,OUTPUT); // OSC2A
  pinMode(11,OUTPUT); // OSC2A
}

//***********************************************************************************************************************
ISR(TIMER1_CAPT_vect) // Timer/Counter1 Capture Event
{
  unsigned int period;
  
  period=(CENTER+channelOut[0]);
  period=limit(1000,period,2000);
  OCR1A = period*unitus;
  
  period=(CENTER+channelOut[1]);
  period=limit(1000,period,2000);
  OCR1B = period*unitus;
}

void updateEsc()
{
  unsigned int period;
  
  period=CENTER+channelOut[2];
  period=limit(1000,period,2000);
  analogWrite(3,(period>>3)+2); // TODO write better
  
  period=CENTER+channelOut[3];
  period=limit(1000,period,2000);
  analogWrite(11,(period>>3)+2); // TODO write better
}
