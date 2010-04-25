import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;


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
      System.out.print(t.row);
      System.out.print(","); 
      System.out.print(t.column);
      System.out.print(") "); 
    }
    System.out.println();    
  }


}




