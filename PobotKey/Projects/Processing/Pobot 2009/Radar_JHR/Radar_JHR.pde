import processing.serial.*;

// USER DEFINED size of the scanner (width of the window)
int scannerSize = 400;
int scannerLineWidth = 4; // 1/100? of size for example
int scannerDotRadius = 2; // 1/200? of size for example

// Computed radius (dependant of the size)
int radius = scannerSize / 2 - 20;

/** Variables **/

int theta = 0; // current value of the detection angle
int sens = 1;  // current value of the scanner direction 

// Store last detection values
int[] detection = new int[181];

// Store previous detection values
int[] previousdet = new int[181];

// The serial port:
Serial port;   

/**
 *
 */
void setup()
{  
  size(scannerSize,scannerSize/2);
  smooth();
  strokeWeight(scannerLineWidth);

  // List all the available serial ports:
  println(Serial.list()); 
  port = new Serial(this, Serial.list()[2], 38400); 
  // serial Arduino is on the second port (USB, not PC com1)

}

// Convert 10 bits analog value into value on the scanner radius
int getDistance(int val) {
  return (int)(val / 1023.0f * radius) - 1;
}

// Serial reception
void receiveDistance()
{
  int value = 0;
  int angle = 0;
  while (port.available() >= 5) {
    if (port.read() == 0xff) {
      angle = (port.read() << 8) | (port.read());       
      sens = (port.read() == 0x00?0:1);
      value = (port.read() << 8) | (port.read());
      detection[angle] = getDistance(1024-value);
      theta = angle;
      //println("> "+angle+": "+value);
    }    
    port.clear();
  }    
}

/**
 *
 */
void draw() {
  //  
  fill(0, 10);
  rect(0, 0, width, height);
  // origine au centre de la fen?tre + 10 pixels du bord
  translate(width/2,10);
  // the center of the scanner
  fill(200); //
  ellipse(0,0,10,10);

  // Convert polar to cartesian
  float x = radius * cos(radians(theta));
  float y = radius * sin(radians(theta));

  // Affichage de la ligne de d?tection courante
  stroke(0,200,0);
  line(0,0,x,y);

  // R?ception sur liaison s?rie
  receiveDistance();

  // Affichage des distances re?ues
  noStroke();
  for (int i=0; i<180; i++)
  {    
    if (detection[i] != -1 && detection[i] != 0)
    {
      //fill(calculeIntense(i));
      fill(255);
      float dx = detection[i]*cos(radians(i));
      float dy = detection[i]*sin(radians(i));   
      ellipse(dx,dy,scannerDotRadius,scannerDotRadius);
    }
  }

}

int calculeIntense(int i)
{  
  if (sens == 1)
  {  
    //
    if (i < theta+20 && i > theta - 60)
    {
      return floor((60 - theta + i)/60.0*255.0);      
    }
  } 
  else {
    //
    if (i > theta-20 && i < theta + 60)
    {
      return floor((60 - i + theta)/60.0*255.0);
    }     
  } 
  return 0;
}




