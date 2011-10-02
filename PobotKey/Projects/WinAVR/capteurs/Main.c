/*
    Title:    Maro AVR-GCC Ver 0.2 User Application Main.c    
    http://www.microrobot.com
*/

//#include <io.h>

//#include <interrupt.h>
#include <iom163v.h>
#include <macros.h>
#include <signal.h>
#include <stdlib.h>

#include <math.h>
 
//. eric : #define SPEED_BASE 60
//. moi :
//. un peu lent : #define SPEED_BASE 4
#define SPEED_BASE 80
//. eric : #define ACCELERATION_DURATION 4
//. moi :
#define ACCELERATION_DURATION 1

//. eric #define HALF_ROTATION_STEPS 2740
//. moi :
//. #define HALF_ROTATION_STEPS 274 //. 45°
//. #define HALF_ROTATION_STEPS 1096 //. 180 + 30 °
//. #define HALF_ROTATION_STEPS 900 //. 180 - 30
//. #define HALF_ROTATION_STEPS 1000 //. 30°
//. #define HALF_ROTATION_STEPS 3000 //. 360 + 180

#define HALF_ROTATION_STEPS 1000 
//. 880
//. 850 //. 1000
#define QUARTER_ROTATION_STEPS (HALF_ROTATION_STEPS / 2)
//. #define QUARTER_ROTATION_STEPS 600 
//. 440 
//. 400
 //. 425 //. 500

// phases moteurs
#define STEPS_COUNT 8
unsigned char StepPhase_L [STEPS_COUNT] = {0x0A,0x02,0X06,0x04,0x05,0x01,0x09,0x08};
unsigned char StepPhase_R [STEPS_COUNT] = {0xA0,0x20,0X60,0x40,0x50,0x10,0x90,0x80};

// sélecteurs de phases
signed char Step_L, Step_R ;

/* Compteurs odométriques

	Remarque :
	1/ l'unité utilisée est le *demi-pas*, afin de pouvoir travailler en entiers 
		 au niveau de l'odométrie du centre,
	2/ ces compteurs sont déclarés en volatile car modifiés par les routines
		 d'interruption
*/
volatile long Odo_L, Odo_R ;		// odométrie des moteurs
volatile long Odo_Robot ;			// odométrie du centre géométrique du robot

// vitesse actuelle (<=> fréquence des impulsions) de chaque moteur
volatile signed char Speed_L, Speed_R ;

// consignes de vitesse des moteurs
signed char RefSpeed_L, RefSpeed_R ;

// delta-v (<=> accélération) de chaque moteur
signed char dSpeed_L, dSpeed_R ;

//. infinite loop 
int nbLoop ;
int nbCote ;
int maxLoop ;

//. capteurs d'obstacle
int read ;

//. indicateurs d'interruption sur obstacle
//. volatile because modified by interruptions
volatile int obstacleLeft = 0 ;
volatile int obstacleRight = 0 ;

/*
	Gestionnaire de l'interruption d'overflow du timer du moteur droit

	L'interruption est reçue lorsque le timer passe en overflow (ici, à 256
	puisqu'il s'agit d'un compteur 8 bits).	
*/

#pragma interrupt_handler overflow0_interrupt:iv_TIMER0_OVF
//INTERRUPT(SIG_OVERFLOW0) {
void overflow0_interrupt (void) {
  static volatile char cnt_R = ACCELERATION_DURATION ;
	
//. if obstacle, stop
//.
 /*
 read = PIND ;
  if( ( read & 0x80 ) == 0 ) {
  } else {
    Odo_L = -1 ;
    Odo_Robot = -1 ;
  }
//.
 */

  // ajustement de vitesse par rapport à la consigne, si nécessaire
	if (Speed_R != RefSpeed_R) {
  	if (cnt_R == 0) {
      if (Speed_R < RefSpeed_R) {
    		Speed_R++ ; 
    	} else {
    		Speed_R-- ;
    	}
  		cnt_R = ACCELERATION_DURATION ;
  	}
  	cnt_R-- ; 
	}
	
  if (Speed_R != 0){
	  // détermination du sens d'évolution des phases
	  signed char inc = (Speed_R > 0) ? 1 : -1 ;
		// sélection du prochain pattern de phases
		/*
			modulo semble ne pas marcher correctement => remplacé par un masque
		*/
  	Step_R = (Step_R + inc) & (STEPS_COUNT - 1) ;

		// mise à jour du compteur odométrique du moteur
  	Odo_R += (inc * 2) ;

		// idem pour le centre (delta_L + delta_R) / 2)		
		Odo_Robot += inc ;
		
		// envoi du nouveau pattern de phases au contrôle du moteur 
  	//outp((inp(PORTC) & 0x0f) | StepPhase_R [Step_R], PORTC);
		PORTC = (PORTC & 0x0f) | StepPhase_R [Step_R] ;
  }

	// réarmement du timer avec la consigne de vitesse du moteur
	// => va repartir en incrémentant à partir de cette valeur
  //outp(2 * abs(Speed_R), TCNT0);
  TCNT0 = 2 * abs(Speed_R) ;
}

// gestionnaire de l'interruption d'overflow du timer du moteur gauche

#pragma interrupt_handler overflow2_interrupt:iv_TIMER2_OVF
//INTERRUPT(SIG_OVERFLOW2) {
void overflow2_interrupt (void) {

  static volatile char cnt_L = ACCELERATION_DURATION ;
	
//. if obstacle, stop
//.
 /*
	read = PIND ;
	if( ( read & 0x80 ) == 0 ) {
	} else {
	  Odo_L = -1 ;
	  Odo_Robot = -1 ;
	}
//.
 */

  // ajustement de vitesse par rapport à la consigne, si nécessaire
  if (Speed_L != RefSpeed_L) {
  	if (cnt_L == 0) {
      if (Speed_L < RefSpeed_L) {
    		Speed_L++ ; 
    	} else {
    		Speed_L-- ;
    	}
  		cnt_L = ACCELERATION_DURATION ;
  	} 
  	cnt_L-- ;
	} 
	
  if (Speed_L != 0){
	  // détermination du sens d'évolution des phases
	  signed char inc = (Speed_L > 0) ? 1 : -1 ;
		// sélection du prochain pattern de phases
  	Step_L = (Step_L + inc) & (STEPS_COUNT - 1) ;

		// mise à jour du compteur odométrique du moteur
  	Odo_L += (inc * 2) ;

		// idem pour le centre (delta_L + delta_R) / 2)		
		Odo_Robot += inc ;
		
		// envoi du nouveau pattern de phases au contrôle du moteur 
  	//outp((inp(PORTC) & 0xf0) | StepPhase_L [Step_L], PORTC);
		PORTC = (PORTC & 0xf0) | StepPhase_L [Step_L] ;
  }

	// réarmement du timer avec la consigne de vitesse du moteur
	// => va repartir en incrémentant à partir de cette valeur
  //outp(2 * abs(Speed_L), TCNT2);
  TCNT2 = 2 * abs(Speed_L) ; 
}

// initialisation des ports I/O utilisés

void init_port(void){
	//outp( 0x06, DDRA ); 	// PA1, PA2 : out (enable moteurs)
	DDRA = 0x06 ;
	//outp( 0x0f, DDRB );		// PB0..PB3 : out
	DDRB = 0x0f ;
	//outp( 0xff, DDRC ); 	// PC0..PC7 : out (pour le pilotage des moteurs)
	DDRC = 0xff ;
	//outp( 0x00, DDRD );		// PD0..PD7 : in
	DDRD = 0x00 ;
}

// initialisation timer 0 (pulse moteur droit)

void init_timer0(void){
	//outp(0x03,TCCR0); // pré diviseur de 64 pour le timer 0 (pour une résolution de 0,8 E -5 sec)
	//. eric : TCCR0 = 0x03 ;
	//. moi :
	TCCR0 = 0x04 ;
	//sbi(TIMSK, 0);	  // autorise l'interrupt d'overflow du timer 0
	TIMSK |= 1 ;
	//outp(0x00,TCNT0); // mise à 0 du timer 0
	TCNT0 = 0 ;
}

// initialisation timer 2 (pulse moteur gauche)

void init_timer2(void){
	//outp(0x04,TCCR2); // pré diviseur de 64 pour le timer 2 (pour une résolution de 0,8 E -5 sec)
	//. eric : TCCR2 = 0x04 ;
	//. moi :
	TCCR2 = 0x06 ;
	//sbi(TIMSK, 6);	 	// autorise l'interrupt d'overflow du timer 2
	TIMSK |= (1 << 6) ;
	//outp(0x00,TCNT2); // mise à 0 du timer 2
	TCNT2 = 0 ;
}


void init_int(void){
 //sbi(GIMSK,6);
 GIMSK |= (1 << 6) ; //. interrupt mask
 //sbi(GIMSK,7);
 GIMSK |= (1 << 7) ;
 //outp(0x0a,MCUCR);
 MCUCR = 0x0F ; //. rising edge //. 0x0a ; front down
}

// initialisation globales
void init_program(void){
	Step_L = 0 ;
	Step_R = 0 ;
	Speed_L = 0 ;
	Speed_R = 0 ;
	RefSpeed_L = 0 ;
	RefSpeed_R = 0 ;
	Odo_R = 0 ;
	Odo_L = 0 ;
	Odo_Robot = 0 ;
}

// activation des moteurs
void enable_motors(void){
 //outp(0x06,PORTA);
 PORTA = 0x06 ;
}

// désactivation des moteurs
void disable_motors(void){
	// outp(0x00,PORTA);
	PORTA = 0 ;
}

// arrêt moteurs
void stop_motors( void ) {
  RefSpeed_L = 0 ;
  RefSpeed_R = 0 ;
  while( ( Speed_L != 0 ) || ( Speed_R != 0 ) );
}

// arrêt complet
void full_stop () {
  stop_motors() ;
  disable_motors() ;
}

// routine de temporisation
void delay_1ms(unsigned int i) {
  unsigned int j ;
  //. disable_motors() ;
  while( i-- ) {
    j = 2000 ;   // 8Mhz
    while( j-- );
  }
  //. enable_motors() ;
}

// rotation vers la gauche, sans mouvement du centre
void hard_turn_left( unsigned short steps ) {
  // rotation des moteurs en sens inverse
  RefSpeed_R = SPEED_BASE ;
  RefSpeed_L = -SPEED_BASE ;
  //. enable_motors() ;
	
  // maintien de la commande pendant le temps demandé
  Odo_R = 0 ;
  while( Odo_R < steps );
  //. while( ( Odo_R != -1 ) && ( Odo_R < steps ) );
  
  //. disable_motors() ;
}

// rotation vers la droite, sans mouvement du centre
void hard_turn_right (unsigned short steps) {
  // rotation des moteurs en sens inverse
  RefSpeed_R = -SPEED_BASE ;
  RefSpeed_L = SPEED_BASE ;
  enable_motors() ;
	
  // maintien de la commande pendant le temps demandé
  Odo_L = 0 ;
  while( Odo_L < steps );
  //. while( ( Odo_L!= -1 ) && ( Odo_L < steps ) );
  
  //. disable_motors() ;
}

void hard_quarter_turn_left( void ) {
  hard_turn_left( QUARTER_ROTATION_STEPS );
}

void hard_half_turn_left( void ) {
  hard_turn_left( HALF_ROTATION_STEPS );
}

void hard_quarter_turn_right( void ) {
  hard_turn_right( QUARTER_ROTATION_STEPS );
}

void hard_half_turn_right( void ) {
  hard_turn_right( HALF_ROTATION_STEPS );
}

//. #pragma interrupt_handler int0_isr:2
#pragma interrupt_handler int0_isr:iv_INT0
//. external interupt on INT0
void int0_isr(void) {
   //. full_stop();
	 obstacleRight = 1 ;
}

#pragma interrupt_handler int1_isr:iv_INT1
//. external interupt on INT1
void int1_isr(void) {
   //. full_stop();
	 obstacleLeft = 1 ;
}

// avance rectiligne d'un nombre de pas donné, avec démarrage et arrêts 
// progressifs
void straight_line( int steps ) {
/*
  long target = Odo_Robot + steps * 2 ;

  RefSpeed_R = SPEED_BASE ;
  RefSpeed_L = SPEED_BASE ;
	*/
	long target = Odo_Robot ;
	if( steps < 0 ) {
	  target -= steps * 2 ;
    RefSpeed_R = -SPEED_BASE ;
    RefSpeed_L = -SPEED_BASE ;
	} else {
	  target += steps * 2 ;
    RefSpeed_R = SPEED_BASE ;
    RefSpeed_L = SPEED_BASE ;
	}
  enable_motors() ;
  while( Odo_Robot < target );
  //. while( ( Odo_Robot != -1 ) && ( Odo_Robot < target ) );
}

void main( void ) {
	nbLoop = 0 ;
	maxLoop = 4 ;

	CLI();
	init_program();
	init_port();
	init_int();
	init_timer0();
	init_timer2();
	SEI();
	
	// temporisation de départ (pour avoir le temps d'enlever les mains !)
	delay_1ms( 1000 );		

/* 11/10/2005 : il me semble que le param
  de avance ou tourne ne sert à rien
	mais que la fonction s'exec during le temps
	donné en param à delay !!!
	
	Si je remplace delay_1ms par stop_motors(),
	il ne fait pas les fonctions
	
	Si je mets stop_motors() après le delay,
	il ne sert à rien
	*/

    straight_line( 10 );
	delay_1ms( 5000 ); //. environ 8s !!!
  stop_motors() ;	
		hard_turn_right( 20 );	 
 	delay_1ms( 5000 );	
  stop_motors() ;	
    straight_line( 10 );
 	delay_1ms( 5000 );	
  stop_motors() ;	
		hard_turn_left( 20 );
 	delay_1ms( 5000 );
  stop_motors() ;	
 
maxLoop = 0 ;
while( nbLoop < maxLoop ) {

	if( obstacleRight ) {
    stop_motors() ;
	  //. reculer
    straight_line( -5 );	 
    delay_1ms( 1000 );	
		//. reset flag
		obstacleRight = 0 ;
	  //. turn right
    hard_turn_right( 5 );
    delay_1ms( 1000 );	
		 
  } else if( obstacleLeft ) {
    stop_motors() ;
	  //. reculer
    straight_line( -5 );	 
    delay_1ms( 1000 );	
		//. reset flag
		obstacleLeft = 0 ;
	  //. turn left
    hard_turn_left( 5 );
    delay_1ms( 1000 );	
		 
	} else {
	  //. avancer
    straight_line( 5 );	 
    delay_1ms( 1000 );
		
	} 

	//. nbLoop++ ;	
}

   full_stop();
}

