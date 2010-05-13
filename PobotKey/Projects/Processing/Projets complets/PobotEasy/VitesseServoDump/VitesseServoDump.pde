import processing.serial.*;

PrintWriter output;
Serial port;

int count;

String trame;


void setup()
{
  count = 0;
  
  port = new Serial(this,Serial.list()[1],9600);
  port.bufferUntil(10);
  
  output = createWriter("vitesses.csv"); 

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
  print(trame);
  count++;
}

void draw()
{
}





