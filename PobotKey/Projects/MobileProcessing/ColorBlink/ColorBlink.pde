import processing.bluetooth.*;
import processing.phone.*;
import processing.sound.*;

// les informations à transmettre à notre BlinkM
int hueVal = 0;
int brightnessVal = 0;

final String SERVICE_NAME = "blinkmService";
String[] clients = new String[0];
String msg;  // status message

Bluetooth bt;
Phone p;
 	
void setup()
{
  //
  bt = new Bluetooth(this);
  p = new Phone(this);


  
  //
  noStroke();
  colorMode(HSB, 255);
  for(int i=0; i<height; i++) {
    for(int j=0; j<width; j++) {
      stroke(i, 255, j);
      point(i, j);
    }
  }
} 

void sendValuesBT()
{
  
}

void pointerPressed() { 
  // lire
  hueVal = pointerX;
  brightnessVal = pointerY;
  // afficher
  p.vibrate(100);
  // sonner
  Sound.playTone(20, 200, 50);
  // envoyer sur la liaison série
  sendValuesBT();
}


