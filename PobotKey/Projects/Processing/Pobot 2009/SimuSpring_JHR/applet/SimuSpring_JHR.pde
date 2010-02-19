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
float M = 0.8;   // Mass
float K = 0.2;   // Spring constant
float D = 0.92;  // Damping

// Objective
float R = 250 - s_height/2;    // Rest position

// Spring simulation variables
float ps = 60.0; // Position
float vs = 0.0;  // Velocity
float as = 0;    // Acceleration
float f = 0;     // Force


void setup() 
{
  size(500, 100);
  rectMode(CORNERS);
  noStroke();
}

void draw() 
{
  background(102);
  updateSpring();
  drawSpring();
}

void drawSpring() 
{
  // Draw base
  fill(0.2);
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


void updateSpring()
{
  // Update the spring position
  if(!move) {
   
    f = -K * (ps - R);    // f=-ky
    as = f / M;           // Set the acceleration, f=ma == a=f/m
    vs = D * (vs + as);   // Set the velocity
    ps = ps + vs;         // Updated position
   
  }
  if(abs(vs) < 0.1) {
    vs = 0.0;
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

void mousePressed() {
  if(over) {
    move = true;
  }
}

void mouseReleased()
{
  move = false;
}
