import processing.core.*; 
import processing.xml.*; 

import proxml.*; 
import processing.serial.*; 

import java.applet.*; 
import java.awt.*; 
import java.awt.image.*; 
import java.awt.event.*; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class EasyBipedeControlSequence extends PApplet {





int rows = 9;
int columns = 18;

int distance_left;
int distance_right;

boolean flag_modif_position = true;
boolean flag_modif_sequence = true;

boolean modePlay = false;
boolean modePause = false;
boolean modeRecord = true;

XMLInOut xmlIO;

// la s\u00e9quence en cours
BipedeSequence curSequence;

int curIndex = 0;

int curSpeed = 9;

long lastMillis = millis();

PImage bg;
PFont font;

Serial port;

public void setup()
{
  size(836, 459);
  bg = loadImage("interface_easybipede.png");
  font = loadFont("ArialMT-32.vlw");
  textAlign(LEFT);
  textFont(font, 20);
  // 
  println(Serial.list());
  String portName = Serial.list()[1];
  port = new Serial(this, portName, 9600);
  port.bufferUntil(10); // line feed
  //
  loadSequence("left");
}

public void draw()
{
  background(bg);

  // Premi\u00e8re partie : indiquer le passage de la souris selon le cas
  afficheSouris();

  // Seconde partie : indiquer la situation courante (s\u00e9quence, play/stop, etc)
  afficheSequence();
  afficheFleche();
  afficheMode();
  afficheSpeed();
  afficheSharp();

  afficheCommande();


  // Principal : g\u00e9rer la communication avec le robot

  if (millis()-lastMillis > calculePeriode())
  {
    envoieCommande(); 
    lastMillis = millis();
    if (modePlay) {
      curIndex++; 
    }
  }  
}

/** 
 * Nombre de millisecondes \u00e0 s'\u00e9couler entre deux envois
 * D\u00e9pend de la vitesse
 */
public long calculePeriode()
{
  // plus la vitesse est grande plus il y a de temps
  return (10-curSpeed)*1000; // 1 seconde par vitesse  
}

public void afficheCommande()
{  
  fill(0,120,0);
  noStroke();
  BipedeCommande cur = curSequence.getCommande(curIndex);
  rect(2+(cur.hg-1)*30,4+(10-cur.gg)*30,30,30);
  rect(2+(cur.hd-1)*30+269,4+(10-cur.gd)*30,30,30);
}

public void afficheSpeed()
{
  fill(20); 
  text(curSpeed, 627, 162);
}

public void afficheSharp()
{ 
  fill(0,250,0);
  noStroke();
  rect(4,4,distance_left, 27);
  rect(273,4,distance_right, 27); 
}

public void afficheMode()
{ 
  fill(0,255,0,30); // couleur verte
  stroke(1);
  if (modePlay)
  {   
    rect(550,11,53,53);
  }
  if (modePause)
  {   
    rect(610,11,53,53);
  }
  if (modeRecord)
  {   
    rect(670,11,53,53);
  }
}

public void afficheFleche()
{ 
  fill(0,255,0,30); // couleur verte
  stroke(1);

  String seqname = curSequence.getName(); // le nom de la s\u00e9quence courante
  if (seqname.equals("forward"))
  {
    rect(610,175,53,53);
  } 
  else if (seqname.equals("backward")) {    
    rect(610,240,53,53);
  } 
  else if (seqname.equals("left")) {    
    rect(550,207,53,53);
  } 
  else if (seqname.equals("right")) {    
    rect(670,207,53,53);
  }  
}

public void afficheSouris()
{

  // cas 1 : la grille
  if (flag_modif_position && mouseY>30 && mouseY<300 && mouseX <540) {
    // conversion des coordonn\u00e9es de la souris en ligne/colonne (division enti\u00e8re)
    int mCol = mouseX/30;
    int mRow = mouseY/30;
    // position de la souris
    if (mousePressed)
    { 
      fill(220,30,30);  
    } 
    else {
      fill(30);  
    }
    rect(2+mCol*30,4+mRow*30,30,30);
  }
  // cas 2 : la s\u00e9quence
  if (flag_modif_sequence)
  { 
    noStroke();
    if (mousePressed)
    {
      fill(250,0,0,50);
    } 
    else {
      fill(0,0,250,50);
    }
    if (mouseOverForward())
    {
      rect(610,175,53,53);
    }
    if (mouseX>610 && mouseX<610+53 && mouseY>243 && mouseY<243+53)
    {
      rect(610,243,53,53);
    }
    if (mouseX>550 && mouseX<550+53 && mouseY>207 && mouseY<207+53)
    {
      rect(550,207,53,53);
    }
    if (mouseX>670 && mouseX<670+53 && mouseY>207 && mouseY<207+53)
    { 
      rect(670,207,53,53);
    }
  }
  // cas 3, les boutons du mode
  if (mouseOverPlay())
  {
     rect(550,11,53,53);
  }
  if (mouseOverStop())
  {
     rect(610,11,53,53);
  }
  if (mouseOverRecord())
  {
     rect(670,11,53,53);
  }
  
}

public void envoieCommande()
{
  BipedeCommande cur = curSequence.getCommande(curIndex);
  port.write(48+cur.gg);
  port.write(48+cur.hg);
  port.write(48+cur.gd);
  port.write(48+cur.hd);
  port.write('p');
  // 
  println("Sent : "+cur.gg+""+cur.hg+""+cur.gd+""+cur.hd);

}

public void mouseChangeCommande()
{  
  if (mouseX < 540 && mouseY > 31 && mouseY < 300 && modeRecord)
  {
    saveNewCommande(mouseX,mouseY);
  }   
}

public void mouseChangeSequence()
{
  String seqname = curSequence.getName(); // le nom de la s\u00e9quence courante
  if (mouseOverForward() && ! seqname.equals("forward"))
  {
    loadSequence("forward");  
    curIndex = 0;  
  }
  if (mouseOverBackward() && ! seqname.equals("backward"))
  {
    loadSequence("backward"); 
    curIndex = 0;     
  }
  if (mouseOverLeft() && ! seqname.equals("left"))
  {
    loadSequence("left");    
    curIndex = 0;  
  }
  if (mouseOverRight() && ! seqname.equals("right"))
  {
    loadSequence("right");    
    curIndex = 0;  
  }

}

public boolean mouseOverPlay()
{
  return (mouseX >= 550 && mouseX <= 550+53 && mouseY>=11 && mouseY <= 11+53);   
}

public boolean mouseOverStop()
{
  return (mouseX >= 610 && mouseX <= 610+53 && mouseY>=11 && mouseY <= 11+53);   
}

public boolean mouseOverRecord()
{
  return (mouseX >= 670 && mouseX <= 670+53 && mouseY>=11 && mouseY <= 11+53);   
}

public boolean mouseOverForward()
{
  return (mouseX >= 610 && mouseX <= 610+53 && mouseY>=175 && mouseY <= 175+53);  
}

public boolean mouseOverBackward()
{
  return (mouseX >= 610 && mouseX <= 610+53 && mouseY>=240 && mouseY <= 240+53);  
}

public boolean mouseOverLeft()
{
  return (mouseX >= 550 && mouseX <= 550+53 && mouseY>=207 && mouseY <= 207+53);  
}

public boolean mouseOverRight()
{
  return (mouseX >= 670 && mouseX <= 670+53 && mouseY>=207 && mouseY <= 207+53);  
}

public void mouseReleased()
{ 
  // changement de commande
  mouseChangeCommande();

  // changement de s\u00e9quence
  mouseChangeSequence();  


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

public void saveNewCommande(int x, int y)
{  
  // enregistrer la nouvelle position  
  BipedeCommande cur = curSequence.getCommande(curIndex);
  if (x < 270) // jambe gauche
  {
    cur.gg = 10-y/30;
    cur.hg = (x/30)+1;
  } 
  else { // jambe droite
    cur.gd = 10-y/30;
    cur.hd = ((x-270)/30)+1;
  }  
  // sauver la s\u00e9quence
  enregistreSequence();
}

public void enregistreSequence()
{

  proxml.XMLElement xml = new proxml.XMLElement("sequence"); 
  xml.addAttribute("name",curSequence.getName());

  for (int i = 0; i < curSequence.nombreCommandes(); i++)
  {
    BipedeCommande bc = curSequence.getCommande(i);
    proxml.XMLElement fils = new proxml.XMLElement("commande");
    fils.addAttribute("gg", bc.gg);
    fils.addAttribute("hg", bc.hg);
    fils.addAttribute("gd", bc.gd);
    fils.addAttribute("hd", bc.hd);
    xml.addChild(fils);    
  }

  xmlIO.saveElement(xml,curSequence.getName()+".xml"); 
}

public void keyPressed()
{
  if (keyCode == RIGHT)
  {
    curIndex++;
    if (curIndex >= curSequence.nombreCommandes())
    {
      curIndex = 0; 
    }
    envoieCommande(); 
    lastMillis = millis();
  }
  if (keyCode == LEFT)
  {
    if (curIndex == 0)
    {
      curIndex = curSequence.nombreCommandes()-1;
    } 
    else {
      curIndex--;
    }
    envoieCommande(); 
    lastMillis = millis();
  }
  if (keyCode == UP)
  {
    curSpeed++;
    if (curSpeed > 9) 
    {
      curSpeed = 9;
    }
  }
  if (keyCode == DOWN)
  {
    if (curSpeed !=0)
    {
      curSpeed--; 
    }
  }

  if (keyCode == DELETE && modeRecord)
  {
    curSequence.removeCommande(curIndex);
    enregistreSequence();
  }

  if (keyCode == 155 && modeRecord) 
  {
    curSequence.insertCommande(curIndex);
    enregistreSequence();    
  }

}


/**
 * R\u00e9ception d'une trame
 */
public void serialEvent(Serial p) {
  // traiter la chaine de caract\u00e8res
  String trame = port.readString();
  // v\u00e9rifier la longueur
  if (trame.length() < 7) {
    print("e");
    return;
  }   
  String[] parts = split(trame, " ");  
  distance_right = (distance_right+PApplet.parseInt(PApplet.parseFloat(parts[0])/2.5f))/2;
  distance_left = (distance_left+PApplet.parseInt(PApplet.parseFloat(parts[1])/2.5f))/2;
}

/**
 *
 */
public void loadSequence(String name)
{
  xmlIO = new XMLInOut(this);
  xmlIO.loadElement(name+".xml"); 
}

public void xmlEvent(proxml.XMLElement xml){    // cr\u00e9ation de la s\u00e9quence
  BipedeSequence seq = new BipedeSequence();    
  seq.setName(xml.getAttribute("name"));

  println(seq.getName());
  // pr\u00e9paration de la liste des commandes
  ArrayList al = new ArrayList();

  // fichier xml sous la forme
  // <?xml version="1.0"?>
  // <sequence name="filename">
  //   <commande gg="0" hg="0" gd="0" hd="0" />
  //   <commande gg="0" hg="0" gd="0" hd="0" />
  // </sequence>
  // nombre de commandes :
  int numCmds = xml.countChildren();
  // pour chaque commande : 
  for (int i = 0; i < numCmds; i++) {
    // 
    BipedeCommande bc = new BipedeCommande();
    proxml.XMLElement cmd = xml.getChild(i);
    bc.gg = cmd.getIntAttribute("gg"); 
    bc.hg = cmd.getIntAttribute("hg"); 
    bc.gd = cmd.getIntAttribute("gd"); 
    bc.hd = cmd.getIntAttribute("hd");     
    al.add(bc);
  }
  seq.setCommandes(al);    
  curSequence = seq;
}


/**
 *
 */
public void afficheSequence()
{  
  for (int i = 0; i < curSequence.nombreCommandes(); i++)
  {
    if (i == curIndex) 
    {
      if (modePlay)
      {
        fill(20,255,20);
      } 
      if (modePause)
      {
        fill(20,20,255);

      }
      if (modeRecord)
      {
        fill(255,20,20);
      }
    } 
    else {     
      fill(20); 
    }
    text(curSequence.getCommande(i).textValue(), 744, 32+i*32);
  }


}




















  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#D4D0C8", "EasyBipedeControlSequence" });
  }
}
