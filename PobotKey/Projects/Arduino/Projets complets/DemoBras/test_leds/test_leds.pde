long i = 0;

void setup() {

}

void loop() {
  if (i < 256) {
    analogWrite(2,255-i); // R
    analogWrite(5,i); // G
    analogWrite(6,0); // B
  } 
  else if (i < 512) {
    analogWrite(2,0);
    analogWrite(5,511-i);
    analogWrite(6,i-255);
  }
  else if (i < 768) {
    analogWrite(2,i-511);
    analogWrite(5,0);
    analogWrite(6,767-i); 
  }
  delay(10); 
  i++;
  if (i >= 768) {
    i = 0;
  } 
}

