import processing.core.*;
import processing.serial.*;

public class MarioSimu {

  PApplet p;  

  PImage piTete;
  PImage piYeux;
  PImage piBouche;
  PImage piTorso;

  Marionnette mario;

  public MarioSimu(PApplet pa) {
    this.p = pa;

    // init images
    piTete = pa.loadImage("tete.png");
    piYeux = pa.loadImage("yeux.png");
    piBouche = pa.loadImage("bouche.png");
    piTorso = pa.loadImage("torso.png"); 

    // init marionnette
    mario = new Marionnette();


  }

  public void readFigure() {

  }

  public void updateChuck(Serial port) {
    if (port !=  null) {
      // lire la commande série
      if (port.available() > 15) {
        String cur = port.readStringUntil('\n');
        if (cur != null) {
          String[] parts = p.split(cur, " ");
          p.println(parts);
          mario.aNeckP.updatePosition(Integer.valueOf(parts[0]));
          mario.aNeckT.updatePosition(Integer.valueOf(parts[1]));
          mario.aEyesH.updatePosition(Integer.valueOf(parts[2]));
          mario.aEyesV.updatePosition(Integer.valueOf(parts[3]));
          mario.aMouth.updatePosition(Integer.valueOf(parts[4]));
        }
      } 
    }
  }

  public void updateMouse() {
    // conversion de la position x de la souris en valeur de position
    // si la souris est au milieu (x = width/2), la tête est centrée (pos = 0)
    // si la souris est tout à droite (x = width), la tête est tout à gauche (pos = minimum)
    mario.aNeckP.updatePosition((p.mouseX-p.width/2)*(mario.aNeckP.maximum-mario.aNeckP.minimum)/p.width);
    // conversion de la position y de la souris en valeur de position
    // si la souris est au milieu (y = height/2), la tête est centrée (pos = 0)
    // si la souris est tout en bas (y = height), la tête est tout en bas (pos = minimum)
    mario.aNeckT.updatePosition(-(p.mouseY-p.height/2)*(mario.aNeckT.maximum-mario.aNeckT.minimum)/p.height);
  }

  public void updateYeux(int value) {
    mario.aEyesV.incrementPosition(value);
  }

  public void updateChuck(String[] parts) {
    // le nunchuck envoie directement les positions
    mario.aNeckP.updatePosition(new Integer(parts[0]).intValue());


  }

  public void printPositions() {
    p.println(mario);    
  }

  public void draw() {

    mario.aNeckP.draw(p,'h',30,20);
    mario.aNeckT.draw(p,'v',10,30);
    mario.aEyesV.draw(p,'v',30,30);

    // nouveau repère 3D
    p.pushMatrix();
    p.translate(p.width/2,p.height/2,-200);  

    // tête mobile
    p.pushMatrix();
    p.rotateY((float)(p.PI/180.0*mario.aNeckP.position));
    p.rotateX((float)(p.PI/180.0*mario.aNeckT.position));

    // tête
    p.image(piTete,-100,-200);
    // visage
    p.pushMatrix();
    p.translate((float)0,(float)0,(float)0.1); // pour éviter la superposition
    p.image(piYeux,-100+30,-200-mario.aEyesV.position);  
    if (p.key == ' ') {
      p.image(piBouche,-100+40,-100,112,50);
    } 
    else {
      p.image(piBouche,-100+40,-100,112,20);
    }

    // les pupilles
    /*
  pushMatrix();
     translate(0,0,1);
     ellipse(-40,-60,5,5);  
     ellipse(40,-60,5,5);  
     popMatrix();
     */
    p.popMatrix();
    p.popMatrix();
    //

    // torso
    p.image(piTorso,-288,0); 
    p.popMatrix();

  }

}






