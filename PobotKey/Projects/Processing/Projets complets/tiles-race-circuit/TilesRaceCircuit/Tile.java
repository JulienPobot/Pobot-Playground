// Project Tiles Race Circuit
// by Julien Holtzer
//
// This source code is release under the BSD License.


import processing.core.*;

/**
 * Class Tile that represents a part of circuit
 */
public class Tile extends PApplet {

  // the width of every Tile
  public static int tileW;

  // the height of every Tile
  public static int tileH;

  // 0 : straight, 1 : round, 2 : duck, 3 : empty
  public int shape;

  // initally direction, now just one of the possible version of this shape
  // see TilesBag class
  public int dir;

  // numéro de ligne >= 0
  public int row;

  // numéro de colonne >= 0
  public int column;

  // if the Tile has been tested for being part of a circuit
  boolean tested = false;
  
  Tile testedAncestor;

  /**
   * @param shape : family of the shape
   * @param dir : direction of the shape (or similar)
   * @param raw : index of raw where to put the raw
   * @param column : index of column where to put the column
   */
  public Tile(int shape, int dir, int row, int column)
  {
    this.shape = shape;
    this.row = row;
    this.dir = dir;
    this.column = column;

  }

  public static Tile newEmptyTile(int raw, int column) {
    return new Tile(0,0,raw, column);
  }

  public static Tile newRandomTile(int raw, int column){
    Tile t = newEmptyTile(raw, column);
    t.randomShapeDir();
    return t;
  }

  public void randomShapeDir() {   
    int sh = (int) random(2);
    int dir = (int) random(5);
    this.shape = sh;
    this.dir = dir; 
  }

  public void draw(PApplet applet)
  {
    applet.pushMatrix();
    applet.translate(column*tileW,row*tileH);
    applet.fill(255);
    applet.stroke(255,0,0);
    applet.strokeWeight(1);    
    applet.rect(2,2,tileW-4,tileH-4);    
    //
    PImage i = TilesBag.getTileImage(this.shape,this.dir);
    if (i != null) {
      applet.image(i,0,0);
    }
    //
    applet.popMatrix();
  }

  public void nextDirection()
  {
    this.dir = (this.dir+1)%TilesBag.dirMax;       
  }

  public void nextShape()
  {
    this.shape = (this.shape+1)%TilesBag.shapeMax;  
  }

  public boolean isNorthConnection()
  {
    return (shape == 0 && (dir == 1 || dir == 2)) || (shape == 1 && (dir == 0 || dir >= 3)) || (shape == 2 && (dir != 3));

  }
  
  public boolean isSouthConnection()
  {
    return (shape == 0 && (dir == 1 || dir == 2)) || (shape == 1 && (dir != 0 && dir != 3)) || (shape == 2 && (dir != 1));

  }
  
  public boolean isWestConnection()
  {
    return (shape == 0 && (dir == 0 || dir == 2)) || (shape == 1 && (dir != 0 && dir != 1)) || (shape == 2 && (dir != 2));

  }
  
  public boolean isEastConnection()
  {
    return (shape == 0 && (dir == 0 || dir == 2)) || (shape == 1 && (dir != 2 && dir != 3)) || (shape == 2 && (dir != 0));

  }

}




