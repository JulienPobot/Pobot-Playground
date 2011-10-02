/*L'électronique vous aurez besoin est: 
 - un microcontrôleur (un Arduino dans mon exemple): obligatoire. 
 - un contrôleur de moteur: en option, c'est que si vous n'utilisez pas servomoteur à conduire votre robot (dans mon exemple j'utilise un conseil RoboClaw) . 
 - la résistance de pull-up pour les canaux A et B. encodeurs 
 - Source d'alimentation et le régulateur 5V. Voici les liens que vous avez à faire: - 
 Sur la gauche codeur, relier le canal A de la D2 broches (numérique) de l'Arduino , 
 et le canal B à la broche de l'Arduino D10 - Sur le droit de l'encodeur, 
 relier le canal A de l'axe de la D3 Arduino, et le canal B à la broche A15 (analogique) de l'Arduino 
 (Ne pas oublier le résistances de pull-up!) 
 Vous pouvez le voir sur la photo de mon prototype pour le concours Eurobot.
 http://www.instructables.com/id/Position-estimation-of-a-wheeled-bot-with-arduino/step2/Electronics/
 http://www.instructables.com/id/Position-estimation-of-a-wheeled-bot-with-arduino/step3/The-source-code/
 */


/*
* ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * JBot wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

// Other includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

/***********/
/* Defines */
/***********/
#define TICK_PER_MM_LEFT  90.9456817668
#define TICK_PER_MM_RIGHT  90.9456817668
#define DIAMETER  166.0 // Distance between the 2 wheels (in millimeter)

#define TWOPI 6.2831853070
#define RAD2DEG 57.2958 /* radians to degrees conversion */

/********************/
/* Global variables */
/********************/
volatile long left_cnt = 0;
volatile long right_cnt = 0;

double pos_X = 0;
double pos_Y = 0;
double theta = 0;

int last_left = 0;
int last_right = 0;

int left_diff = 0;
int right_diff = 0;

double total_distance = 0.0;

/***********************/
/* INTERRUPT FUNCTIONS */
/***********************/

// External Interrupt 4 service routine => PIN2
ISR(INT4_vect)
{
  //#asm("cli")
  if ((PINB & 0x10) != 0) {
    if ((PINE & 0x10) != 0)
      left_cnt--;
    else
      left_cnt++;
  } 
  else {
    if ((PINE & 0x10) == 0)
      left_cnt--;
    else
      left_cnt++;
  }

  //#asm("sei")
}

// External Interrupt 5 service routine => PIN3
ISR(INT5_vect)
{
  if ((PINK & 0x80) != 0) {
    if ((PINE & 0x20) != 0)
      right_cnt++;
    else
      right_cnt--;
  } 
  else {
    if ((PINE & 0x20) == 0)
      right_cnt++;
    else
      right_cnt--;
  }

}

// Pin change 0-7 interrupt service routine => PIN10
ISR(PCINT0_vect)
{
  if ((PINE & 0x10) != 0) {
    if ((PINB & 0x10) != 0) {
      left_cnt++;
    } 
    else
      left_cnt--;
  } 
  else {
    if ((PINB & 0x10) == 0) {
      left_cnt++;
    } 
    else
      left_cnt--;
  }

}

// Pin change 16-23 interrupt service routine => PIN-ADC15
ISR(PCINT2_vect)
{
  if ((PINE & 0x20) != 0) {
    if ((PINK & 0x80) != 0)
      right_cnt--;
    else
      right_cnt++;
  } 
  else {
    if ((PINK & 0x80) == 0)
      right_cnt--;
    else
      right_cnt++;
  }

}

// Timer 1 overflow interrupt service routine
ISR(TIMER1_OVF_vect)
{
  sei(); // enable interrupts
  get_Odometers();

}


/*************************/
/* SYSTEM INITIALIZATION */
/*************************/
void setup()
{
  // Crystal Oscillator division factor: 1
#pragma optsize-
  CLKPR = 0x80;
  CLKPR = 0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif



  // Input/Output Ports initialization
  // Port A initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
  PORTA = 0x00;
  DDRA = 0x00;

  // Port B initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
  PORTB = 0x00;
  DDRB = 0x00;

  // Port C initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
  PORTC = 0x00;
  DDRC = 0x00;

  // Port D initialization
  // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
  // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
  PORTD = 0x00;
  DDRD = 0x00;

  // Port E initialization
  // Func2=In Func1=In Func0=In
  // State2=T State1=T State0=T
  PORTE = 0x00;
  DDRE = 0x00;

  PORTK = 0x00;
  DDRK = 0x00;


  /**********************/
  /* I/O INITIALIZATION */
  /**********************/
  // OUTPUTS
  // pinMode(13, OUTPUT);



  // Timer/Counter 1 initialization
  // Clock source: System Clock
  // Mode: Ph. correct PWM top=00FFh
  // OC1A output: Discon.
  // OC1B output: Discon.
  // OC1C output: Discon.
  // Noise Canceler: Off
  // Input Capture on Falling Edge
  // Timer 1 Overflow Interrupt: On
  // Input Capture Interrupt: Off
  // Compare A Match Interrupt: Off
  // Compare B Match Interrupt: Off
  // Compare C Match Interrupt: Off
  TCCR1A = 0x01;
  TCCR1B = 0x04;
  TCNT1H = 0x00;
  TCNT1L = 0x00;
  ICR1H = 0x00;
  ICR1L = 0x00;
  OCR1AH = 0x00;
  OCR1AL = 0x00;
  OCR1BH = 0x00;
  OCR1BL = 0x00;
  OCR1CH = 0x00;
  OCR1CL = 0x00;

  // External Interrupt(s) initialization
  EICRA = 0x00;
  EICRB = 0x05;
  EIMSK = 0x30;
  EIFR = 0x30;
  // Interrupt on PCINT
  PCICR = 0x05;
  PCIFR = 0x05;
  PCMSK0 = 0x10;
  PCMSK1 = 0x00;
  PCMSK2 = 0x80;



  // Timer(s)/Counter(s) Interrupt(s) initialization
  TIMSK1 |= 0x01;
  TIFR1 |= 0x01;


  /******************************/
  /* Initialization of the code */
  /******************************/


  // Global enable interrupts
  sei();

}

/******************/
/* MAIN CODE LOOP */
/******************/
void loop()
{


}


/********************************/
/* POSITION ESTIMATION FUNCTION */
/********************************/

/* Compute the position of the robot */
void get_Odometers(void)
{
  long left_wheel = 0;
  long right_wheel = 0;

  double left_mm = 0.0;
  double right_mm = 0.0;

  double distance = 0.0;


  left_wheel = left_cnt;
  right_wheel = right_cnt;

  left_diff = last_left - left_wheel;
  right_diff = last_right - right_wheel;

  last_left = left_wheel;
  last_right = right_wheel;

  left_mm = ((double) left_diff) / TICK_PER_MM_LEFT;
  right_mm = ((double) right_diff) / TICK_PER_MM_RIGHT;

  distance = (left_mm + right_mm) / 2;
  total_distance += distance;

  theta += (right_mm - left_mm) / DIAMETER;

  if (theta > PI)
    theta -= TWOPI;
  if (theta < (-PI))
    theta += TWOPI;

  pos_Y += distance * sin(theta);
  pos_X += distance * cos(theta);

}



