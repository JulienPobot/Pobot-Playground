import lll.wrj4P5.*;
import lll.Loc.*;
Wrj4P5 wii;

Loc[] irl;

void setup() {
  size(300,300,P3D);

  // connect first wiimote
  wii = new Wrj4P5(this).connect(true);
}

void draw()
{
  // wait until wiimote connected
  if (wii.isConnecting()) return;
  
  background(0);

  wii.rimokon.setLED(1,true);
  wii.rimokon.setLED(2,false);
  
  irl = wii.rimokon.irLights;  
  drawIrl();

}

void drawIrl()
{
  fill(255,0,0);
  ellipse(irl[0].x*width,irl[0].y*height,5,5);
  fill(0,255,0);
  ellipse(irl[1].x*width,irl[1].y*height,5,5);
  fill(0,0,255);
  ellipse(irl[2].x*width,irl[2].y*height,5,5);
  
  println(irl[0].x);  
}


