import processing.serial.*;

Serial port;

int tps = 150;

void setup()
{
  // 
  println(Serial.list());
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600);
}


void draw()
{
  move_un();  
}

void move_un()
{  
  port.write("4951"); // pause
  delay(tps);
  port.write("4551p");
  delay(tps);
  port.write("1551p");
  delay(tps);
  port.write("2961p");
  delay(tps);
  port.write("4951p"); // pause
  delay(tps);
  port.write("4955p");
  delay(tps);
  port.write("4984p");
  delay(tps);
  port.write("4971p"); 
  delay(tps);
  port.write("4951p");
  delay(tps);
}
