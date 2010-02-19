import processing.core.*; 
import processing.xml.*; 

import processing.serial.*; 

import java.applet.*; 
import java.awt.*; 
import java.awt.image.*; 
import java.awt.event.*; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class Oscilloscope3D extends PApplet {



Serial serial;
int serialPort = 1;

int sen = 3; // sensors

int[] valuesX;
int[] valuesY;
int[] valuesZ;

String trame;
String[] parts;

public void setup() 
{
  size(640, 480);
  frameRate(25);

  // Open the port that the board is connected to and use the same speed (9600 bps)

  println(Serial.list());
  serial = new Serial(this, Serial.list()[serialPort], 115200); 
  serial.bufferUntil(10); // line feed

  valuesX = new int[width];
  valuesY = new int[width];
  valuesZ = new int[width];
  smooth();
}

public int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}

/**
 * R\u00e9ception d'une trame
 */
public void serialEvent(Serial p) {
  // traiter la chaine de caract\u00e8res
  trame = serial.readString();
  // v\u00e9rifier la longueur
  if (trame.length() < 12) {
    print("e");
    return;
  }   
  parts = split(trame, " ");
  // d\u00e9caler les valeurs pr\u00e9c\u00e9dentes  
  for (int i=0; i<width-1; i++) {
    valuesX[i] = valuesX[i+1];
    valuesY[i] = valuesY[i+1];
    valuesZ[i] = valuesZ[i+1];    
  }

  // enregistrer les nouvelles valeurs filtr\u00e9es
  valuesX[width-1] = PApplet.parseInt(PApplet.parseFloat(parts[0]));
  valuesY[width-1] = PApplet.parseInt(PApplet.parseFloat(parts[1]));
  valuesZ[width-1] = PApplet.parseInt(PApplet.parseFloat(parts[2]));
  
}

public void draw()
{ 
  // d\u00e9clencher l'affichage
  updateOscillo();
  
}

public void updateOscillo() {
  background(250);
  for (int x=1; x<width; x++) {
    stroke(150,0,0);
    line(width-x,   height-1-getY(valuesX[x-1]), 
    width-1-x, height-1-getY(valuesX[x]));
    //
    stroke(0,150,0);
    line(width-x,   height-1-getY(valuesY[x-1]), 
    width-1-x, height-1-getY(valuesY[x]));
    //    
    stroke(0,0,150);
    line(width-x,   height-1-getY(valuesZ[x-1]), 
    width-1-x, height-1-getY(valuesZ[x]));
  }
}


  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#d4d0c8", "Oscilloscope3D" });
  }
}
