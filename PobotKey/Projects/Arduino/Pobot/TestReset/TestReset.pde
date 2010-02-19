int increment = 0;

void soft_reset() 
{
  asm volatile ("  jmp 0");  
} 

void setup()
{
  Serial.begin(9600);
  increment = 10;
}

void loop()
{
  increment++;
  Serial.println(increment);
  delay(200);

  if (Serial.available() > 0) {
    if (Serial.read() == 'r')
    {
      soft_reset();
    }
  }
}
