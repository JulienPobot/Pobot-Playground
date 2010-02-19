#include "_init.h"

#include "WProgram.h"
void setup();
void loop();
void ds_init();
void setup()
{
  ds_init();

}


//our variables for temp sensing.
int raw = 0;
float voltage = 0.0;
int celsius = 0;
int fahrenheit = 0;

void loop()
{
  //take an analog reading
  raw = analogRead(TEMP_SENSOR_PIN);

  //convert it to voltage.  analogRead returns a value of 0-1023 representing the voltage level between 0.0v and 5.0v
  voltage = (raw * 5.0) / 1024.0;

  //finally, convert voltage to temperature.  the LM35 outputs 10.0mV per degree Celsius.  For example 1000mV (1v) would be 100C.
  celsius = voltage * 100;

  //of course you can combine this into one compact statement
  celsius = (5.0 * analogRead(TEMP_SENSOR_PIN) * 100.0) / 1024.0;

  //how about fahrenheit?
  fahrenheit = (celsius * 1.8) + 32;

  //now send it to the output so you can see it.
  Serial.print("Raw: ");
  Serial.println(raw, DEC);
  Serial.print("Voltage: ");
  Serial.println(voltage, DEC);
  Serial.print("Celsius: ");
  Serial.println(celsius, DEC);
  Serial.print("Fahrenheit: ");
  Serial.println(fahrenheit, DEC);
  Serial.println(" ");

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

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

