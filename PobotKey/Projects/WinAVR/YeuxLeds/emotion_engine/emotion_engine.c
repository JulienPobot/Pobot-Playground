/**
 * Pobot 2008
 * "emotion engine" du projet "Yeux"
 * v1	Nicolas Boulay (nicolas@pobot.org)
 * v2	Julien Holtzer (julien@pobot.org)
 */
 
/**
 * Versions (date + Comm)
 * - 02/08/2008 : ajout de commentaires
 */

#include "global.h"			// include our global settings

#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <util/crc16.h>
#include <util/delay.h>

#include "uart.h"			// include AVRLIB uart function library

#include "dbimage.h"		
#include "emotion_engine.h"

#include "../common.h"

static u08 trame[12];

/*---------- internal ---------*/
static u08 crc_run(void);
static void out_init(void);
static void out_trame_create(uint8_t screen_id, uint16_t image_id);
static void out_send(void);
static void send_image(uint8_t cmd, uint8_t cnt, uint8_t screen_id);
static void delayms(uint16_t millis);
/*---------------------------------------------*/

/**
 * Programme principal du "emotion engine"
 *
 */
int main(void)
{
  uint8_t cnt=0;
  uint8_t cmd=0;
  out_init();
  DDRD = 1 << PD3;/*Led*/
  while(1){
    if( (PIND & 1<<PD2) == 0){ /*Button press*/
      delayms(50);
      cmd++;
      cnt=0;
      PORTD= PORTD ^ (1 << PD3); /*green Led state  change*/
    }
    if(cmd == sizeof(clip)/sizeof(struct clip_t)){
      cmd = 0;
    }

    send_image(cmd,cnt,SCREEN_G);
    send_image(cmd,cnt,SCREEN_D);
    cnt++;
  }
  
  return 0;
}

/*--------------- out ------------------------------*/
static void delayms(uint16_t millis) {
  uint16_t loop;
  while ( millis ) {
    _delay_ms(1);
    millis--;
  }
}

static void out_init(void)
{
  uartInit();
  uartSetBaudRate(2400); // 10 images/s max
}

/**
 * Creation d'une trame
 * - remplir la trame qui sera envoyée à la carte "yeux"
 */
static void out_trame_create(uint8_t screen_id, uint16_t image_id)
{
  trame[0] = 0xA5; // ??
  trame[1] = screen_id;

  memcpy_P(&(trame[2]),&(dbimage[image_id][0]),8);
  trame[10] = crc_run();
}

static void out_send(void)
{
	uartSendBuffer(trame,(u16)sizeof(trame));
  //while(uartSendBuffer((char*)trame,sizeof(trame))==FALSE);
}

static uint8_t crc_run()
{
  uint8_t crc = 0, i;
  
  for (i = 0; i < (sizeof (trame)-1); i++)
    crc = _crc_ibutton_update(crc, trame[i]);
  
  return crc;
}

static uint16_t find_image(uint8_t cmd, uint8_t cnt, uint8_t screen)
{
  const uint16_t start_index =   pgm_read_word (&(clip[cmd].start_index));
  const uint8_t size =   pgm_read_byte(&(clip[cmd].size));
  if (cnt > size){
    cnt = size;
  }
 
  return  pgm_read_byte(&(dbvideo[start_index+cnt][screen]));
}

static void send_image(uint8_t cmd, uint8_t cnt, uint8_t screen_id)
{
  uint16_t image_id =  find_image(cmd, cnt, screen_id);
  out_trame_create(screen_id, image_id);
  out_send();
}
