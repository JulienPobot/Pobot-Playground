import processing.serial.*;

Serial port;

int tps = 500;

String pattern[] = {
  "4555p",
  "3535p",
  "3415p",
  "2415p",  
  "2637p",
  "3748p",
  "5656p",
  "6666p",
  "7676p",
  "7473p",
  "6373p",
  "6353p",
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

}

void keyPressed() {

  if (keyCode == LEFT) 
  {
    index--;    
    if (index < 0) 
    {
      index = 0;
    } 
  }
  
  if (keyCode == RIGHT || key == ' ')
  {  
    index++;
    if (index > pattern.length-1) 
    {
      index = 0;
    } 
  }  

  if (keyCode == LEFT || keyCode == RIGHT || key == ' ')
  {
    port.write(pattern[index]);
  }
  
  if (key == ' ' || keyCode == LEFT || keyCode == RIGHT)
  {
    println("> "+pattern[index]);
  }
  
  if (key == ' ') {
    delay(tps);
  }

}




