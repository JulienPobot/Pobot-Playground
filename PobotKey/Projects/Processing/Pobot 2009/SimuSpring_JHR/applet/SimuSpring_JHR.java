import processing.core.*; 
import processing.xml.*; 

import java.applet.*; 
import java.awt.*; 
import java.awt.image.*; 
import java.awt.event.*; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class SimuSpring_JHR extends PApplet {

/**
 * Spring. 
 * 
 * Click, drag, and release the horizontal bar to start the spring. 
 * 
 * Updated 1 September 2001
 */

// Spring drawing constants for top bar
int s_height = 16;     // Height
int top = 10;         // Left position
int bottom = 90;       // Right position
int max = 490;         // Maximum Y value
int min = 10;          // Minimum Y value
boolean over = false;  // If mouse over
boolean move = false;  // If mouse down and over

// Spring simulation constants
float M = 0.8f;   // Mass
float K = 0.2f;   // Spring constant
float D = 0.92f;  // Damping

// Objective
float R = 250 - s_height/2;    // Rest position

// Spring simulation variables
float ps = 60.0f; // Position
float vs = 0.0f;  // Velocity
float as = 0;    // Acceleration
float f = 0;     // Force


public void setup() 
{
  size(500, 100);
  rectMode(CORNERS);
  noStroke();
}

public void draw() 
{
  background(102);
  updateSpring();
  drawSpring();
}

public void drawSpring() 
{
  // Draw base
  fill(0.2f);
  rect(ps, height/2 - top,  R, height/2 + top);

  // Set color and draw top bar
  if(over || move) { 
    fill(255);
  } 
  else { 
    fill(204);
  }
  rect(ps-s_height/2, top, ps + s_height, bottom);
}


public void updateSpring()
{
  // Update the spring position
  if(!move) {
   
    f = -K * (ps - R);    // f=-ky
    as = f / M;           // Set the acceleration, f=ma == a=f/m
    vs = D * (vs + as);   // Set the velocity
    ps = ps + vs;         // Updated position
   
  }
  if(abs(vs) < 0.1f) {
    vs = 0.0f;
  }

  // Test if mouse is over the top bar
  if(mouseY > top && mouseY < bottom && mouseX > ps && mouseX < ps + s_height) {
    over = true;
  } 
  else {
    over = false;
  }

  // Set and constrain the position of top bar
  if(move) {
    ps = mouseX - s_height/2;
    if (ps < min) { 
      ps = min; 
    } 
    if (ps > max) { 
      ps = max; 
    }
  }
}

public void mousePressed() {
  if(over) {
    move = true;
  }
}

public void mouseReleased()
{
  move = false;
}

  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#d4d0c8", "SimuSpring_JHR" });
  }
}
