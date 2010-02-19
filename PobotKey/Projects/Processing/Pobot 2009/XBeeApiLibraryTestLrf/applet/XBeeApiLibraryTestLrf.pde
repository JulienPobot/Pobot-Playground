import processing.serial.*;

import xbee.XBeeDataFrame;
import xbee.XBeeReader;

Serial port;
XBeeReader xbee;

void setup()
{
  println(Serial.list());
  port = new Serial(this,Serial.list()[2],9600);
  xbee = new XBeeReader(this,port);
  xbee.startXBee();
}

void xBeeEvent(XBeeReader xbee) {
  XBeeDataFrame data = xbee.getXBeeReading();
  
  if (data.getApiID() == xbee.SERIES1_IOPACKET) {    
    int analogVal = data.frameData[8] * 256 + data.frameData[9];    
    //println("A: "+analogVal);    
  } 
  else {
    println("Non supporté");
  }
}

void keyPressed()
{
  println("Send command");
  
  xbee.sendRemoteCommand(0,0,0x5678,"D1",5);
  delay(1000);
  xbee.sendRemoteCommand(0,0,0x5678,"D1",4);
  delay(1000);
}


void draw() { }
