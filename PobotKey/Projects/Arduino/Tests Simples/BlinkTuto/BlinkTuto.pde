/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
 int gndpin=2;
 int pwrpin=3;

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  
    DDRC |= _BV(pwrpin) | _BV(gndpin);  // make outputs
  PORTC &=~ _BV(gndpin);
  PORTC |=  _BV(pwrpin);

}

void loop() {
  digitalWrite(A5, HIGH);   // set the LED on
  delay(2000);              // wait for a second
  digitalWrite(A5, LOW);    // set the LED off
  delay(500);              // wait for a second
}
