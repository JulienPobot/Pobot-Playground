#define CHANINTERLEAVE 0 // 660 for multiplex, 0 for others#define MAXCHANOUT 6 // max output channels

#define DEADBAND 8 // microseconds

//***********************************************************************************************************************

void setupRx() {
  pinMode(2, INPUT); // 3 is used for esc
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);



  // interrupt on pin change PCINT
  PCICR |= (1 << PCIE2);

  PCMSK2 = (1 << PCINT18) | // chan1
  (1 << PCINT20) | // chan3
  (1 << PCINT21) | // chan5
  (1 << PCINT22) ; // chan7

 
}

//***********************************************************************************************************************

ISR(PCINT2_vect)
{
  //static byte currentChannel;
  static byte state;
  static unsigned long tstart;

  byte chan1=PIND & (1<<2);
  byte chan3=PIND & (1<<4);
  byte chan5=PIND & (1<<5);
  byte chan7=PIND & (1<<6);
  unsigned long time=micros();

  if (chan1) 
  {
    tstart=time;
    state=10;
  } 
  else if (state==10) // going down chan1
  {
    channelIn[0]=time-tstart-CENTER;
    channelIn[0]=limit(-500,channelIn[0],500);
    tstart=time;
    state=20;
  } 
  else if (chan3) 
  {
    channelIn[1]=CHANINTERLEAVE+time-tstart-CENTER;
    channelIn[1]=limit(-500,channelIn[1],500);
    tstart=time;
    state=30;
  }
  else if (state==30) // going down chan3
  {
    channelIn[2]=time-tstart-CENTER;
    channelIn[2]=limit(-500,channelIn[2],500);
    tstart=time;
    state=40;
  }
  else if (chan5) 
  {
    channelIn[3]=CHANINTERLEAVE+time-tstart-CENTER;
    channelIn[3]=limit(-500,channelIn[3],500);
    tstart=time;
    state=50;
  }
  else if (state==50) // going down chan5
  {
    channelIn[4]=time-tstart-CENTER;
    channelIn[4]=limit(-500,channelIn[4],500);
    tstart=time;
    state=60;
  }
  else if (chan7) 
  {
    channelIn[5]=CHANINTERLEAVE+time-tstart-CENTER;
    channelIn[5]=limit(-500,channelIn[5],500);
    tstart=time;
    state=70;
  }
  else if (state==70) // going down chan7
  {
    channelIn[6]=time-tstart-CENTER;
    channelIn[6]=limit(-500,channelIn[6],500);
    tstart=time;
    state=80;
  }
}



