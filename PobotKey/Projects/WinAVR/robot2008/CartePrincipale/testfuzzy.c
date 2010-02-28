//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include "math.h"
#include "global.h"		// include our global settings

#include "timer.h"
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "vt100.h"		// include vt100 terminal support
#include "i2c.h"		// include i2c 
#include "servo.h"		// include servo library

#include "lcd03.h"
#include "Fuzzy.h"

#define SERVO_PINCE_GAUCHE 	0
#define SERVO_PINCE_DROITE	1

#define POS_PINCE_GAUCHE_OUVERT  0
#define POS_PINCE_GAUCHE_FERME 	255

#define POS_PINCE_DROITE_OUVERT 255
#define POS_PINCE_DROITE_FERME 	 0


//
double over_count;
int second_count;

// position courante
double x,y,theta;
// position cible
double cx,cy,ctheta;
// vitesses moteurs
double vg,vd;

// entrée du générateur flou
float inputs[2] = {0, 0};
// sortie du générateur flou
float outputs[2] = { 0, 0};
// la distance à parcourir jusqu'au prochain point
float d = 0;
// l'angle entre la direction du robot et le prochain point
float a = 0;

void interruptionTimer1(void);

#define SPEED_SERVO	1

int main(void)
{	
	float distX,distY;
	
	// initialize peripherals
	timerInit();
	i2cInit();
	
	// initialize the UART (serial port)
	uartInit();
	uartSetBaudRate(9600);
	// make all rprintf statements use uart for output
	rprintfInit(uartSendByte);
	// initialize vt100 terminal
	vt100Init();
	
	rprintfCRLF();
	rprintf("test fuzzy");
	rprintfCRLF();
	
	//
	timer1Init();
	timer1SetPrescaler(TIMER_CLK_DIV64);
	timerAttach(TIMER1OVERFLOW_INT,interruptionTimer1);
	
	// initialize RC servo system
	servoInit();
	servoSetChannelIO(SERVO_PINCE_GAUCHE, _SFR_IO_ADDR(PORTB), PB0);
	servoSetChannelIO(SERVO_PINCE_DROITE, _SFR_IO_ADDR(PORTB), PB7);
	//
	servoSetPosition(SERVO_PINCE_GAUCHE,POS_PINCE_GAUCHE_OUVERT);	
	timerPause(SPEED_SERVO);
	
	// set port pins to output
	outb(DDRB, 0xFF);
	outb(PORTB, 0x00);
	// init LCD03
	lcd03Init();
	
	// init ports
	DDRA = 0xFF;
	PORTA = 0x00;
	// init buzzer
	sbi(DDRD,4);
	sbi(PORTD,4);
	
	// init variables (inutile pour l'instant)
	x = 0;
	y = 0;
	theta = 0;
	cx = 200;
	cy = 150;
	ctheta = 90;
	
	while (1) {
		// glitch de test
		sbi(PORTA,0);
		
		// test des servos
		if (second_count == 5) {
		servoSetPosition(SERVO_PINCE_GAUCHE,POS_PINCE_GAUCHE_OUVERT);	
	}
	if (second_count == 10) {
		servoSetPosition(SERVO_PINCE_GAUCHE,POS_PINCE_GAUCHE_FERME);	
	}
		
		// compute distance
		distX = x - cx;
		distY = x - cy;
		// compute distance
		d = sqrt(distX*distX+distY*distY);
		// compute angle
		a = atan(distY/distX)/PI*180;
		// ask fuzzy
		inputs[0] = d;
		inputs[1] = a;
		
		// pour oscillo
		cbi(PORTA,0);
		
		// appel fuzzy
		fuzzy_step(inputs, outputs);
	   
		// read vitesses moteur
		vg = outputs[0];
		vd = outputs[1];
		
		rprintf("Vg : %d | Vd : %d",vg,vd);
	}
	
	return 0;
}

/**
 * Interruption sur overflow du compteur 16 bits
 */
void interruptionTimer1(void)
{
	// 50 ms
	over_count++;
	
	// réarmement
	TCNT1H = 0xCF;
	TCNT1L = 0xDB;
	sbi(TIMSK, TOIE1);	
	
	// lecture/écriture I2C
	lcd03Display("Je suis content",1,2);
	
	// lecture I/O
	
	// stratégie
	
	
	
	// compter les secondes
	if (over_count >= 20) {
		over_count = 0;
		second_count++;
		// flash
		if (second_count % 2) {
			sbi(PORTA,1);
			sbi(PORTD,4);
		} else {
			cbi(PORTA,1);
			cbi(PORTD,4);
		}
	}
	
	
	
			

}
