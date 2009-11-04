import processing.serial.*;

int rows = 9;
int columns = 18;

int distance_left;
int distance_right;

boolean flag_modif_position = true;
boolean flag_modif_sequence = true;

// la séquence en cours
BipedeSequence curSequence;

PImage bg;
PFont font;

Serial port;

void setup()
{
  size(798, 480);
  bg = loadImage("interface_easybipede.jpg");
  font = loadFont("ArialMT-32.vlw");
  textAlign(LEFT);
  textFont(font, 32);
  // 
  println(Serial.list());
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600);
  port.bufferUntil(10); // line feed
  //
  curSequence = BipedeSequenceLoader.load("left");
}

void draw()
{
  background(bg);

  /* Première partie : indiquer le passage de la souris selon le cas */

  // cas 1 : la grille
  if (flag_modif_position && mouseY>30 && mouseY<300 && mouseX <540) {
    // conversion des coordonnées de la souris en ligne/colonne (division entière)
    int mCol = mouseX/30;
    int mRow = mouseY/30;
    // position de la souris
    fill(30);  
    rect(2+mCol*30,2+mRow*30,29,29);
  }
  // cas 2 : la séquence
  if (flag_modif_sequence)
  { 
    fill(0,0,250,50);
    if (mouseX>640 && mouseX<640+55 && mouseY>140 && mouseY<140+55)
    {
      rect(640,140,55,55);
    }
    if (mouseX>640 && mouseX<640+55 && mouseY>220 && mouseY<220+55)
    {
      rect(640,220,55,55);
    }
    if (mouseX>560 && mouseX<560+55 && mouseY>220 && mouseY<220+55)
    {
      rect(560,220,55,55);
    }
    if (mouseX>720 && mouseX<720+55 && mouseY>220 && mouseY<220+55)
    { 
      rect(720,220,55,55);
    }
  }

  /* Seconde partie : indiquer la situation courante (séquence, play/stop, etc) */

  // cas 1 : la sequence en cours

  fill(0,255,0,30); // couleur verte
  noStroke();

  String seqname = curSequence.getName(); // le nom de la séquence courante
  if (seqname.equals("forward"))
  {
    rect(640,140,55,55);
  } 
  else if (seqname.equals("backward")) {    
    rect(640,220,55,55);
  } 
  else if (seqname.equals("left")) {    
    rect(560,220,55,55);
  } 
  else if (seqname.equals("right")) {    
    rect(720,220,55,55);
  }
  
  // cas 2 : la distance des capteurs Sharp
  fill(0,250,0);
  rect(4,4,distance_left, 27);
  rect(273,4,distance_right, 27);


  /*
  fill(0,120,0);
   rect((cmd[1]-1-48)*30,(cmd[0]-48)*30,30,30);
   rect((cmd[3]-1-48)*30+269,(cmd[2]-48)*30,30,30);
   //
   String pos = new String(cmd);
   text(pos, 15, 25); 
   */
}

void sendPosition()
{
  /*
  port.write(cmd[0]);
   port.write(cmd[1]);
   port.write(cmd[2]);
   port.write(cmd[3]);
   port.write('p');
   */
}

void mouseReleased()
{  
  /*
  if (mouseX<270) {
   cmd[0] = (char) (mRow+48);
   cmd[1] = (char) (mCol+1+48);
   } 
   else {
   cmd[2] = (char) (mRow+48);
   cmd[3] = (char) (mCol-8+48);
   }
   sendPosition();
   */
}


/**
 * Réception d'une trame
 */
void serialEvent(Serial p) {
  // traiter la chaine de caractères
  String trame = port.readString();
  // vérifier la longueur
  if (trame.length() < 7) {
    print("e");
    return;
  }   
  String[] parts = split(trame, " ");  
  distance_right = (distance_right+int(float(parts[0])/2.5))/2;
  distance_left = (distance_left+int(float(parts[1])/2.5))/2;
}













