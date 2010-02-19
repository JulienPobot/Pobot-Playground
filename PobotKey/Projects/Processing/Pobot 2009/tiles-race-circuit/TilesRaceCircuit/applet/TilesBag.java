// Project Tiles Race Circuit
// by Julien Holtzer
//
// This source code is release under the BSD License.

import processing.core.*;

/**
 *
 */
public class TilesBag extends PApplet {

  // singleton
  private static TilesBag instance = new TilesBag();

  public static int shapeMax = 4;

  public static int dirMax = 7;  

  public static int roundmax = 6;
  
  public static int duckmax = 4;

  public static int straightmax = 3;  

  public static int shapeMax() {
    return 3; 
  }

  static PImage[] tilesRound = new PImage[roundmax];

  static PImage[] tilesStraight = new PImage[straightmax];
  
  static PImage[] tilesDuck = new PImage[duckmax];

  static PImage tileEmpty;

  public TilesBag() {
  }

  public static void initTilesBag(PApplet applet) {
    TilesBag.instance.initBag(applet);
  }

  public void initBag(PApplet applet) {
    tilesRound[0] = applet.loadImage("../data/marble_roundNEs.png");
    tilesRound[1] = applet.loadImage("../data/marble_roundSEs.png");
    tilesRound[2] = applet.loadImage("../data/marble_roundSWs.png");
    tilesRound[3] = applet.loadImage("../data/marble_roundNWs.png");
    tilesRound[4] = applet.loadImage("../data/marble_doublels.png");
    tilesRound[5] = applet.loadImage("../data/marble_doublers.png");

    tilesStraight[0] = applet.loadImage("../data/marble_straights.png");
    tilesStraight[1] = applet.loadImage("../data/marble_straight2s.png");
    tilesStraight[2] = applet.loadImage("../data/marble_crosss.png");

    tileEmpty = applet.loadImage("../data/marble_emptys.png");
    
    tilesDuck[0] = applet.loadImage("../data/marble_doublesideWs.png");
    tilesDuck[1] = applet.loadImage("../data/marble_doublesideNs.png");
    tilesDuck[2] = applet.loadImage("../data/marble_doublesideEs.png");
    tilesDuck[3] = applet.loadImage("../data/marble_doublesideSs.png");
    
  }

  public static PImage getTileImage(int shape, int dir) {
    return TilesBag.instance.getImage(shape,dir);
  }

  public PImage getImage(int shape, int dir) {
    switch (shape)
    {
    case 0:
      return tilesStraight[dir%TilesBag.straightmax];
    case 1:
      return tilesRound[dir%TilesBag.roundmax];
    case 2:
      return tilesDuck[dir%TilesBag.duckmax];    
    case 3:
      return tileEmpty;
    }
    return tileEmpty;
  }

}



