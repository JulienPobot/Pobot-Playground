/**
 * # Copyright (c) 2009 Julien Holtzer
 */

import processing.net.*; 

// client that connects to the port for speed orders
Client myClient;

PrintWriter output;

PFont font;

String received;

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
  myClient = new Client(this, "127.0.0.1", 5202);
  // Font
  font = loadFont("ArialMT-12.vlw");
  textFont(font);

  output = createWriter("easyodo.csv"); 
}

void keyPressed() {
  if (key == 'r') {
    output = createWriter("easyodo.csv"); 

  }
  if (key == 's') {
    output.flush();
    output.close();
  }
}

void draw() {
  if (myClient.available() > 0) {   
    received = myClient.readStringUntil(10); 
    //
    if (received == null) return;
    //
    String[] parts = split(received," ");
    //println("("+parts[0]+","+parts[1]+")");

    output.println(parts[0]+","+parts[1]);

    // Shift arrays storing values
    for (int i=0; i<width-1; i++) {
      values1[i] = values1[i+1];
      values2[i] = values2[i+1];
    }
    // Keep new value
    values1[width-1] = int(parts[0]);
    values2[width-1] = int(parts[1]);    
    //


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
  fill(0, 102, 153);
  text("G: "+values1[width-1],10,20);
  text("D: "+values2[width-1],10,40);
}

int getY(int val) {
  return height/2 + (int)(val / 2000.0f * height);
}







