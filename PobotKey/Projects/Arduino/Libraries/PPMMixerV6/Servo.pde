void setupServo()
{
  TCCR0A = 0; 
  TIMSK0 |= 1 << OCIE0A; 
  TIMSK0 |= 1 << OCIE0B; 
  
  pinMode(8,OUTPUT);
  pinMode(12,OUTPUT);
}

//***********************************************************************************************************************


byte state_servoA,state_servoB;
byte countA,countB;
byte lastOCRA,lastOCRB;

// prescaler is set by default to 64 on Timer0 for arduino specific functions
// Duemilanove : 16000000 Hz / 64 => 4 microseconds
// 256 steps = 1 counter cycle = 1024 microseconds
// algorithm strategy:
// pulse high
// do nothing for 1000 microseconds
// do nothing for [0 to 1000] microseconds
// pulse down
// do nothing for 18 counter cycles
// ==> the total cycle lasts around 19-20ms => 50Hz, OK for a standard servo
ISR (TIMER0_COMPA_vect) { 
  uint8_t ocr;
  
  if (state_servoA == 0) {
    digitalWrite( 8,HIGH );
    ocr= 250; // 1000 micro seconds
    state_servoA = 1;
  } else if (state_servoA == 1) {
    ocr = lastOCRA ;  // (servo val - 1000) x 256 / 1000 (or approx (servo val – 1000) / 4 )
    state_servoA = 2;
  } else if (state_servoA == 2) {
    digitalWrite( 8,LOW );
    countA = 16; // 18 x 1020 microseconds
    state_servoA = 3;
    ocr = 255; // 1020 microseconds
  } else if (state_servoA == 3) {
    if (countA >0) {
      countA--;
    } else {
      state_servoA = 0;
      //count = 18; 
    }
    ocr = 255;
  }
  OCR0A+=ocr;
}

ISR (TIMER0_COMPB_vect) { 
  uint8_t ocr;
  
  if (state_servoB == 0) {
    digitalWrite( 12,HIGH );
    ocr= 250; // 1000 micro seconds
    state_servoB = 1;
  } else if (state_servoB == 1) {
    ocr = lastOCRB ;  
    state_servoB = 2;
  } else if (state_servoB == 2) {
    digitalWrite( 12,LOW );
    countB = 18; 
    state_servoB = 3;
    ocr = 250; // 1000 microseconds
  } else if (state_servoB == 3) {
    if (countB >0) {
      countB--;
    } else {
      state_servoB = 0;
      //count = 18; 
    }
    ocr = 250; // 1000 microseconds
  }
  OCR0B+=ocr;
}

void updateServo()
{
  int period;
  period=limit(-500,channelOut[4],500);
  lastOCRA=(CENTER+period-1000)*250.0/1000.0;
  
  period=limit(-500,channelOut[5],500);
  lastOCRB=(CENTER+period-1000)*250.0/1000.0;
}

