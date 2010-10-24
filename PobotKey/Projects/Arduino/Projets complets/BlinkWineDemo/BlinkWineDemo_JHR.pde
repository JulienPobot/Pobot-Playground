
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
} wine_slot;

wine_slot rack[6];

int cmdChar;

void setup()
{
  // démarrer les leds
  BlinkM_begin();
  
  // initialiser le rack (adresses I2C)
  rack[0].addr = 0x01;
  rack[1].addr = 0x02;
  rack[2].addr = 0x03;
  rack[3].addr = 0x04;
  rack[4].addr = 0x05;
  rack[5].addr = 0x06;  
  
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
  
  // gérer les scripts
  rack[0].script = 5;
  rack[1].script = 3;
  rack[2].script = 5;
  rack[3].script = 3;
  rack[4].script = 3;
  rack[5].script = 4;
  
  rack[0].reps = 5;
  rack[1].reps = 1;
  rack[2].reps = 2;
  rack[3].reps = 4;
  rack[4].reps = 5;
  rack[5].reps = 3;
  
  
  // éteindre le rack
  rack[0].mode = 'E';
  rack[1].mode = 'E';
  rack[2].mode = 'E';
  rack[3].mode = 'E';
  rack[4].mode = 'E';
  rack[5].mode = 'E';   
   
}

void loop()
{
  int i;
  // gérer les leds
  for (i=0; i<6; i++)
  {
    switch (rack[i].mode)
    {
      case 'E':  // éteint
        BlinkM_fadeToRGB(rack[i].addr,0,0,0);
        break;
      case 'A':  // allumé
        BlinkM_fadeToRGB(rack[i].addr,rack[i].red,rack[i].grn,rack[i].blu);
        break;
      case 'S':  // script
        BlinkM_playScript(rack[i].addr,rack[i].script,rack[i].reps,0);
        break;
      default:   // comme le A
        BlinkM_fadeToRGB(rack[i].addr,rack[i].red,rack[i].grn,rack[i].blu);
        break;
    }    
  }  
  // changer les inputs
  while (serialAvailable())
  {
    cmdChar = serialRead ();
    printByte (cmdChar);        
    switch (cmdChar)
    {
      case '0':  // tout éteint
        for (i=0; i<6; i++)
        {
          rack[i].mode = 'E';
        }
        break;
      case '1':  // tout allumé
        for (i=0; i<6; i++)
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
        break;       
      case '3':  // juste les rouges
        rack[0].mode = 'E';
        rack[1].mode = 'A';
        rack[2].mode = 'E';
        rack[3].mode = 'A';
        rack[4].mode = 'A';
        rack[5].mode = 'E';
        break;
      case '4':  // un bleu, un rouge et un vert
        rack[0].mode = 'A';
        rack[1].mode = 'E';
        rack[2].mode = 'E';
        rack[3].mode = 'A';
        rack[4].mode = 'E';
        rack[5].mode = 'A';
        break;
      case '5':  // les scripts sur deux rouges et un vert
        rack[0].mode = 'E';
        rack[1].mode = 'S';
        rack[2].mode = 'E';
        rack[3].mode = 'E';
        rack[4].mode = 'S';
        rack[5].mode = 'S';
        break;
      case '6':  // tous les scripts
        rack[0].mode = 'S';
        rack[1].mode = 'S';
        rack[2].mode = 'S';
        rack[3].mode = 'S';
        rack[4].mode = 'S';
        rack[5].mode = 'S';
        break;
    }
  }
  //  
}
