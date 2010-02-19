import processing.serial.*;


Serial port;

void setup()
{
  println(Serial.list());
  port = new Serial(this,Serial.list()[2],9600);
}

void keyPressed()
{
  println("Send command");

  port.write(0x7E);
  // packet type
  port.write(0x17);
  // frame id
  port.write(0x00);
  // 64 bit address
  port.write(0x00);
  port.write(0x00);
  port.write(0x00);
  port.write(0x00);
  port.write(0x00);
  port.write(0x00);
  port.write(0x00);
  port.write(0x00);
  // 16 bit addres
  port.write(0x56);
  port.write(0x78);
  port.write('D');
  port.write('1');
  port.write('4'); 

}

void draw()
{
  while (port.available() != 0) {
    String val = hex(port.readChar()); 
    if (val.equals("007E")) {
      println();  
    }    
    print(val);
    print(" ");
  } 
}







