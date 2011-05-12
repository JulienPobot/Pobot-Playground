

int analogValue=0;



void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  

 Serial.begin(9600);


}

void loop() {
  

analogValue = analogRead(0);
  Serial.println(analogValue);
  
delay(1000);

  
  
}
