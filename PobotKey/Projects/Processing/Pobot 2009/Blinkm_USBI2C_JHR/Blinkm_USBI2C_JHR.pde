/**
 * 
 */
import processing.serial.*;

Serial port;  // Create object from Serial class

void setup() 
{
  println(Serial.list());
  port = new Serial(this, Serial.list()[1], 19200, 'N', 8, 2.0);
}

void draw()
{
  // la commande 'g'
  byte[] msg = {
    0x53,0x12,0x01,0x63}; 
  port.write(msg);

  while(port.available()>0)
  {
    println(port.read());
  }
  
  delay(200);
}
