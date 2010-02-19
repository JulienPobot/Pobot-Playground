/**
 * # Copyright (c) 2009 Julien Holtzer
 * #
 * # Permission is hereby granted, free of charge, to any person obtaining a copy
 * # of this software and associated documentation files (the "Software"), to deal
 * # in the Software without restriction, including without limitation the rights
 * # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * # copies of the Software, and to permit persons to whom the Software is
 * # furnished to do so, subject to the following conditions:
 * #
 * # The above copyright notice and this permission notice shall be included in
 * # all copies or substantial portions of the Software.
 * #
 * # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * # THE SOFTWARE.
 */

/**
 * Ce sketch Arduino est destin\u00e9 \u00e0 \u00eatre utilis\u00e9 avec une t\u00e9l\u00e9commande sur liaison s\u00e9rie
 * qui envoit sur la liaison s\u00e9rie 3 octets : 
 * - 0xFF
 * - vitesse gauche (entre -100 et +100)
 * - vitesse droite (entre -100 et +100)
 * 
 * En retour, l'Arduino envoie sur la liaison s\u00e9rie les valeurs d'odom\u00e9trie
 */

#include <Servo.h>
#include <MsTimer2.h>

// constantes indiquant la position (en degr\u00e9s) neutre (roue immobile)
// \u00e0 ajuster pour chacune des roues et chacun des robots
#define NEUTRE_G  74
#define NEUTRE_D  77

// nombre de vitesses autour du neutre
#define VAR  5

#include "WProgram.h"
void setup();
void loop();
void feedback();
void gauche();
void droite();
Servo servG;
Servo servD;

// vitesses (entre -100 et 100)
int vg;
int vd;

// encodeurs
int countG;
int countD;

int angle = 0;
int x = 0;
int y = 0;

// la gestion du temps
int count_sec = 0;
int count_50ms = 0;

void setup()
{
  // communication s\u00e9rie
  Serial.begin(115200);
  Serial.println("> Connection to Pobot Easy granted.");
  // connecter les servomoteurs
  servG.attach(10);
  servD.attach(9);
  // arr\u00eater le robot
  servG.write(NEUTRE_G);
  servD.write(NEUTRE_D);
  // initialiser les vitesses
  vg = 0;
  vd = 0;
  // d\u00e9clarer les interruptions externes
  attachInterrupt(0,gauche,CHANGE);
  attachInterrupt(1,droite,CHANGE);  
  // g\u00e9rer une interruption r\u00e9guli\u00e8re
  MsTimer2::set(50, feedback); // 50ms period
  MsTimer2::start();
}

void loop()
{
  // mise \u00e0 jour des vitesses
  if (Serial.available() >= 3)
  {    
    if (Serial.read() != 0xFF) 
    {
      Serial.println("Erreur de r\u00e9ception !!");
      Serial.flush();
      return;      
    }
    vg = Serial.read();
    vd = Serial.read();
    // r\u00e9tablir les valeurs sign\u00e9es
    if (vg >= 128) {
      vg = vg-255;      
    }   
    if (vd >= 128) {
      vd = vd-255;  
    }
    // commander les servomoteurs  
    servG.write(NEUTRE_G+vg*VAR/100.0);
    servD.write(NEUTRE_D-vd*VAR/100.0);
    Serial.flush();
  }

  delay(20); 

}

/** Gestion de la communication en retour  **/

// Envoyer une trame d'informations sur la liaison s\u00e9rie
void feedback()
{
  count_50ms++;

  Serial.print(countG,DEC);
  Serial.print(" ");
  Serial.print(countD,DEC);
  Serial.print(" ");
  Serial.println();

  /*
  // odom\u00e9trie
   angle += countD-countG;
   
   // 
   int dist = (countG+countD;
   
   
   
   //
   countG = 0;
   countD = 0;
   
   // envoi toutes les 50 ms
   flag_envoi = true;
   
   if (count_50ms >= 20) 
   {
   count_50ms = 0;
   count_sec++;
   //flag_envoi = true;
   }
   
   */

}

/** Gestion de l'odom\u00e9trie **/

// Interruption de l'odom\u00e8tre gauche
void gauche()
{
  // Gestion du sens de rotation
  if (vg <0) {
    countG--;
  } 
  else {
    countG++; 
  } 
}

// Interruption de l'odom\u00e8tre droit
void droite()
{
  // Gestion du sens de rotation
  if (vd <0) {
    countD--;   
  } 
  else {
    countD++;
  }
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

