/*
 * (c) 2008 Sofian Audry (info@sofianaudry.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import processing.serial.*;

Serial port;  // Create object from Serial class
int val;      // Data received from the serial port

// array for storing 'width'-last values
int[][] values;

// received values (init with 0)
int[] vals = {
  0,0};

// coeffs PID
double Kp, Ki, Kd = 0;
double pval;
double error;
double[] errors;

void setup() 
{
  size(640, 480);
  // Open the port that the board is connected to and use the same speed (9600 bps)
  println(Serial.list());
  port = new Serial(this, Serial.list()[1], 9600);  
  // clear and wait for linefeed
  port.clear();
  port.bufferUntil(10); // 10 = linefeed = \n

  values = new int[2][width];  
  errors = new double[width];

  smooth();  
}

int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}


// handle serial data
void serialEvent(Serial p) { 
  String data = p.readStringUntil(10);
  if (data != null) {
    // println(data);
    vals = int(split(data, ' '));
  }
}

int lisse(int val) {
  return lisseMoyenne(val);  
}

int lisseMoyenne(int val) {
  int acc = 0;
  int count = 0;
  for (int i=2; i<30; i++) {
    if (values[0][width-i] != 0 ) {
      acc += values[0][width-i];
      count++; 
    }
  }
  if (count == 0) { 
    return val;
  }
  return acc/count;  
}


/**
 * Algorithme de lissage
 */
int lissePID(int val) {
  //
  Kp = 1;
  Ki = 0.4;
  Kd = 0.0;
  // update errors
  error = val - values[1][width-1];
  println(error);
  for (int i=0; i<width-1; i++) {
    errors[i] = errors[i+1];
  }
  errors[width-1] = error;
  // remember current values
  // compute value
  return (int) (Kp*error + Ki*sommeErrors() + Kd*(error - errors[width-2]));  
}

double sommeErrors() {
  double acc = 0;
  double count = 0;
  //
  for (int i=0; i<width-1; i++) {
    if (errors[i] != 0) {
      acc += errors[i];
      count++;
    }
  } 
  if (count == 0) return 0;
  return acc;
}

void draw()
{
  // each step, forward the line (even if nothing received)
  for (int i=0; i<width-1; i++) {
    values[0][i] = values[0][i+1];
    values[1][i] = values[1][i+1];
  }
  values[0][width-1] = vals[0];
  values[1][width-1] = lisse(vals[0]);

  // clean screen
  background(0);

  // display first value
  stroke(255);
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(values[0][x-1]+512), 
    width-1-x, height-1-getY(values[0][x]+512));
  }

  // display second value
  stroke(255,0,0);
  for (int x=1; x<width; x++) {
    line(width-x,   height-1-getY(values[1][x-1]+512), 
    width-1-x, height-1-getY(values[1][x]+512));
  }

}





