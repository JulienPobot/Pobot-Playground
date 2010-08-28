#define IMG_WHITE   0
#define IMG_BLACK   1
#define IMG_DAMIER  2
#define IMG_DAMIER2 3
#define IMG_HALF    4
#define IMG_HALF2   5

#define SCREEN_G   0x11
#define SCREEN_D   0x22
#define SCREEN_ALL 0x0f

#include <avr/pgmspace.h>

/**
 * base de données d'images
 * - chaque ligne est une image
 * - chaque caractère est la ?? colonne ou ligne (à vérifier)
 * - on accède avec un index
 */
const uint8_t dbimage[][8] PROGMEM = 
  { {0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff},
    {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00},
    {0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA},
    {0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55},
    {0xF0,0xF0,0xF0,0xF0, 0xF0,0xF0,0xF0,0xF0},
    {0x0F,0x0F,0x0F,0x0F, 0x0F,0x0F,0x0F,0x0F},
    {0xFF,0xFF,0xFF,0xFF, 0xF9,0x0F9,0x0F9,0xFF}
  };
  
/** 
 * base de données représentant une vidéo
 * - chaque élément est composé d'un doublet
 * - le doublet désigne les deux images (1 image par oeil)
 */
const uint16_t dbvideo[][2] PROGMEM = 
  {{0,0},{1,1},{2,2},{3,3},{4,4},{5,5}};

/* part of the film*/
struct clip_t {
  uint16_t start_index;
  uint8_t size;
};

const struct clip_t clip[] PROGMEM = {
  {0, 1},
  {1, 1},
  {2, 1},
  {3, 1},
  {4, 1},
  {5, 1},
  {6, 1},
  {0, 7},
  };

/*
  CMD -> clip -> dbvideo + CNT -> dbimage
*/
