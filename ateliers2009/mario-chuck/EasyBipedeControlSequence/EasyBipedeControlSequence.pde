import proxml.*;
import processing.serial.*;


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

// la séquence en cours
BipedeSequence curSequence;

int curIndex = 0;

int curSpeed = 0;

PImage bg;
PFont font;

Serial port;

void setup()
{
  size(836, 459);
  bg = loadImage("interface_easybipede.png");
  font = loadFont("ArialMT-32.vlw");
  textAlign(LEFT);
  textFont(font, 20);
  // 
  println(Serial.list());
  String portName = Serial.list()[0];
  port = new Serial(this, portName, 9600);
  port.bufferUntil(10); // line feed
  //
  loadSequence("forward");
}

void draw()
{
  background(bg);

  // Première partie : indiquer le passage de la souris selon le cas
  afficheSouris();

  // Seconde partie : indiquer la situation courante (séquence, play/stop, etc)
  afficheSequence();
  afficheFleche();
  afficheMode();
  afficheSpeed();
  afficheSharp();

  afficheCommande();
}

void afficheCommande()
{  
  fill(0,120,0);
  noStroke();
  BipedeCommande cur = curSequence.getCommande(curIndex);
  rect(2+(cur.hg-1)*30,4+(10-cur.gg)*30,30,30);
  rect(2+(cur.hd-1)*30+269,4+(10-cur.gd)*30,30,30);
}

void afficheSpeed()
{
  fill(20); 
  text(curSpeed, 627, 162);
}

void afficheSharp()
{ 
  fill(0,250,0);
  noStroke();
  rect(4,4,distance_left, 27);
  rect(273,4,distance_right, 27); 
}

void afficheMode()
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

void afficheFleche()
{ 
  fill(0,255,0,30); // couleur verte
  stroke(1);

  String seqname = curSequence.getName(); // le nom de la séquence courante
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

void afficheSouris()
{

  // cas 1 : la grille
  if (flag_modif_position && mouseY>30 && mouseY<300 && mouseX <540) {
    // conversion des coordonnées de la souris en ligne/colonne (division entière)
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
  // cas 2 : la séquence
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
    if (mouseX>610 && mouseX<610+53 && mouseY>175 && mouseY<175+53)
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
  if (mouseX < 540 && mouseY > 31 && mouseY < 300 && modeRecord)
  {
    saveNewCommande(mouseX,mouseY);
  }
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

void saveNewCommande(int x, int y)
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
  // sauver la séquence
  enregistreSequence();
}

void enregistreSequence()
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

void keyPressed()
{
  if (keyCode == RIGHT)
  {
    curIndex++;
    if (curIndex >= curSequence.nombreCommandes())
    {
      curIndex = 0; 
    }
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

/**
 *
 */
void loadSequence(String name)
{
  xmlIO = new XMLInOut(this);
  xmlIO.loadElement(name+".xml"); 
}

void xmlEvent(proxml.XMLElement xml){    // création de la séquence
  BipedeSequence seq = new BipedeSequence();    
  seq.setName(xml.getAttribute("name"));

  println(seq.getName());
  // préparation de la liste des commandes
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
void afficheSequence()
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

















