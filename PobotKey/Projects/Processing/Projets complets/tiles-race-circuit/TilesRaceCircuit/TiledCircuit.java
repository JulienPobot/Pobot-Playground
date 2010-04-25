import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;
import processing.core.*;


public class TiledCircuit {

  public List circuit;

  public boolean closed = false;

  public TiledCircuit() {
    circuit = new ArrayList();
  } 

  public void addTile(Tile ti) {
    circuit.add(ti); 
  }

  public boolean containsTile(Tile ti) {
    return circuit.contains(ti); 
  }

  public void displayText() {
    Iterator it = circuit.iterator(); 
    while (it.hasNext()) {
      Tile t = (Tile) it.next();
      System.out.print("("); 
      System.out.print(t.row+1);
      System.out.print(","); 
      System.out.print(t.column+1);
      System.out.print(") "); 
    }
    System.out.println();    
  }

  public void draw(PApplet pa) {
    System.out.println("Draw circuit : "+circuit.size());
    Iterator it = circuit.iterator(); 
    while (it.hasNext()) {
      Tile t = (Tile) it.next();

      pa.pushMatrix();
      pa.translate(t.column*t.tileW,t.row*t.tileH,1);
      pa.fill(255,0,0,100);
      pa.stroke(255,0,0);
      pa.strokeWeight(1);    
      pa.rect(2,2,t.tileW-4,t.tileH-4);    
      pa.popMatrix();
    }    

  }

}





