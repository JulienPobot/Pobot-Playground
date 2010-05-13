/**
# Copyright (c) 2009 Julien Holtzer
*/

import processing.net.*;

// le serveur qui diffusera les consignes de vites
Server myServer;

PImage bg1,bg2,bg3;
PFont fontA;

float x;
float y;

float speed;
float theta;

float vg;
float vd;

float scale = 100;

char choiceBg = 'a';

boolean followMouse = false;

void setup() 
{
  // Starts a myServer on port 5204
  myServer = new Server(this, 5204); 
  //
  size(400,400);
  bg1 = loadImage("radar.png");  
  bg2 = loadImage("radar2.png");  
  bg3 = loadImage("radar3.png");  
  // Set the font and its size (in units of pixels)
  fontA = loadFont("Ziggurat-HTF-Black-32.vlw");
  textFont(fontA, 12);
}

void draw()
{
  if (choiceBg == 'a') 
    background(bg1);
  else if (choiceBg == 'b')
    background(bg2);
  else if (choiceBg == 'c')
    background(bg3);
  else 
    background(100);

  // display the current aim  
  fill(255);
  ellipse(width/2-x,height/2+6-y,8,8);

  // suivi de la souris
  if (followMouse)
  {    
    x = -(mouseX-width/2);
    y = -(mouseY-height/2);  
    convertCtoP();

  }

  //  
  fill(255);
  textAlign(RIGHT);
  text("x: "+(int)x, width/2-x-5, height/2-y+20);
  text("y: "+(int)y, width/2-x-5, height/2-y+32);
  text("s: "+(int)speed, width/2-x-5, height/2-y+44);
  text("a: "+(int)degres(theta), width/2-x-5, height/2-y+56);

  textAlign(LEFT);
  text("speed: "+(int) speed, abs(speed)+15,14);
  text("angle: "+(int)degres(theta), abs(degres(theta))+15,14+15);

  textAlign(CENTER);
  if (vg>=0)
    text("vg: "+(int) vg,width/2-150,height/2+6+15);
  else
    text("vg: "+(int) vg,width/2-150,height/2+6-8);
  if (vd>=0)
    text("vd: "+(int) vd,width/2+150,height/2+6+15);
  else
    text("vd: "+(int) vd,width/2+150,height/2+6-8);

  // vitesse moyenne et angle de rotation

  if (speed<0) fill(180,0,0);
  else fill(0,180,0);
  rect(5,5,abs(speed),10);
  if (theta<0) fill(180,0,0);
  else fill(0,180,0);
  rect(5,20,degres(abs(theta)),10);

  // vitesse roue gauche et vitesse roue droite

  vd = max(-100,min(100,(y + x)/1.5));
  vg = max(-100,min(100,(y - x)/1.5));

  if (vg<0) fill(180,0,0);
  else fill(0,180,0);
  rect(width/2-150,height/2+6,5,-vg/2);  
  if (vd<0) fill(180,0,0);
  else fill(0,180,0);
  rect(width/2+150,height/2+6,5,-vd/2);

  // envoyer les valeurs de vitesse
  myServer.write(0xFF);
  myServer.write((int)vg);
  myServer.write((int)vd);
}

void mousePressed()
{
  followMouse = !followMouse;
}

void keyPressed()
{
  //println(key+" "+keyCode);
  //
  switch(keyCode)
  {
  case UP:
    speed+=1;
    // adjust the theta when passing 0
    if (speed == 0) {
      theta -= PI; 
    }
    convertPtoC();
    return;
  case DOWN:
    speed-=1;
    // adjust the theta when passing 0
    if (speed == 0) {
      theta += PI; 
    }
    convertPtoC();
    return;
  case LEFT:
    theta+=.1;
    convertPtoC();
    return;
  case RIGHT:
    theta-=.1;
    convertPtoC();
    return;
  }

  switch(key)
  {
  case 'a':
    choiceBg = 'a';
    break;
  case 'b':
    choiceBg = 'b';
    break;
  case '1':
    x+=10;
    y-=10;
    convertCtoP();
    break;
  case '2':
    y-=10;
    convertCtoP();
    break;
  case '3':
    x-=10;
    y-=10;
    convertCtoP();
    break;    
  case '4':
    x+=10;
    convertCtoP();
    break;
  case '5':
  case 'R':
    x=0;
    y=0;
    speed = 0;
    theta = 0;
    convertCtoP();
    break;    
  case '6':
    x-=10;
    convertCtoP();
    break;    
  case '7':
    x+=10;
    y+=10;
    convertCtoP();
    break;    
  case '8':
    y+=10;
    convertCtoP();
    break;
  case '9':
    x-=10;
    y+=10;
    convertCtoP();
    break;  
  default:
    choiceBg = key;
    break;
  }


}

void convertPtoC()
{
  moduloTheta();
  x = speed * sin(theta); 
  y = speed * cos(theta);
}

void convertCtoP()
{
  speed = sqrt(pow(x,2)+pow(y,2));
  theta = atan2(x,y);
  if (y<0) {
    speed = -speed;
  }
  moduloTheta();
}

void moduloTheta()
{
  while (theta > PI) {
    theta -= 2*PI;
  } 
  while (theta < -PI) {
    theta += 2*PI; 
  }
}

float degres(float theta)
{
  return 180.0/PI*theta; 
}



