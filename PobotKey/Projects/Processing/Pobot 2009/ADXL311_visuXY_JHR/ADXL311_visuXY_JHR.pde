/*
 * Visualizator XY ADXL311 + USB/I2C
 * Represents graphical visualization of analog values x and y from accelerometer,
 * but not such as an oscilloscope (time in abscisse) though it is based on the accelerometer of Sofian.
 
 * Uses USB/I2C from Devantech to read analog values
 *
 * (c) 2008 Julien Holtzer (julien@pobot.org)
 * 
 * This project is based on the oscilloscope part of Accrochages
 * See http://accrochages.drone.ws
 * 
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
int val1, val2;      // Data received from the serial port

int[] values1,values2;

void setup() 
{
  size(400, 400);
  // Open the port that the board is connected to and use the same speed (19200 bps)

  println(Serial.list());
  port = new Serial(this, Serial.list()[1], 19200, 'N', 8, 2.0);
  values1 = new int[width];
  values2 = new int[width];
  smooth();
  background(255);
}

void draw()
{
  // la commande de lecture
  byte[] msg = {
    0x5a,0x12,0x00,0x00        }; 
  port.write(msg);

  while(port.available() >= 4)
  {
    val1 = (port.read() << 8) | (port.read());      
    val2 = (port.read() << 8) | (port.read());
    // println("> "+val1+" "+val2);    

    // Shift arrays storing values
    for (int i=0; i<width-1; i++) {
      values1[i] = values1[i+1];
      values2[i] = values2[i+1];
    }
    // Keep new value
    values1[width-1] = val1;
    values2[width-1] = val2;
    // Clean screen with color
    stroke(255);
    fill(255,10);
    rect(0,0,width,height);

    stroke(200,0,0); // color      
    ellipse(getY(val1),getY(val2),10,10);
 
  }
}

int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}
