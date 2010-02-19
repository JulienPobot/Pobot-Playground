
// Projet d'expérimentation sur une souris optique utilisant un capteur ADNS2610
// - récupération des coordonnnées x/y pour la localisation directe d'un robot mobile
// - affichage de l'image pour tests de calibration et de précision

// Experimental project using an optical mouse (Agilent sensor)
// - get x & y coordinates directly for localization of a mobile robot
// - capture the raw image from the sensor for calibration tests and measuring how sharp the sensor may be

// Written by Julien Holtzer (Association Pobot) in september 2009 -> contact [at] pobot.org
// Based on a project by Martijn The -> post [at] martijnthe.nl
// Tutorial: http://www.martijnthe.nl/optimouse/
// Based on the sketches by Benoît Rousseau (french Arduino enthusiast) on the Arduino.cc forum

// Martijn & Benoît ADNS code
#include "ADNS2610.h"

#define SCLK 3  // Serial clock pin on the Arduino (any digital except 0 and 1 as used for serial com)
#define SDIO 2  // Serial data (I/O) pin on the Arduino (any digital except 0 and 1 as used for serial com)

// Martijn wrote code for the sensor as a C++ object for easy access.
ADNS2610 sensor = ADNS2610(SCLK, SDIO);

// index for reading all pixels
int i;

signed char pix;

/**
 * Initialization
 */
void setup()
{
  //
  Serial.begin(19200);  
  //
  sensor.begin();

}

void loop()
{

  if (Serial.available())
  {
    switch (Serial.read())
    {
    case 'X':
      //
      Serial.print(sensor.dx());
      break;
    case 'Y':
      //
      Serial.print(sensor.dy());
      break;
    case 'P': 
      // sensor always awake

        // init the register for frame

        // start loop over every pixels
      for (i=0;i++;i<325)
      {
        pix = sensor.pixel();   
        Serial.print(pix);
      }

      break;
    case 'S':
      Serial.print('c');
      break;

    default:
      Serial.print('!');
    }
  }
}

