#ifndef __TIMERS_2_CONF_H__
#define __TIMERS_2_CONF_H__


// ---- Timer configuration. Use timer 2 ----
#define NB_TIMER 5

typedef unsigned long int TIMER_TYPE;

// Which are the timers :
// --------------------
#define TIMER_10_MS 0           /* Send info on UART periodicaly */
#define TIMER_10_MS_DELAY 10

#define TIMER_TEMP 1
#define TIMER_TEMP_DELAY 500

#define TIMER_LCD 2
#define TIMER_LCD_DELAY 200

#define TIMER_TURN90 3
#define TIMER_TURN90_DELAY 1050

#define TIMER_RECULE 4
#define TIMER_RECULE_DELAY 1000

// Time between timer interrupts in us.
#define TM2_TICK_DELAY_US  1024

#endif /* __TIMERS_2_CONF_H__ */
