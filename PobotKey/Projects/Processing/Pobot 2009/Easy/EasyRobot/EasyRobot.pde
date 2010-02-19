/**
 * # Copyright (c) 2009 Julien Holtzer
 */

import processing.serial.*;
import processing.net.*;

Server serveurRobot;
Client clientConsignes;
Serial portRobot;

String mesString;
String[] mesToken;

int val1,val2;

void setup()
{  
  // Starts the server for return of robot on port 5201
  serveurRobot = new Server(this, 5202);

  // Connect the client that reads consignes (port 5204)
  clientConsignes = new Client(this, "127.0.0.1", 5204);

  //
  println(Serial.list());
  portRobot = new Serial(this, Serial.list()[1], 115200);
  portRobot.bufferUntil(10);
}

void draw()
{
  if (clientConsignes.available() > 1)
  {
    if (clientConsignes.read() != 0xFF)
    {
      //println("Bad packet ! "+millis()); 
      return;
    }
    val1 = clientConsignes.read();
    val2 = clientConsignes.read();
    //
    println("Envoi "+val1+" et "+val2);
    //
    delay(20);
    //
    portRobot.write(0xFF);
    portRobot.write(val1);
    portRobot.write(val2);
    //
    clientConsignes.clear();
  }

}

void serialEvent(Serial thisPort) {

  mesToken = split(thisPort.readString(),' ');  
  for (int i = 0; i < mesToken.length-1; i++)
  {
    serveurRobot.write(mesToken[i]);
    serveurRobot.write(" ");
  }
  serveurRobot.write(10);
}





