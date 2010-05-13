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

/*  Xbee Signal Strength Reader
  Language: Processing

 Reads a packet from an Xbee radio and parses it.  The packet
 should be 22 bytes long. It should be made up of the following:
 byte 1:     0x7E, the start byte value
 byte 2-3:   packet size, a 2-byte value  (not used here)
 byte 4:     API identifier value, a code that says what this response is (not used here)
 byte 5-6:   Sender's address
 byte 7:     RSSI, Received Signal Strength Indicator (not used here)
 byte 8:     Broadcast options (not used here)
 byte 9:     Number of samples to follow
 byte 10-11: Active channels indicator (not used here)
 byte 12-21: 5 10-bit values, each ADC samples from the sender 

 Created 3 Mar. 2007
 by Tom Igoe
 */
import processing.serial.*;

Serial xbee;                    // input serial port from the Xbee Radio
int[] packet = new int[22];     // with 5 samples, the Xbee packet is 22 bytes long
int byteCounter;                // keeps track of where you are in the packet
int rssi = 0;                   // received signal strength
int address = 0;                // the sending Xbee's address

Serial myPort;                  // The serial port

int fontSize = 18;              // size of the text on the screen
int lastReading = 0;               // value of the previous incoming byte

void setup () {
  size(400, 300);        // window size

  // create a font with the second font available to the system:
  PFont myFont = createFont(PFont.list()[2], fontSize);
  textFont(myFont);

  // get a list of the serial ports:
  println(Serial.list());
  // open the serial port attached to your Xbee radio:
  xbee = new Serial(this, Serial.list()[0], 9600);
}

void draw() {
  // if you have new data and it's valid (>0), graph it:
  if ((rssi > 0 ) && (rssi != lastReading)) {
    // set the background:
    background(0);
    // set the bar height and width:
    // rssi should range from 92 to 0:
    int rectHeight = 92 - rssi;
    int rectWidth = 50;
    // draw the rect:
    stroke(23, 127, 255);
    fill (23, 127, 255);
    rect(width/2 - rectWidth, height-rectHeight, rectWidth, height);
    // write the number:
    text("Xbee Radio Signal Strength test", 10, 20);
    text("From: " + hex(address), 10, 40);

    text ("RSSI: -" + rssi + " dB", 10, 60);
    // save the current byte for next read:
    lastReading = rssi;
  }
}

void serialEvent(Serial xbee) {
  // read a byte from the port:
  int thisByte = xbee.read();
  // if the byte = 0x7E, the value of a start byte, you have a new packet:
  if (thisByte == 0x7E) {   // start byte
    // parse the previous packet if there's data:
    if (packet[2] > 0) {
      parseData(packet);
    }
    // reset the byte counter:
    byteCounter = 0;
  }
  // put the current byte into the packet at the current position:
  packet[byteCounter] = thisByte;
  //  increment the byte counter:
  byteCounter++;
}

/*
 Once you've got a packet, you need to extract the useful data.
 This method gets the address of the sender and RSSI.
 */
void parseData(int[] thisPacket) {

  // read the address. It's a two-byte value, so you
  // add the two bytes as follows:
  address = thisPacket[5] + thisPacket[4] * 256;
  // get RSSI:
  rssi = thisPacket[6];
}
