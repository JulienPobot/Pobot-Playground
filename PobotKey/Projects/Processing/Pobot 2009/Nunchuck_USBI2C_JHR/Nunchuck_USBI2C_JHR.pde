/**
 * http://www.wiili.org/index.php/Wiimote/Extension_Controllers/Nunchuk
 * http://www.pobot.org/Convertisseur-USB-I2C.html
 * http://www.robot-electronics.co.uk/htm/usb_i2c_tech.htm
 */
import processing.serial.*;

Serial port;  // Create object from Serial class

// I2C nunchuck address
// 0x52 in 7-bits
// 0xA4 in 8-bits 
byte address = (byte) 0xA4; 

void setup() 
{
  // display available serial ports on this computer
  println(Serial.list());
  // parameters for serial communication with Devantech device
  port = new Serial(this, Serial.list()[2], 19200, 'N', 8, 2.0);

  // initialization of the nunchuck
  nunchuck_init();
}


void nunchuck_init() {  
  byte[] msg1 = {
    0x55,address,0x40,0x01,0x00}; 
  port.write(msg1);  
}

void nunchuck_test() {
  byte[] msg = {
    0x53,0x00,0x00}; 
  port.write(msg);
}

void keyPressed() {
  switch (key) {
  case 'i':
    nunchuck_init();
    break;
  case 't':
    nunchuck_test();
    break;
  default: 
    println("Unrecognized command.");

  } 

}


void serialEvent(Serial p) { 
  // keep the received value
  int val = port.read();
  println(val);  
}

void draw()
{
  delay(200);
}




