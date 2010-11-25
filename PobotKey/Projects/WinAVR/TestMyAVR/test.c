#include <avr/io.h>
#include <util/delay.h>

int A,B,C,var=0;

void init(void)
{  
SREG= 0b10000000;
DDRA = 0b01111111;
ADMUX= 0b01100111;
ADCSRA=0b11101000;

}

void ISR(int _VECTOR(14))
 {            

var = ADCH/0.5;
A = (int)(var/100);
B = (int)((var/10)-(10*A));
C = (int)((var)-(10*B)-(100*A));
 }


int main(void)
{

init();  

while(1)
{
PORTA = 0b01110000;
_delay_ms(0.001);
PORTA = 0b01100000 + A;
_delay_ms(0.001);
PORTA = 0b01010000 + B;
_delay_ms(0.001);
PORTA = 0b00110000 + C;
_delay_ms(0.001);
} 
}