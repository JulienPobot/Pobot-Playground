import processing.serial.*;


Serial port;

void setup()
{
  println(Serial.list());
  port = new Serial(this,Serial.list()[2],9600);
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






