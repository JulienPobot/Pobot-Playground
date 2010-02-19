import processing.serial.*;
import eeml.*;

Serial port;
DataOut dOut;

float lastUpdate;

void setup()
{
  //  
  println(Serial.list());
  port = new Serial(this,Serial.list()[2],9600);
  // set up DataOut object; requires URL of the EEML you are updating, and your Pachube API key   
  dOut = new DataOut(this, "http://www.pachube.com/api/1267.xml", "6bdd53f70381ad2c09b99b8c9fdded79e73f06f6127091daea715be2ae686f62");   
  dOut.addData(0,"temperature");
  //dOut.setUnits(0,"degree Celsius","°C","derivedSI");
  //
  lastUpdate = millis()-63000;
}


void draw()
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
      println("> Pachube update OK with value "+temperature+" °C.");
    } else {
      println("> Error with Pachube."); 
    }
    lastUpdate = millis();
  }   
}  

int readCurrentTemperature()
{  
  port.write("t");  
  delay(200);
  if (port.available()>0) 
  {
    return port.read();
  }
  return 0;
}





