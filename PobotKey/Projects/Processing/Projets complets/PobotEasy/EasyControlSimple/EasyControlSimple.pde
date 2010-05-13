/**
# Copyright (c) 2009 Julien Holtzer
*/

import processing.net.*;

// le serveur qui diffusera les consignes de vites
Server myServer;


void setup() 
{
  // Starts a myServer on port 5204
  myServer = new Server(this, 5204); 
  //
  size(100,100);
}

void draw()
{

}

void keyPressed()
{
  println("Received "+keyCode);
  switch(keyCode)
  {
  case UP:
    // envoyer les valeurs de vitesse
    myServer.write(0xFF);
    myServer.write(100);
    myServer.write(100);    
    return;
  case DOWN:
    // envoyer les valeurs de vitesse
    myServer.write(0xFF);
    myServer.write(-100);
    myServer.write(-100);
    return;
  case LEFT:
    // envoyer les valeurs de vitesse
    myServer.write(0xFF);
    myServer.write(-100);
    myServer.write(100);
    return;
  case RIGHT:
    // envoyer les valeurs de vitesse
    myServer.write(0xFF);
    myServer.write(100);
    myServer.write(-100);
    return;
  }

  switch(key)
  {
  case ' ':  
    // envoyer les valeurs de vitesse
    myServer.write(0xFF);
    myServer.write(0);
    myServer.write(0);
    break;    
  default:
    break;
  }
}

