import processing.serial.*;
import ddf.minim.*;

AudioSample kick;
AudioSample snare;
AudioSample hat;
AudioSample hc;


Serial port;
int val;
boolean isplaying = false;

void setup() 
{
  size(500,500);
  smooth();

  // port série
  port = new Serial(this, Serial.list()[1], 9600);
  
  // gestion audio
  Minim.start(this);
  
  //
  kick = Minim.loadSample("C_Kick.wav",512);  
  hc = Minim.loadSample("C_HC.wav",512);  
  snare = Minim.loadSample("C_Snare.wav",512);  
}

void draw()
{
  while (port.available() >= 3) {
    if (port.read() == 0xff) {
      val = (port.read() << 8) | (port.read());
    }
  }
  // afficher la valeur dans la console
  //println("Val: "+ val);
  
  // jouer les instruments
  if (! isplaying)
  {
    if (val < 500) 
    {
      kick.trigger();    
      //isplaying = true;
      println("kick");
      fill(0,0,255);
      ellipse(random(500),random(500),10,10);      
    } else if (val < 800) { 
      snare.trigger();  
      isplaying = true;
      println("snare");
      fill(255,0,0);
      rect(random(500),random(500),10,10);
    } else if (val < 750) {   
      hc.trigger();  
      isplaying = true;
      println("hc");
    }
  } else {
    if (val > 850) {    
      // on vient de jouer quelque chose
      // et le champ est libre
      isplaying = false;
    }
  }
  
}

