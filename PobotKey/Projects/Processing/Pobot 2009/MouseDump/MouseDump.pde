import processing.serial.*;

PrintWriter output;
Serial port;

int count;
String trame;

void setup()
{
  size(500, 500, P3D);
  noFill();
  stroke(204);
  //
  count = 0;

  port = new Serial(this,Serial.list()[0],115200);
  port.bufferUntil(10);

  output = createWriter("mouse.csv"); 

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






