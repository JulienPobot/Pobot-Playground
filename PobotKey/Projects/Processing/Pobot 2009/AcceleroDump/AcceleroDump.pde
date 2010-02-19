import processing.serial.*;

PrintWriter output;
Serial port;

int count;

String trame;

int[] valuesX;
int[] valuesY;

void setup()
{
  count = 0;
  
  port = new Serial(this,Serial.list()[1],115200);
  port.bufferUntil(10);
  
  output = createWriter("accelero.csv"); 

}

void mousePressed()
{
 port.write("a"); 
}

void keyPressed() 
{
  output.flush();
  output.close();
  println();
  println("Count: "+count);
  exit();
  
}

void serialEvent(Serial p) {
  trame = port.readString();
  output.print(trame);
  print(".");
  count++;
}

void draw()
{
  delay(500);
  println();
  println("Count: "+count);
}





