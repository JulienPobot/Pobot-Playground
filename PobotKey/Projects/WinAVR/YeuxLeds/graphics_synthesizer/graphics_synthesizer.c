#include "global.h"		// include our global settings
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
//#include <avr/interrupt.h>	// include interrupt support
#include <util/crc16.h>
#include <string.h>
#include "../common.h"
//#include <util/delay_basic.h>
/*---------- Global ---------*/
static  int8_t screen_id;

/*---------- internal ---------*/

static void screen_id_init(void);

static  void init_display(void);
//void display(uint8_t frame[8]);
static   void display_step(uint8_t frame[8],int8_t step);

static  void uartInit(u32 baudrate);
static  int8_t uartGetByte(char * c);
static  void uartSetBaudRate(u32 baudrate);

static  uint8_t trame_get(uint8_t i);
static  void trame_push(uint8_t val);
static  int trame_ok(void);
static  uint8_t crc_run(void);
/*---------------------------------------------*/

uint8_t display_cur[8] = {0xAA,0x55,0xAA,0x55, 0xAA,0x55,0xAA,0x55};
const uint8_t display_cur_g[8] = {0x0,0x3c,0x3c,0x3c, 0x3c,0x34,0x3c,0x00};

int main(void)
{
  int8_t i=0; char c; 

  screen_id_init();
  uartInit(2400); // ~20 image/s 
  init_display();

  if(screen_id == SCREEN_G){
    memcpy(display_cur, display_cur_g,8);
  }

  while(1){
    display_step(display_cur,i);
    // _delay_loop_2(6000); //~20ms.
    i=(i+1) % 8;
    
    if(uartGetByte(&c)){
      trame_push(c);
      if(trame_ok()){
	uint8_t j;
	for (j = 2; j < 11; j++){
	  display_cur[j-2]=trame_get(j); 
	}
      }
    }
    
  }
  return 0;
}

/*---------------------------------------------*/
// bit PD6 to be read before init_display()
static void screen_id_init(void)
{
  DDRD=0;
  if(PIND6 & (1<<PIND6)) {
    screen_id = SCREEN_D;
  } else {
    screen_id = SCREEN_G;
  }
}
/*---------------------------------------------*/
uint8_t trame[11];
uint8_t trame_index=0;
static  uint8_t trame_get(uint8_t i)
{
  return trame[(i+trame_index) % sizeof(trame)];
}

static  void trame_push(uint8_t val)
{
  trame[trame_index]=val;
  trame_index=(trame_index+1) % sizeof(trame);
}

static  uint8_t crc_run(void)
{
  uint8_t crc = 0, i;
  
  for (i = 0; i < (sizeof (trame)-1); i++)
    crc = _crc_ibutton_update(crc, trame_get(i));
  
  return crc; 
}

static  int trame_ok(void)
{
  
  if(trame_get(0)==0xA5){
      if((trame_get(1) == screen_id) || 
	 (trame_get(1) == SCREEN_ALL)){
	//if(crc_run()==trame_get(10))
	  {
	    return 1;
	  }
      }
  }
  return 0;
}

/*---------------------------------------------*/

static  void init_display(void)
{
  DDRB=0xFF; /*colomn wire*/
  DDRA=0x3;  /*row 0 1*/
  DDRD=0x7E; /*row 2 - 7*/
}

/* the columns are on the portb
 * the rows on the port A and D
 * columns are connected with P-MOS
 * row with N-MOS
 * P-MOS are closed with gate at ground
 * N-MOS are closed with gate at VCC
*/
static  void display_step(uint8_t frame[8],int8_t step)
{
  switch(step){
  case 0 :PORTB=~frame[0]; PORTA=1 << PA0; PORTD=0;       break;
  case 1 :PORTB=~frame[1]; PORTA=1 << PA1; PORTD=0;       break;
  case 2 :PORTB=~frame[2]; PORTA=0;        PORTD=1 << PD1;break;
  case 3 :PORTB=~frame[3]; PORTA=0;        PORTD=1 << PD2;break;
  case 4 :PORTB=~frame[4]; PORTA=0;        PORTD=1 << PD3;break;
  case 5 :PORTB=~frame[5]; PORTA=0;        PORTD=1 << PD4;break;
  case 6 :PORTB=~frame[6]; PORTA=0;        PORTD=1 << PD5;break;
  case 7 :PORTB=~frame[7]; PORTA=0;        PORTD=1 << PD6;break;
  }
}

/*---------------------------------------------*/
static  void uartSetBaudRate(u32 baudrate)
{
	// calculate division factor for requested baud rate, and set it
	u16 bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
	outb(UBRRL, bauddiv);
	#ifdef UBRRH
	outb(UBRRH, bauddiv>>8);
	#endif
}

static  void uartInit(u32 baudrate)
{
	// enable RxD/TxD and interrupts
	//outb(UCR, BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN));
        outb(UCSRB, BV(RXEN));
	// set default baud rate
	uartSetBaudRate(baudrate);  
}

static  int8_t uartGetByte(char * c)
{
  if(UCSRA & (1<<RXC)){
    *c=UDR;
    return 1;
  } else {
    return 0;
  }
}
