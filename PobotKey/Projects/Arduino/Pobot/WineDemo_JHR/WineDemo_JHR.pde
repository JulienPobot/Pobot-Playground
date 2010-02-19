
#include "Wire.h"
#include "BlinkM_funcs.h"

typedef struct _wine_slot {
  byte addr;
  byte id; // script id (for color)
  byte red;
  byte grn;
  byte blu;
  char mode;   
  byte script;
  byte reps;
  byte time;
  byte fade;
} wine_slot;

wine_slot rack[7];

char cmdChar;
int i;

void setup()
{
  Serial.begin(19200);
  
  // démarrer les leds
  BlinkM_begin();
  
  // tout arrêter les scripts en cours
  BlinkM_stopScript(0x00);
  
  // initialiser le rack (adresses I2C)
  rack[0].addr = 0x01;
  rack[1].addr = 0x02;
  rack[2].addr = 0x03;
  rack[3].addr = 0x04;
  rack[4].addr = 0x05;
  rack[5].addr = 0x06;
  rack[6].addr = 0x07;  
  
  // initialiser le rack (couleur)
  rack[0].red = 0x00;
  rack[0].grn = 0x00;  
  rack[0].blu = 0xFF;  
  
  rack[1].red = 0xFF;
  rack[1].grn = 0x00;  
  rack[1].blu = 0x00;  
  
  rack[2].red = 0x00;
  rack[2].grn = 0x00;  
  rack[2].blu = 0xFF;  
  
  rack[3].red = 0xFF;
  rack[3].grn = 0x00;  
  rack[3].blu = 0x00;  
  
  rack[4].red = 0xFF;
  rack[4].grn = 0x00;  
  rack[4].blu = 0x00;  
  
  rack[5].red = 0x00;
  rack[5].grn = 0xFF;  
  rack[5].blu = 0x00;  
  
  rack[6].red = 0xFF;
  rack[6].grn = 0xFF;  
  rack[6].blu = 0x00;  
  
  // gérer les scripts
  rack[0].script = 5;
  rack[1].script = 3;
  rack[2].script = 5;
  rack[3].script = 3;
  rack[4].script = 3;
  rack[5].script = 4;
  rack[6].script = 8;
  
  rack[0].reps = 0;
  rack[1].reps = 0;
  rack[2].reps = 0;
  rack[3].reps = 0;
  rack[4].reps = 0;
  rack[5].reps = 0;
  rack[6].reps = 0;
  
  rack[0].fade = 30;
  rack[1].fade = 15;
  rack[2].fade = 1;
  rack[3].fade = 1;
  rack[4].fade = 250;
  rack[5].fade = 100;
  rack[6].fade = 1;
  
  rack[0].time = 0;
  rack[1].time = 0;
  rack[2].time = -30;
  rack[3].time = -30;
  rack[4].time = 0;
  rack[5].time = 10;
  rack[6].time = -30;
  

  for (i=0; i<7; i++)
  {
    rack[i].mode = 'E';    // éteindre le rack
    BlinkM_setFadeSpeed(rack[i].addr,100);
    BlinkM_setTimeAdj(rack[i].addr,0);
  }
   
  Serial.println("Ready");
}

void loop()
{
  int i;
  // gérer les leds
  for (i=0; i<7; i++)
  {
    switch (rack[i].mode)
    {
      case 'E':  // éteint
        BlinkM_setFadeSpeed(rack[i].addr,100);
        BlinkM_setTimeAdj(rack[i].addr,0);
        BlinkM_fadeToRGB(rack[i].addr,0,0,0);
        break;
      case 'A':  // allumé
        BlinkM_setFadeSpeed(rack[i].addr,100);
        BlinkM_setTimeAdj(rack[i].addr,0);
        BlinkM_fadeToRGB(rack[i].addr,rack[i].red,rack[i].grn,rack[i].blu);
        break;
      case 'S':  // script
        BlinkM_setFadeSpeed(rack[i].addr,rack[i].fade);
        BlinkM_setTimeAdj(rack[i].addr,rack[i].time);
        BlinkM_playScript(rack[i].addr,rack[i].script,rack[i].reps,0);
        rack[i].mode = 'R';
        break;
      case 'R':
        // ne rien faire (script en cours)
      default:   // 
        // ne rien faire
        break;
    }    
  }  
  // changer les inputs
  while (Serial.available())
  {
    cmdChar = Serial.read();   
    switch (cmdChar)
    {
      case '0':  // tout éteint
        for (i=0; i<7; i++)
        {
          rack[i].mode = 'E';
        }
        break;
      case '1':  // tout allumé
        for (i=0; i<7; i++)
        {
          rack[i].mode = 'A';
        }
        break;       
      case '2':  // juste les bleus
        rack[0].mode = 'A';
        rack[1].mode = 'E';
        rack[2].mode = 'A';
        rack[3].mode = 'E';
        rack[4].mode = 'E';
        rack[5].mode = 'E';
        rack[6].mode = 'E';
        break;       
      case '3':  // juste les rouges
        rack[0].mode = 'E';
        rack[1].mode = 'A';
        rack[2].mode = 'E';
        rack[3].mode = 'A';
        rack[4].mode = 'A';
        rack[5].mode = 'E';
        rack[6].mode = 'E';
        break;
      case '4':  // un bleu, un rouge, un vert, un jaune
        rack[0].mode = 'A';
        rack[1].mode = 'E';
        rack[2].mode = 'E';
        rack[3].mode = 'A';
        rack[4].mode = 'E';
        rack[5].mode = 'A';
        rack[6].mode = 'A';
        break;
      case '5':  // les scripts sur deux rouges et un vert
        rack[0].mode = 'E';
        rack[1].mode = 'S';
        rack[2].mode = 'E';
        rack[3].mode = 'E';
        rack[4].mode = 'S';
        rack[5].mode = 'S';
        rack[6].mode = 'S';
        break;
      case '6':  // tous les scripts
        rack[0].mode = 'S';
        rack[1].mode = 'S';
        rack[2].mode = 'S';
        rack[3].mode = 'S';
        rack[4].mode = 'S';
        rack[5].mode = 'S';
        rack[6].mode = 'S';
        break;
    }
  }
  //  
}
