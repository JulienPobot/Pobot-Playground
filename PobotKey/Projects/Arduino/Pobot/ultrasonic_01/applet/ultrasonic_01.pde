
#include <Wire.h>

void setup()
{
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);          // start serial communication at 9600bps
}

void loop()
{
  
  // 1) read version

  Serial.print("STOP  ");
  Wire.beginTransmission(0x02);
  Wire.send(0x41);
  Wire.send(0x00); 
  Wire.endTransmission();
  
  // 2) attendre
  delay(4000);
  
  Serial.print("START  ");  
  Wire.beginTransmission(0x02);
  Wire.send(0x41);
  Wire.send(0x02);
  Wire.endTransmission();
  
  // 3) lire  
  delay(4000);
  
  /*
  Wire.beginTransmission(0x02);
  Wire.endTransmission();
  
  Wire.requestFrom(0x03,8);
  while (Wire.available() > 0)
  {
    Serial.print(Wire.receive(),HEX);
    Serial.print("-");
  }
  Serial.println();  

  */
}
