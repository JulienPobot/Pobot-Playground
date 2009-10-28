import processing.serial.*;

//
MarioSimu msi;
MarioScene msc;

// événements acceptés pour bouger la marionnette 
boolean chuckMove = false;
boolean mouseMove = false;

Serial port;

char mode;

int sauveX;
int sauveY;
boolean flagAlt = false;

void setup()
{ 
  size(520, 520, P3D); 
  background(255);
  lights();
  noStroke();
  fill(0,0,255);

  msi = new MarioSimu(this);
  msc = new MarioScene(this);

  addMouseWheelListener(new java.awt.event.MouseWheelListener() { 
    public void mouseWheelMoved(java.awt.event.MouseWheelEvent evt) { 
      mouseWheel(evt.getWheelRotation());
    }
  }
  ); 

}

void mouseWheel(int delta) {
  if (mouseMove) {
    msi.updateYeux(-delta*5);
  }
}

void draw()
{
  background(255);
  if (chuckMove)
  {
    msi.updateChuck(port);
  }
  if (mouseMove) 
  {
    msi.updateMouse(); 
  }  
  // update MarioSimu
  msi.draw();
  // update MarioScene
  msc.draw();
}


void keyPressed() {
  switch (key)
  {
  case 's':
    String filename = "screenshot_"+year()+nf(month(),2)+nf(day(),2)+"_"+nf(hour(),2)+nf(minute(),2)+nf(second(),2)+".png";
    save(filename);   
    msc.sauvePosition(msi,filename);
    println("Saved photo.");
    break;   
  case 'd':
    println("Display positions:");
    msi.printPositions();
    break;
  case 'm':
    mouseMove = !mouseMove;
    println("Control with mouse "+mouseMove+".");
    break;
  case 'c':
  case 'n': 
    chuckMove = !chuckMove;
    if (port == null) {
      initializePort(); 
    }
    println("Control with nunchuck "+chuckMove+".");
    break;
  case 'h':
    println("Help for Mario Chuck display test :");
    println(" s: save current frame into png");
    println(" m: switch to mouse control");
    println(" n: switch to nunchuck control");
    println(" h: this help");
    break;
  case ' ':
    // nothing specific
    break;
  case CODED:
    // nothing specific
    break;
  default:
    println("Unrecognized command.");
  }
}

void initializePort()
{
  println(Serial.list());
  try {
    port = new Serial(this, Serial.list()[1], 9600);
  } 
  catch (Exception e) {
    println("Error initializing the port.");        
  }
}



















