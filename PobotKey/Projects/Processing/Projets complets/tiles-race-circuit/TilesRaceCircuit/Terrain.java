// Project Tiles Race Circuit
// by Julien Holtzer
//
// This source code is release under the BSD License.

import processing.core.*;
import java.util.List;
import java.util.ArrayList;

/**
 * Class Terrain that contains all tiles representing a possible circuit
 */
public class Terrain {

  // max number of rows
  int nrow;

  // max number of columns
  int ncol; 

  // table where are put the tiles
  Tile[][] table;

  /**
   * @param row : max number of rows
   * @param col : max number of columns
   */
  public Terrain(int row, int col) {
    nrow = row;
    ncol = col;
    table = new Tile[nrow][ncol];
  }

  /**
   * Add a Tile on the Terrain at given row and given column
   *
   * @param row : the row of the tile
   * @param col : the column of the tile
   * @param t : the tile
   */
  public void setTile(int row, int col, Tile t) {
    table[row][col] = t;    
  }

  /**
   * Return the Tile at the given row and column
   */
  public Tile getTile(int row, int col) {
    return table[row][col]; 
  }

  public Tile getNorthTile(Tile ti) {
    if (ti.row > 0) {
      return table[ti.row-1][ti.column];
    } 
    return null;
  }

  public Tile getSouthTile(Tile ti) {
    if (ti.row < nrow-1) {
      return table[ti.row+1][ti.column];
    } 
    return null;
  }

  public Tile getWestTile(Tile ti) {
    if (ti.column > 0) {
      return table[ti.row][ti.column-1];
    } 
    return null;
  }
  public Tile getEastTile(Tile ti) {
    if (ti.column < ncol-1) {
      return table[ti.row][ti.column+1];
    } 
    return null;
  }

  public List searchConnectedTiles(Tile ti) {
    List l = new ArrayList();
    // tester chacune des 4 directions possibles
    if (ti.isNorthConnection()) {
      // get the north tile on this Terrain (can be null if border)
      Tile t = getNorthTile(ti);
      // test if this tile is not the ancestor
      // test if south is a connection
      if (t != null && !t.equals(ti.testedAncestor) && t.isSouthConnection()) {
        // good connected tile
        l.add(t); 
      }
    }
    // SOUTH
    if (ti.isSouthConnection()) {
      Tile t = getSouthTile(ti);
      if (t != null && ! t.equals(ti.testedAncestor) && t.isNorthConnection()) {
        l.add(t); 
      }      
    }
    // WEST
    if (ti.isWestConnection()) {
      Tile t = getWestTile(ti);    
      if (t != null && ! t.equals(ti.testedAncestor) && t.isEastConnection()) {
        l.add(t); 
      }      
    }
    // EAST
    if (ti.isEastConnection()) {
      Tile t = getEastTile(ti);      
      if (t != null && ! t.equals(ti.testedAncestor) && t.isWestConnection()) {
        l.add(t); 
      }      
    }
    return l;
  }

  /**
   * Distribute empty tiles over the Terrain,
   * with all rows and all columns filled.
   */
  public void distributeTiles()
  {
    for (int i = 0; i<nrow; i++)
    {
      for (int j = 0; j<ncol; j++)
      {
        Tile t = Tile.newEmptyTile(i,j);
        setTile(i,j,t);
      }
    }
  }

  /**
   * Change all tiles with random shape and direction
   * except empty tile.
   */
  public void distributeRandomTiles()
  {   
    for (int i = 0; i<nrow; i++)
    {
      for (int j = 0; j<ncol; j++)
      {
        Tile t = Tile.newRandomTile(i,j);
        setTile(i,j,t);
      }
    }
  }

  /**
   * Change the direction to the next value of a Tile at given row and given column
   * @param tileRow : row of the tile to be changed
   * @param tileColumn : column of the tile to be changed
   *
   */
  public void changeDirectionTile(int tileRow, int tileColumn)
  {
    table[tileRow][tileColumn].nextDirection();
  }

  /**
   * Change the shape to the next value of a Tile at given row and given column
   * @param tileRow : row of the tile to be changed
   * @param tileColumn : column of the tile to be changed
   *
   */
  public void changeShapeTile(int tileRow, int tileColumn)
  {
    table[tileRow][tileColumn].nextShape();
  }

  /** 
   * Display the Terrain in the main window
   * @param applet : the Processing window where to display the Terrain
   */
  public void draw(PApplet applet) {
    for (int i = 0; i<nrow; i++)
    {
      for (int j = 0; j<ncol; j++)
      {
        // simply call the draw method of each Tile.
        table[i][j].draw(applet);
      }
    }     
  }
  
  

  public void emptyTestedMarks() {
    for (int i = 0; i<nrow; i++)
    {
      for (int j = 0; j<ncol; j++)
      {
        table[i][j].tested = false;
        table[i][j].testedAncestor = null;
      }
    }     
  } 
  
  
}
















