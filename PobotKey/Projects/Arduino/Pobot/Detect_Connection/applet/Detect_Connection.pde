
void setup()
{
  Serial.begin(115200);   
  pinMode(14,INPUT); 
  digitalWrite(14,HIGH);  
}

void loop()
{  
  Serial.print(analogRead(0));
  Serial.print(" ");
  Serial.print(isConnected()?500:0);
  Serial.print(" ");
  Serial.print(0);
  Serial.print(" ");
  Serial.println();
  delay(50);  
}

boolean isConnected(void) 
{
  int count = 0;
  // mettre à zéro
  pinMode(14,OUTPUT);
  digitalWrite(14,LOW);
  pinMode(14,INPUT);  
  digitalWrite(14,HIGH);
  while (digitalRead(14) != HIGH) {
    count++;
    if (count > 10) return true;
  }
  return analogRead(0) != 1023 || count != 0;
}
