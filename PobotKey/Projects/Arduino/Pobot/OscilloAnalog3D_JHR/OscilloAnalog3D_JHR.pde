// The Arduino code.


void setup() {
  Serial.begin(115200); 
  digitalWrite(14,LOW); 
}

void loop() {
  int valX = analogRead(0);
  int valY = analogRead(1);
  int valZ = analogRead(2);
  Serial.print(valX & 0xFFFF,DEC);
  Serial.print(" ");
  Serial.print(valY & 0xFFFF,DEC);
  Serial.print(" ");
  Serial.print(valZ & 0xFFFF,DEC);
  Serial.println();
  delay(2);
}
