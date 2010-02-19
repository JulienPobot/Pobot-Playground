void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop()
{
  while (Serial.available() != 0)
  {
    switch (Serial.read())
    {
    case 'H':
      digitalWrite(13,HIGH);
      break;
    case 'L':
      digitalWrite(13,LOW);
      break;
    }
  }
}
