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
int vg, vd; 

// position and heading of the robot
int rx, ry, rt;

// variation since last time
int dr,dt;

int[] values1,values2;

void setup() {
  size(400, 400);
  background(255);
  smooth();
  // init history arrays
  values1 = new int[width];
  values2 = new int[width];
  //
  rx = width/2;
  ry = height/2;
 
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
    vg = myClient.read();
    vd = myClient.read();
    // convertir en non signé
    if (vg >= 128)
    {
      vg -= 256; 
    }
    if (vd >= 128)
    {
      vd -= 256;  
    }
    // Convert speed to position
    dr = (vg+vd)/20;
    dt = (vg-vd);
    rt += dt;    
    rx += dr*cos(rt);
    ry += dr*sin(rt);
    
    // Shift arrays storing values
    for (int i=0; i<width-1; i++) {
      values1[i] = values1[i+1];
      values2[i] = values2[i+1];
    }    
    // Keep new value
    values1[width-1] = vg;
    values2[width-1] = vd;    
    //
    delay(200);
    //
    myClient.clear();
  }  
  // Clean screen with white
  //background(255);
  // Display zero

  // Display value 1 
  ellipse(rx,ry,2,2);
}

int getY(int val) {
  return (int)(val / 250.0f * height) + height/2;
}


