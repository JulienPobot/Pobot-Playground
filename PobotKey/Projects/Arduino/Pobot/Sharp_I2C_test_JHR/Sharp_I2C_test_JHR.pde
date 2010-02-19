#include <Wire.h>

void setup() 
{  
  Serial.begin(9600);  // start serial for output
  Wire.begin(); // initialisation I2C
  //
  Wire.beginTransmission(2);
  Wire.send(0x41);
  Wire.send('2');
  Wire.endTransmission();
  //
  Wire.beginTransmission(2);
  Wire.send(0x41);
  Wire.send('E');
  Wire.endTransmission();
  //
} 


void loop()
{
  Wire.beginTransmission(0x02);
  Wire.send(0x53);
  Wire.endTransmission();
  Wire.requestFrom(0x02, 1);
  if( Wire.available() ) {
    byte b = Wire.receive();
    Serial.print(b); 
  }
  Serial.println();
  //
  delay(100);

}
