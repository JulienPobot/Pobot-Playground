import processing.serial.*;

Serial port;

int tps = 150;

String pattern[] = {
  "4555",
  "4657"  
};

int index = 0;

void setup()
{
  // 
  println(Serial.list());
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600);
}


void draw()
{
  port.write(pattern[index]);
  index++;
  if (index > pattern.length) 
  {
    index = 0;
  }
}

