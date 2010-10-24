#ifndef __TIMERS_2_H__
#define __TIMERS_2_H__

#include "timers_2_conf.h"

#if NB_TIMER <8
extern unsigned char timer_running;
#elif NB_TIMER < 16
extern unsigned int timer_running;
#endif
extern volatile int8_t timer_ticks_to_increment; // Incremented on each interrupt
extern uint16_t g_free_running_counter;

/** 
 Initialyse the timers
*/
// static inline void timer_init(void)
// {
  
//   TCCR2A = _BV(WGM21); // Mode 2, Clear on match OCRA

//   // Set prescaler
//   if      (TM2_PRESCAL == 1024) TCCR2B = 5;
//   else if (TM2_PRESCAL ==  256) TCCR2B = 4;
//   else if (TM2_PRESCAL ==   64) TCCR2B = 3;
//   else if (TM2_PRESCAL ==    8) TCCR2B = 2;
//   else if (TM2_PRESCAL ==    1) TCCR2B = 1;
//   else                          TCCR2B = 0;

//   timer_running = 0; // No timer is running at init.
  
//   // Set the value to count to
//   OCR2A = TM2_TO_COUNT;
  
//   // Enable interrupt when match.
//   TIMSK2 = _BV(OCIE2A);
// }

/**
 Should be called periodicaly from the main loop.
 It takes care of decrementing the timers when the interrupts have ticked the flag
 */
void timer_handle(void);

/**
 Start a timer given it's ID and the number of milliseconds to wait
*/
void timer_start(uint8_t timer_no, unsigned int delay_ms);

/** 
 Return true if the given timer is complete
*/
unsigned char timer_complete(uint8_t timer_no);



#endif /* __TIMERS_2_H__ */


