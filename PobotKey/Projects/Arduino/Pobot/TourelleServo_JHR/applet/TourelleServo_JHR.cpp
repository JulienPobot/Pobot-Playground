#include <Servo.h>
#include <MsTimer2.h>

/** 
 * Programme de gestion d'un "radar" 
 * avec 1 capteur (US ou IR) sur un servomoteur
 */

// les limites du servo
#define LEFT  5
#define RIGHT 175

//
#define INCREMENT  1

// la position de consigne du servo
#include "WProgram.h"
void setup();
void loop();
void turn();
int position = LEFT;
int sens = RIGHT;

boolean update = false;

// le servo
Servo serv;




void setup()
{
  // initialisation de la liaison s\u00e9rie
  Serial.begin(38400);

  //

  // initialisation du servomoteur sur la patte num\u00e9rique 9
  serv.attach(9);

  // position de d\u00e9part
  serv.write(position); 

  // initialisation du timer
  MsTimer2::set(50, turn); // une p\u00e9riode de 50 millisecondes
  MsTimer2::start();
}

void loop()
{
  if (update)
  {
    // remettre \u00e0 z\u00e9ro
    update = false;

    // envoyer la nouvelle consigne au servo
    serv.write(position);

    // lire le capteur 
    int val = analogRead(0);

    // envoyer sur la liaison s\u00e9rie
    Serial.print( 0xff, BYTE);
    Serial.print( (position >> 8) & 0xff, BYTE);
    Serial.print( position & 0xff, BYTE);
    
    Serial.print( (sens==LEFT?0x00:0x01) );
       
    Serial.print( (val >> 8) & 0xff, BYTE);
    Serial.print( val & 0xff, BYTE);
    Serial.println();
  }

}

void turn()
{
  if (sens == RIGHT) 
  {
    if (position >= RIGHT)
    {
      position -= INCREMENT;    
      sens = LEFT;
    } 
    else {
      position += INCREMENT;
    }
  } 
  else {
    if (position <= LEFT)
    {
      position += INCREMENT;
      sens = RIGHT;
    } 
    else {
      position -= INCREMENT;
    }
  }
  update = true;
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

