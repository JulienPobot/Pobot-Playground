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

/**
 * First drive of the Pobot Easy : simple tests using a command sent through serial.
 * --> use it to test how your mechanics is 
 */
 
#include <Servo.h>

// use "Calibre Servo" Arduino program to know the value of immobile position for each servo
#define POS_NEUTRE_D  77
#define POS_NEUTRE_G  76

Servo servG;
Servo servD;

void setup()
{
  // serial communication 
  Serial.begin(9600);
  // using "detach" makes the robot not move at all,
  // as servomotors will not be controlled
  servG.detach();
  servD.detach();
 

}

void loop()
{
  // as soon as someithing is received from the serial communication
  if (Serial.available())
  {
    // every command is only one byte long (character)
    char val = Serial.read();
    switch (val)
    {
    case ' ':
      // stop
      Serial.println("> Robot stop");
      servG.detach();
      servD.detach();
      break;
    case '8':
      // en avant
      servG.attach(10);
      servD.attach(9);
      servG.write(POS_NEUTRE_G+10);
      servD.write(POS_NEUTRE_D-10);
      Serial.println("> Robot forward");
      break;
    case '5':
      // arrière
      servG.attach(10);
      servD.attach(9);
      servG.write(POS_NEUTRE_G-10);
      servD.write(POS_NEUTRE_D+10);
      Serial.println("> Robot backward");
      break;
    case '4':
      // à gauche
      servG.attach(10);
      servD.attach(9);
      servG.write(POS_NEUTRE_G+10);
      servD.write(POS_NEUTRE_D+10);      
      Serial.println("> Robot left");
      break;
    case '6':
      // à droite
      servG.attach(10);
      servD.attach(9);
      servG.write(POS_NEUTRE_G-10);
      servD.write(POS_NEUTRE_D-10);      
      Serial.println("> Robot right");
      break;
    default:      
      servG.attach(10);
      servD.attach(9);
      // the received value int value (and not char)
      // is used as the speed (same for both)
      servG.write(POS_NEUTRE_G+val);
      servD.write(POS_NEUTRE_D-val);
      Serial.println("> Robot controlled");
      break;
    }
  }
}

