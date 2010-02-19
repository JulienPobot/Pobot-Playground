#include "_init.h"

void setup()
{
  ds_init();

}


void loop()
{
  //take an analog reading
  raw = analogRead(TEMP_SENSOR_PIN);


  //only read temperature once a second.
  delay(1000);
}



//our init and bootup function
void ds_init()
{
  //buttons are inputs.
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  pinMode(BUTTON3_PIN, INPUT);
  
  //leds and buzzers are outputs.
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SLIDER1_LED_PIN, OUTPUT);
  pinMode(SLIDER2_LED_PIN, OUTPUT);
  pinMode(SLIDER3_LED_PIN, OUTPUT);

  //shift register pins are output.
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  //flash our LEDs
  digitalWrite(LED1_PIN, HIGH);
  digitalWrite(LED2_PIN, HIGH);
  digitalWrite(SLIDER1_LED_PIN, HIGH);
  digitalWrite(SLIDER2_LED_PIN, HIGH);
  digitalWrite(SLIDER3_LED_PIN, HIGH);

  //turn 7-segment on
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 255);
  digitalWrite(LATCH_PIN, HIGH);

  //buzz for a bit.
  for (int i=0; i<500; i++)
  {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(BUZZER_PIN, LOW);
  }

  //turn off LEDs
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(SLIDER1_LED_PIN, LOW);
  digitalWrite(SLIDER2_LED_PIN, LOW);
  digitalWrite(SLIDER3_LED_PIN, LOW);

  //turn 7-segment off
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  digitalWrite(LATCH_PIN, HIGH);

  //startup serial
  Serial.begin(9600);
  Serial.println("started");
}
