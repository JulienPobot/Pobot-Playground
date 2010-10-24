#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "timers_2_conf.h"
#include "timers_2.h"

#ifdef CARTE_ACTIONNEURS  // For encodeur logitech
 #include "hardware.h"
 #include <util/delay.h>
 #include "encodeur_carrousel.h"
#endif

TIMER_TYPE timer_counters[NB_TIMER];
uint16_t g_free_running_counter;


#if NB_TIMER <8
unsigned char timer_running;
#elif NB_TIMER < 16
unsigned int timer_running;
#else
#warning "Too many timers"
#endif

volatile int8_t timer_ticks_to_increment; // Incremented on each interrupt
ISR(TIMER2_COMPA_vect)
{
  timer_ticks_to_increment ++;
}

/**
 Should be called periodicaly from the main loop.
 It takes care of decrementing the timers when the interrupts have ticked the flag
 */
void timer_handle(void)
{
  while (timer_ticks_to_increment > 0) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      timer_ticks_to_increment--;
      g_free_running_counter++;
    }

    for (uint8_t i = 0; i< NB_TIMER; i++) {
      if ( (timer_running & _BV(i)) &&
          timer_counters[i]-- == 0) {
        timer_running &= ~_BV(i);
      }
    }
  }
}


void timer_start(uint8_t timer_no, unsigned int delay_ms)
{
    timer_counters[timer_no] = delay_ms * 1000UL / TM2_TICK_DELAY_US;
    timer_running |= _BV(timer_no);
}

unsigned char timer_complete(uint8_t timer_no)
// Return True is the timer has endend
{
  return !(timer_running & _BV(timer_no));
}


