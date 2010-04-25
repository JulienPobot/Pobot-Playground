import processing.serial.*;

int rows = 9;
int columns = 18;

char cmd[] = {
  '5','5','5','5'};

PImage bg;
PFont font;

Serial port;


void setup()
{
  size(540, 300);
  bg = loadImage("fond_easybipede.jpg");
  font = loadFont("ArialMT-32.vlw");
  textAlign(LEFT);
  textFont(font, 32);
  // 
  println(Serial.list());
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600);
}

void draw()
{
  background(bg);

  int mCol = mouseX/30;
  int mRow = mouseY/30;

  if (mouseY>30) {
    // position de la souris    
    fill(30);  
    rect(mCol*30,mRow*30,30,30);
    //
    if (mousePressed == true) {
      if (mouseX<270) {
        cmd[0] = (char) (mRow+48);
        cmd[1] = (char) (mCol+1+48);
      } 
      else {
        cmd[2] = (char) (mRow+48);
        cmd[3] = (char) (mCol-8+48);
      }
      sendPosition();
    }
  }

  // sélection courante   
  fill(0,120,0);
  rect((cmd[1]-1-48)*30,(cmd[0]-48)*30,30,30);
  rect((cmd[3]-1-48)*30+269,(cmd[2]-48)*30,30,30);
  //
  String pos = new String(cmd);
  text(pos, 15, 25); 

}

void sendPosition()
{
  port.write(cmd[0]);
  port.write(cmd[1]);
  port.write(cmd[2]);
  port.write(cmd[3]);
  port.write('p');
}







