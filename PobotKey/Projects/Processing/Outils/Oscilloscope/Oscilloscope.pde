
import processing.serial.*;

Serial port;  // Create object from Serial class
int val;      // Data received from the serial port
int maxVal = 0;
int[] values;

void setup() 
{
  size(640, 480);
  // Open the port that the board is connected to and use the same speed (9600 bps)
  println(Serial.list());
  port = new Serial(this, Serial.list()[1], 9600);
  values = new int[width];
  smooth();  
}

int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}

void draw()
{
  while (port.available() >= 3) {
    if (port.read() == 0xff) {
      val = (port.read() << 8) | (port.read());
      println("> "+val);
    }
  }
  for (int i=0; i<width-1; i++)
    values[i] = values[i+1];
  values[width-1] = val;
  background(255);
  stroke(0);
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(values[x-1]), 
         width-1-x, height-1-getY(values[x]));
  }
}

/*
// The Arduino code.

#define ANALOG_IN 0

void setup() {
  Serial.begin(9600); 
}

void loop() {
  int val = analogRead(ANALOG_IN);
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);
}

*/

