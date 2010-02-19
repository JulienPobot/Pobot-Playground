import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;
import processing.core.*;


public class CircuitAnalyzer {

  List circuits;

  public CircuitAnalyzer()
  {
    circuits = new ArrayList(); 
  }



  /**
   * @return boolean true if at least one complete circuit has been found
   */
  public boolean analyze(Terrain ter) {
    //
    int count = 0;
    // empty the current list
    circuits = new ArrayList();
    // clean all tested marks on Terrain.
    ter.emptyTestedMarks(); 

    // browse all Tiles on the Terrain to start searching a circuit
    for (int i=0;i<ter.nrow;i++) {
      for (int j=0; j<ter.ncol;j++) {
        // test if the current Tile has not yet been tested
        Tile ti = ter.getTile(i,j);
        if (!ti.tested) {
          count++;
          startNewAnalysis(ti,ter); 
        }        
      }      
    }    
    System.out.println("> started "+count+" new analysis.");
    return (circuits.size() != 0);
  } 

  public void startNewAnalysis(Tile ti, Terrain ter) {
    // mark the tile as tested
    ti.tested = true;

    // create a new circuit
    TiledCircuit tc = new TiledCircuit();
    
    // add the Tile to this circuit
    tc.addTile(ti);

    // search the connected Tiles
    List cons = ter.searchConnectedTiles(ti);

    Iterator it = cons.iterator(); 
    while (it.hasNext()) {
      Tile t = (Tile) it.next();
      // mark them as tested ancestor
      t.testedAncestor = ti;
      // continue the analysis from this new Tile
      continueAnalysis(t,tc,ter);
    }


    // test if the analysis shows a closed circuit
    if (tc.closed) {
      // display the found sequence
      tc.displayText();
      //
      // if true, add it to the list
      circuits.add(tc);
    }

  }

  /**
   *
   */
  public void continueAnalysis(Tile ti, TiledCircuit tc, Terrain ter) {   
    // mark the Tile as tested
    ti.tested = true;
    // add it to the circuit
    tc.addTile(ti);
    // search the connected Tiles
    List cons = ter.searchConnectedTiles(ti); 
    //
    Iterator it = cons.iterator(); 
    while (it.hasNext()) {
      Tile t = (Tile) it.next();
      // stop the recursion
      if (tc.containsTile(t)) {
        // succesfully found a circuit !
        tc.closed = true; 
      } 
      else {
        // mark the ancestor
        t.testedAncestor = ti;
        // continue the analysis from this new Tile
        continueAnalysis(t,tc,ter);
      }
    }
  }

  public void displayCircuits(PApplet pa) {
    Iterator it = circuits.iterator();
    while (it.hasNext()) {
      TiledCircuit tc = (TiledCircuit) it.next();
      tc.draw(pa);
    } 
  }
}









