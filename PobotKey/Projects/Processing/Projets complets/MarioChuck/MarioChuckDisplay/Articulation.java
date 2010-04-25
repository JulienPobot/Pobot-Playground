import processing.core.*;
import java.lang.Math;

public class Articulation {

  final static int minIncrement = 1;

  // les limites  
  int minimum;
  int maximum;

  // la position (consigne et courante)
  int position;
  int consigne;

  public Articulation(int minPos, int maxPos, int value) {
    minimum = minPos;
    maximum = maxPos;
    position = value;
    consigne = value;
  } 

  /**
   * Corrige une valeur en fonction des 
   */
  private int corrige(int value) {
    return Math.max(minimum,Math.min(maximum,value));
  }

  /**
   * Met à jour la consigne avec une nouvelle valeur
   */
  public void updatePosition(int value) {    
    consigne = corrige(value);
  }

  /**
   *
   */
  public void incrementPosition(int incr) {
    consigne += incr;      
    consigne = corrige(consigne);
  }

  public int getProportionalPosition(double ratio) {
    return (int) (position * ratio); 
  }

  /**
   *
   */
  public void update() {
    if (position == consigne) { 
      return; 
    }   
    if (position < consigne) { 
      position += minIncrement; 
    } 
    else { 
      position -= minIncrement; 
    }
    position = corrige(position);
  }

  public void draw(PApplet p, char type, int x, int y) {
    // mettre à jour la position selon la consigne
    update();
    // horizontal
    if (type == 'h') {
      p.fill(200);
      p.rect(x,p.height-y,180,10);
      p.fill(100);
      p.rect(x,p.height-y,(position-minimum)*180/(maximum-minimum),10);      
      p.fill(255,0,0);
      p.rect(x+180*(consigne-minimum)/(maximum-minimum)-5,p.height-y,10,10);
    } 
    // vertical
    else {
      p.fill(200);
      p.rect(x,p.height-y-180,10,180);
      p.fill(100);
      p.rect(x,p.height-y,10,-(position-minimum)*180/(maximum-minimum));  
      p.fill(255,0,0);
      p.rect(x,p.height-y-(consigne-minimum)*180/(maximum-minimum)-5,10,10);
    }         
  }
  
  public String toString() {
     return minimum + " < " + position + " / " + consigne + " < " + maximum + "\n";
  }
}








