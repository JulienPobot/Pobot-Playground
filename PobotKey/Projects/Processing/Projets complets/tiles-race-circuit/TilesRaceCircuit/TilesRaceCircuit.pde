/*
# Copyright (c) 2009 Julien Holtzer
 #
 # Permission is hereby granted, free of charge, to any person obtaining a copy
 # of this software and associated documentation files (the "Software"), to deal
 # in the Software without restriction, including without limitation the rights
 # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 # copies of the Software, and to permit persons to whom the Software is
 # furnished to do so, subject to the following conditions:
 #
 # The above copyright notice and this permission notice shall be included in
 # all copies or substantial portions of the Software.
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 # THE SOFTWARE.
 */


Terrain terr;
TilesBag tbag;
CircuitAnalyzer circa;

void setup()
{
  Tile.tileW = 60;
  Tile.tileH = 60;

  TilesBag.initTilesBag(this);

  // Create a new empty circuit of empty tiles
  // max number of rows and max number of columns
  terr = new Terrain(4,5);
  terr.distributeTiles();

  // Initalize the circuit analyzer
  circa = new CircuitAnalyzer();  


  // Set the graphical area size
  size(Tile.tileW*terr.ncol,Tile.tileH*terr.nrow,P3D);

  // White background
  background(255);

  noLoop();
}

void draw()
{  
  terr.draw(this);
}

void analysis() {
  
  
  // Start an analysis
  circa = new CircuitAnalyzer();
  boolean good = circa.analyze(terr);

  if (good) {
    println("Found circuits !"); 
  } 
  else {
    // terr.distributeRandomTiles();
    // recursion
    // analysis();
  }

}

void keyPressed()
{
  switch (key)
  {
  case 's':
    save("../circuits/race_"+year()+nf(month(),2)+nf(day(),2)+"_"+nf(hour(),2)+nf(minute(),2)+".png");   
    println("Saved circuit.");
    break;    
  case 'r':
    terr.distributeRandomTiles();
    println("Redistribute random tiles.");
    break;
  case 'a':
    println("Starting analysis for circuits");
    analysis();
    break;
  default: 
    println("Unrecognized command.");
  }  
  redraw();
}

void mousePressed()
{
  int tileRow = mouseY / Tile.tileH;
  int tileColumn = mouseX / Tile.tileW;

  if (mouseButton == LEFT)
  {
    terr.changeDirectionTile(tileRow,tileColumn);
    println("Changing direction.");
  } 
  else if (mouseButton == RIGHT) {       
    terr.changeShapeTile(tileRow,tileColumn);
    println("Changing shape.");
  } 
  redraw();
}







