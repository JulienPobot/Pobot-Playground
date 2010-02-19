/**
 * By Grace Kimb (http://itp.nyu.edu/physcomp/sensors/Code/ADXL202Processing)
 * 2-Axis Accelerometer Skirt Visualization
 * From Tom Igoe's Datalogger : http://itp.nyu.edu/physcomp/sensors/Code/DataloggerMulti
 * and Tom Igoe's TwoByteReceiver : http://itp.nyu.edu/physcomp/sensors/Code/TwoByteReceiver
 */

import processing.serial.*;

Serial port;                         // The serial port

// Whether we've heard from the microcontroller
int xTilt;
int yTilt;
//
int minyTilt = 0; // 185; //variables used to trigger lights
int midyTilt = 512; // 220;
int maxyTilt = 1024; //270;


void setup() {
  size(900, 600);  // Stage size
  noStroke();      
  // Print a list of the serial ports, for debugging purposes:
  println(Serial.list());

  // JHR : specific access to USB-I2C Devantech connector
  port = new Serial(this, Serial.list()[1], 19200, 'N', 8, 2.0);

  background(255, 250, 205);
}

/**
 * Function called continuously to refresh the screen
 * This is completely reworked by JHR to adapt to another controller sending values on serial line.
 */
void draw() {
  // la commande de lecture
  byte[] msg = {
    0x5a,0x12,0x00,0x00                }; 
  port.write(msg);
  while(port.available() >= 4)
  {
    xTilt = (port.read() << 8) | (port.read()) / 2;      
    yTilt = (port.read() << 8) | (port.read()) / 2;

    println("xTilt is " + xTilt + " -- yTilt is " + yTilt);
    background(255, 250, 205);
    drawGraph();
  }
  delay(100);
}

void drawGraph () {
  int valueToGraph = 0;
  valueToGraph = yTilt;
  smooth();
  strokeWeight(0.75);
  fill(0,0,128);
  stroke(0,0,128);

  line(10, 10, (width-10), 10);
  line((width-10), 10, (width-10), height-10);
  line((width-10), height-10, 10, height-10);
  line(10, height-10, 10, 10);

  //Curve takes 4 inputs: curve(x1, y1, x2, y2, x3, y3, x4, y4);
  //x1, y1, z1 	  	int or float: coordinates for the first ancho
  //x2, y2, z2 	  	int or float: coordinates for the first point
  //x3, y3, z3 	  	int or float: coordinates for the second point
  //x4, y4, z4 	  	int or float: coordinates for the second anchor

  line((width/2 - 75), 50, (width/2 - 75), 40); //draw waistband of skirt
  line((width/2 + 75), 50, (width/2 + 75), 40);
  curve((width/2), 0, (width/2 - 75), 40, (width/2 + 75), 40, (width/2), 0); 
  curve((width/2), 0, (width/2 - 75), 50, (width/2 + 75), 50, (width/2), 0);

  curve((width/2 -75), (height/2 + 100), (width/2 + 75), 50, ((width/2+valueToGraph)), (height-100), (width/2 - 75), (height/2 + 100)); //draw skirt, second x value is determined by the amount that y tilts
  curve((width/2 + 75), (height/2 + 100), (width/2 -75), 50, ((width/2-valueToGraph)), (height-100), (width/2 + 75), (height/2 + 100)); 
  curve((width/2), (height-300), ((width/2+valueToGraph)), (height-100), ((width/2-valueToGraph)), (height-100), (width/2), (height-300));

  strokeWeight(.75); // draw lights
  fill(255, 250, 205);
  stroke(0,0,128);
  ellipse((width/2), (height - 150), 20, 20);
  ellipse((width/2), (height - 200), 20, 20);
  ellipse((width/2), (height - 250), 20, 20);
  ellipse((width/2), (height - 150), 15, 15);
  ellipse((width/2), (height - 200), 15, 15);
  ellipse((width/2), (height - 250), 15, 15);

  if((valueToGraph > minyTilt)&&(valueToGraph<=midyTilt)){
    fill(129,229, 17);
    stroke(0,0,128);
    ellipse((width/2), (height - 150), 15, 15);
    fill(255, 250, 205);
    stroke(0,0,128);
    ellipse((width/2), (height - 200), 15, 15);
    ellipse((width/2), (height - 250), 15, 15);
  }

  if((valueToGraph > midyTilt)&&(valueToGraph<=maxyTilt)){
    fill(129,229, 17);
    stroke(0,0,128);
    ellipse((width/2), (height - 150), 15, 15);
    ellipse((width/2), (height - 200), 15, 15);
    fill(255, 250, 205);
    stroke(0,0,128);
    ellipse((width/2), (height - 250), 15, 15);
  }

  if(valueToGraph > maxyTilt){
    fill(129,229, 17);
    stroke(0,0,128);
    ellipse((width/2), (height - 150), 15, 15);
    ellipse((width/2), (height - 200), 15, 15);
    ellipse((width/2), (height - 250), 15, 15);
  }
}
