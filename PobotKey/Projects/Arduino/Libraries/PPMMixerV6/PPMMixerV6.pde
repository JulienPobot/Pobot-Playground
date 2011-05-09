/*
 PPMMixer V5
 Copytight (C) 2010
 Francesco Ferrara
 ferrara{a}libero.it
 
 connect signal channel from a receiver on pin D2 D4 D5 D6 to chan1 chan3 chan5 chan7 of your receiver
 chan2,chan4,chan6 are derived from difference with the other channels
 apply gnd common with arduino and servo
 apply 5v from a battery to servo and receiver
 
 connect signal channel out to 4 servos and 2 esc or 4 esc and 2 servo on pin D9 D10 - D3 D11 - D8 D12 look at globals.h
 look at RX for CHANINTERLEAVE it is the time offset if you have e multiplex receiver, set 0 otherwise
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>          // definitions for power-down modes
#include <math.h>

#include "globals.h"

void setup()
{
  Serial.begin(115200);
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable(); 

  setupRx();
  setupEsc(495); // in hz
  setupServo();

  for(int i=0;i<MAXCHANOUT;i++)
    channelOut[i]=-500;
  digitalWrite(13,HIGH);

}

void mixing() 
{
  float ele,ail,rud,thr;

  // servo anticlock = +
  // channelIn +sx 
  // channelIn +up 

  // g.p +up
  // p.r +sx  
  // g.y +dx

  ele=channelIn[ELE];
  ail=channelIn[AIL];
  rud=-channelIn[RUD];
  thr=channelIn[THR];

  channelOut[FRONTL]=thr-ele/2-ail;
  channelOut[FRONTR]=thr-ele/2+ail;
  channelOut[REAR]  =thr+ele;
  channelOut[YAW]   =rud; // servo +dx
}



unsigned long prevTime=micros();
unsigned long cycleTime=DT;
long count=10;  
boolean led;


void loop()
{ 

  unsigned long curTime=micros();
  unsigned long cycleTime=curTime-prevTime;
  if (cycleTime>=DT)  // 500hz = 2000us
  {
    prevTime=curTime;

    mixing();
    if (count--<=0)
    {
      count=250; // print some debug info every some cycle

      digitalWrite(13,led);
      led=!led;
    }

    updateEsc();
    updateServo();

  }

  sleep_cpu(); // wait for some interrupt
} 




















