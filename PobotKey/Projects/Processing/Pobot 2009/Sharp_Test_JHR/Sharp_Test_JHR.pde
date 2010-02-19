import processing.serial.*;

Serial port;

float[] values;
float val;
float mean;

void setup() 
{
  size(800, 600);
  port = new Serial(this, Serial.list()[1], 38400);
  values = new float[width];
  smooth();
}

int getY(float val) {
  return (int)(val / 1023.0f * height) - 1;
}

void draw()
{
  while (port.available() >= 3) {
    if (port.read() == 0xff) {
      val = (port.read() << 8) | (port.read());     
    }
  } 
  
  // décalage du tableau
  for (int i=0; i<width-1; i++)
    values[i] = values[i+1];
  values[width-1] = val;
  
  // calcul de la moyenne
  float somme = 0;
  for (int i = width-50; i <width; i++)
  {
    somme += values[i];    
  }
  mean = somme / 50;
  println((int)mean);
  
  // affichage  
  background(0);
  stroke(255);
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(values[x-1]), 
         width-1-x, height-1-getY(values[x]));
  }
}
