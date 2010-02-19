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

public class EasyBipedeSecond extends PApplet {



Serial port;

int tps = 150;

String pattern[] = {
  "4555",
  "4657"  
};

int index = 0;

public void setup()
{
  // 
  println(Serial.list());
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600);
}


public void draw()
{
  port.write(pattern[index]);
  index++;
  if (index > pattern.length) 
  {
    index = 0;
  }
}


  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#D4D0C8", "EasyBipedeSecond" });
  }
}
