import processing.serial.*;

Serial port;

int index = 0;

color c;

void setup()
{
  size(160,120);    
  //
  port = new Serial(this, Serial.list()[1], 115200); 
  // ne démarrer le traitement d'une trame 
  // qu'après réception du caractère fin de ligne (10 = \n)
  port.bufferUntil(10);
}

void keyPressed()
{
  loadPixels();
  // déclencher la photo
  port.write("p");
  index = 0;  
}

void serialEvent(Serial p)
{
  if (index < width*height)
  {
    int r = port.readChar();
    int g = port.readChar();
    int b = port.readChar();

    if (r < 256 && g < 256 && b < 256)
    {
      c = color(r, g, b);    
    }    
    int linefeed = port.readChar();

    pixels[index++] = c;
    updatePixels(); 
  } 
  else {
    println("OK");
  }    
}

void draw()
{  

}





