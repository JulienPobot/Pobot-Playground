/**
 * PID. 
 * 
 * Click, drag, and release the horizontal bar to start the PID. 
 * 
 * Updated 19 December 2008
 */

// array of Mobiles
MobileTest mobile;


void setup() 
{
  size(500,300);
  rectMode(CORNERS);
  noStroke();
  // initialisation des mobiles
  mobile = new MobileTest(0, width/2,2,0,0);<
  //

}

void draw() 
{
  background(100);
  for (int i=0;i<lineQty;i++)
  {      
    //mobiles[i].updateMobile();
    mobiles[i].drawMobile();
  }
}

void mousePressed() {

  for (int i=0;i<lineQty;i++)
  {      
    mobiles[i].testMove();
  }
}

void mouseReleased()
{
 for (int i=0;i<lineQty;i++)
  {      
    mobiles[i].endMove();
  }
}
