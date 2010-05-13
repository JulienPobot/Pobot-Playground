import processing.core.*; import processing.serial.*; import java.applet.*; import java.awt.*; import java.awt.image.*; import java.awt.event.*; import java.io.*; import java.net.*; import java.text.*; import java.util.*; import java.util.zip.*; import javax.sound.midi.*; import javax.sound.midi.spi.*; import javax.sound.sampled.*; import javax.sound.sampled.spi.*; import java.util.regex.*; import javax.xml.parsers.*; import javax.xml.transform.*; import javax.xml.transform.dom.*; import javax.xml.transform.sax.*; import javax.xml.transform.stream.*; import org.xml.sax.*; import org.xml.sax.ext.*; import org.xml.sax.helpers.*; public class Radar_JHR extends PApplet {

// USER DEFINED size of the scanner (width of the window)
int scannerSize = 400;
int scannerLineWidth = 4; // 1/100\u00b0 of size for example
int scannerDotRadius = 2; // 1/200\u00b0 of size for example

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
//Serial port;   

/**
 *
 */
public void setup()
{  
  size(scannerSize,scannerSize/2);
  smooth();
  strokeWeight(scannerLineWidth);
 
  // List all the available serial ports:
  //println(Serial.list()); 
  //port = new Serial(this, Serial.list()[1], 38400); 
  // serial Arduino is on the second port (USB, not PC com1)

}

// Convert 10 bits analog value into value on the scanner radius
public int getDistance(int val) {
  return (int)(val / 1023.0f * radius) - 1;
}

// Serial reception
/*
void receiveDistance()
{
  int val = 0;
  while (port.available() >= 3) {
    if (port.read() == 0xff) {
      val = (port.read() << 8) | (port.read());
    }
  }    
  detection[theta] = getDistance(val);
}
*/

/**
 *
 */
public void draw() {
  //  
  fill(0, 10);
  rect(0, 0, width, height);
  // origine au centre de la fen\u00eatre + 10 pixels du bord
  translate(width/2,10);
  // the center of the scanner
  fill(200); //
  ellipse(0,0,10,10);

  // Convert polar to cartesian
  float x = radius * cos(radians(theta));
  float y = radius * sin(radians(theta));

  // Affichage de la ligne de d\u00e9tection courante
  stroke(0,200,0);
  line(0,0,x,y);

  // R\u00e9ception sur liaison s\u00e9rie
  //receiveDistance();

    // Affichage des distances re\u00e7ues
  noStroke();
  for (int i=0; i<180; i++)
  {    
    if (detection[i] != -1 && detection[i] != 0)
    {
      fill(calculeIntense(i));
      float dx = detection[i]*cos(radians(i));
      float dy = detection[i]*sin(radians(i));   
      ellipse(dx,dy,scannerDotRadius,scannerDotRadius);
    }
  }

  // mise \u00e0 jour de l'angle
  if (sens == 1) 
  {  
    theta++;
    if (theta == 180)
    {
      sens = 0;
    }
  } 
  else {
    theta--;
    if (theta == 0)
    {
      sens = 1;
    }
  }

}

public int calculeIntense(int i)
{  
  if (sens == 1)
  {  
    //
    if (i < theta+20 && i > theta - 60)
    {
      return floor((60 - theta + i)/60.0f*255.0f);      
    }
  } 
  else {
    //
    if (i > theta-20 && i < theta + 60)
    {
      return floor((60 - i + theta)/60.0f*255.0f);
    }     
  } 
  return 0;
}

  static public void main(String args[]) {     PApplet.main(new String[] { "Radar_JHR" });  }}