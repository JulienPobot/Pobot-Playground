import processing.core.*; 
import processing.xml.*; 

import processing.serial.*; 
import eeml.*; 

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

public class PachubeTemperatureUpdate extends PApplet {




Serial port;
DataOut dOut;

float lastUpdate;

public void setup()
{
  //  
  println(Serial.list());
  port = new Serial(this,Serial.list()[2],9600);
  // set up DataOut object; requires URL of the EEML you are updating, and your Pachube API key   
  dOut = new DataOut(this, "http://www.pachube.com/api/1267.xml", "6bdd53f70381ad2c09b99b8c9fdded79e73f06f6127091daea715be2ae686f62");   
  dOut.addData(0,"temperature");
  //dOut.setUnits(0,"degree Celsius","\u00b0C","derivedSI");
  //
  lastUpdate = millis()-63000;
}


public void draw()
{
  // update every minute
  if ((millis() - lastUpdate) > 60000){
    //
    int temperature = readCurrentTemperature();
    if (temperature > 60) {
       temperature = readCurrentTemperature(); 
    }
    dOut.update(0, temperature); 
    if (dOut.updatePachube() == 200) {
      println("> Pachube update OK with value "+temperature+" \u00b0C.");
    } else {
      println("> Error with Pachube."); 
    }
    lastUpdate = millis();
  }   
}  

public int readCurrentTemperature()
{  
  port.write("t");  
  delay(200);
  if (port.available()>0) 
  {
    return port.read();
  }
  return 0;
}






  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#d4d0c8", "PachubeTemperatureUpdate" });
  }
}
