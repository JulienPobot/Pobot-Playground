import processing.core.*; 
import processing.xml.*; 

import processing.serial.*; 
import xbee.XBeeDataFrame; 
import xbee.XBeeReader; 

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

public class XBeeApiLibraryTestLrf extends PApplet {






Serial port;
XBeeReader xbee;

public void setup()
{
  println(Serial.list());
  port = new Serial(this,Serial.list()[2],9600);
  xbee = new XBeeReader(this,port);
  xbee.startXBee();
}

public void xBeeEvent(XBeeReader xbee) {
  XBeeDataFrame data = xbee.getXBeeReading();
  
  if (data.getApiID() == xbee.SERIES1_IOPACKET) {    
    int analogVal = data.frameData[8] * 256 + data.frameData[9];    
    //println("A: "+analogVal);    
  } 
  else {
    println("Non support\u00e9");
  }
}

public void keyPressed()
{
  println("Send command");
  
  xbee.sendRemoteCommand(0,0,0x5678,"D1",5);
  delay(1000);
  xbee.sendRemoteCommand(0,0,0x5678,"D1",4);
  delay(1000);
}


public void draw() { }

  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#D4D0C8", "XBeeApiLibraryTestLrf" });
  }
}
