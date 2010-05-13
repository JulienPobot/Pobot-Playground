/**
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

import processing.net.*; 

// client that connects to the port for speed orders
Client myClient;

// two last values retrieved
int val1, val2;  

int[] values1,values2;

void setup() {
  size(640, 240);
  smooth();
  // init history arrays
  values1 = new int[width];
  values2 = new int[width];
  // Connect to the local machine at port 5204.
  // This example will not run if you haven't
  // previously started a server on this port
  myClient = new Client(this, "127.0.0.1", 5204);
}

void draw() {
  if (myClient.available() > 1) {
    if (myClient.read() != 0xFF)
    {
      println("Bad packet !");
      return;
    }
    val1 = myClient.read();
    val2 = myClient.read();
    // convertir en non signé
    if (val1 >= 128)
    {
      val1 -= 256; 
    }
    if (val2 >= 128)
    {
      val2 -= 256;  
    }
    // Shift arrays storing values
    for (int i=0; i<width-1; i++) {
      values1[i] = values1[i+1];
      values2[i] = values2[i+1];
    }
    // Keep new value
    values1[width-1] = val1;
    values2[width-1] = val2;    
    //
    myClient.clear();
  }  
  // Clean screen with white
  background(255);
  // Display zero
  stroke(200);
  line(0,height/2,width,height/2);

  // Display value 1 
  stroke(100,0,0); // color
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(values1[x-1]), 
    width-1-x, height-1-getY(values1[x]));
  }
  // Display value 2 
  stroke(0,100,0); // color
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(values2[x-1]), 
    width-1-x, height-1-getY(values2[x]));
  }
}

int getY(int val) {
  return (int)(val / 250.0f * height) + height/2;
}


