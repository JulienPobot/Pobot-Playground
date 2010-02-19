
#include "WiiChuck.h"

void WiiChuck::begin() 
{
  // alimentation du nunchuck
  DDRC |= _BV(PC3) | _BV(PC2);  // pattes Analog 2 et 3 en sortie
  PORTC &=~ _BV(PC2);           // patte Analog 2 à 0v
  PORTC |=  _BV(PC3);           // patte Analog 3 à 5v
  delay(100);  // une petite attente
  Wire.begin();
  cnt = 0;
  averageCounter = 0;
  Wire.beginTransmission (0x52);	// transmit to device 0x52
  Wire.send (0x40);		// sends memory address
  Wire.send (0x00);		// sends memory address
  Wire.endTransmission ();	// stop transmitting
  update();            
  for (i = 0; i<3;i++) {
    angles[i] = 0;
  }
  zeroJoyX = DEFAULT_ZERO_JOY_X;
  zeroJoyY = DEFAULT_ZERO_JOY_Y;
}


void WiiChuck::calibrateJoy() {
  zeroJoyX = joyX;
  zeroJoyY = joyY;
}

void WiiChuck::update() {

  Wire.requestFrom (0x52, 6);	// request data from nunchuck
  while (Wire.available ()) {
    // receive byte as an integer
    status[cnt] = _nunchuk_decode_byte (Wire.receive()); //
    cnt++;
  }
  if (cnt > 5) {
    lastZ = buttonZ;
    lastC = buttonC;
    lastJoyX = readJoyX();
    lastJoyY = readJoyY();
    //averageCounter ++;
    //if (averageCounter >= AVERAGE_N)
    //    averageCounter = 0;

    cnt = 0;
    joyX = (status[0]);
    joyY = (status[1]);
    for (i = 0; i < 3; i++) 
      //accelArray[i][averageCounter] = ((int)status[i+2] << 2) + ((status[5] & (B00000011 << ((i+1)*2) ) >> ((i+1)*2))); 
      angles[i] = (status[i+2] << 2) + ((status[5] & (B00000011 << ((i+1)*2) ) >> ((i+1)*2))); 

    //accelYArray[averageCounter] = ((int)status[3] << 2) + ((status[5] & B00110000) >> 4); 
    //accelZArray[averageCounter] = ((int)status[4] << 2) + ((status[5] & B11000000) >> 6); 

    buttonZ = !( status[5] & B00000001);
    buttonC = !((status[5] & B00000010) >> 1);
    _send_zero(); // send the request for next bytes

  }
}


// UNCOMMENT FOR DEBUGGING
//byte * getStatus() {
//    return status;
//}

float WiiChuck::readAccelX() {
  // total = 0; // accelArray[xyz][averageCounter] * FAST_WEIGHT;
  return (float)angles[0] - ZEROX;
}
float WiiChuck::readAccelY() {
  // total = 0; // accelArray[xyz][averageCounter] * FAST_WEIGHT;
  return (float)angles[1] - ZEROY;
}
float WiiChuck::readAccelZ() {
  // total = 0; // accelArray[xyz][averageCounter] * FAST_WEIGHT;
  return (float)angles[2] - ZEROZ;
}

boolean WiiChuck::zPressed() {
  return (buttonZ && ! lastZ);
}
boolean WiiChuck::cPressed() {
  return (buttonC && ! lastC);
}

// for using the joystick like a directional button
boolean WiiChuck::rightJoy(int thresh=60) {
  return (readJoyX() > thresh and lastJoyX <= thresh);
}

// for using the joystick like a directional button
boolean WiiChuck::leftJoy(int thresh=60) {
  return (readJoyX() < -thresh and lastJoyX >= -thresh);
}


int WiiChuck::readJoyX() {
  return (int) joyX - zeroJoyX;
}

int WiiChuck::readJoyY() {
  return (int)joyY - zeroJoyY;
}


// R, the radius, generally hovers around 210 (at least it does with mine)
// int R() {
//     return sqrt(readAccelX() * readAccelX() +readAccelY() * readAccelY() + readAccelZ() * readAccelZ());  
// }


// returns roll degrees
int WiiChuck::readRoll() {
  return (int)(atan2(readAccelX(),readAccelZ())/ M_PI * 180.0);
}

// returns pitch in degrees
int WiiChuck::readPitch() {        
  return (int) (acos(readAccelY()/RADIUS)/ M_PI * 180.0);  // optionally swap 'RADIUS' for 'R()'
}

