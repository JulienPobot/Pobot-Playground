// The Arduino code.


void setup() {
  Serial.begin(38400); 
}

void loop() {
  int val = analogRead(0);
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);
}
