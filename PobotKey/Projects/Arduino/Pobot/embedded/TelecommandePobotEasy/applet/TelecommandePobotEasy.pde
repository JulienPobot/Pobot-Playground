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
 * Ce sketch Arduino est destiné à être utilisé avec une télécommande sur liaison série
 * qui envoit sur la liaison série 3 octets : 
 * - 0xFF
 * - vitesse gauche (entre -100 et +100)
 * - vitesse droite (entre -100 et +100)
 * 
 * En retour, l'Arduino envoie sur la liaison série les valeurs d'odométrie
 */

#include <Servo.h>
#include <MsTimer2.h>

// constantes indiquant la position (en degrés) neutre (roue immobile)
// à ajuster pour chacune des roues et chacun des robots
#define NEUTRE_G  74
#define NEUTRE_D  77

// nombre de vitesses autour du neutre
#define VAR  5

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
  // communication série
  Serial.begin(115200);
  Serial.println("> Connection to Pobot Easy granted.");
  // connecter les servomoteurs
  servG.attach(10);
  servD.attach(9);
  // arrêter le robot
  servG.write(NEUTRE_G);
  servD.write(NEUTRE_D);
  // initialiser les vitesses
  vg = 0;
  vd = 0;
  // déclarer les interruptions externes
  attachInterrupt(0,gauche,CHANGE);
  attachInterrupt(1,droite,CHANGE);  
  // gérer une interruption régulière
  MsTimer2::set(50, feedback); // 50ms period
  MsTimer2::start();
}

void loop()
{
  // mise à jour des vitesses
  if (Serial.available() >= 3)
  {    
    if (Serial.read() != 0xFF) 
    {
      Serial.println("Erreur de réception !!");
      Serial.flush();
      return;      
    }
    vg = Serial.read();
    vd = Serial.read();
    // rétablir les valeurs signées
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

// Envoyer une trame d'informations sur la liaison série
void feedback()
{
  count_50ms++;

  Serial.print(countG,DEC);
  Serial.print(" ");
  Serial.print(countD,DEC);
  Serial.print(" ");
  Serial.println();

  /*
  // odométrie
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

/** Gestion de l'odométrie **/

// Interruption de l'odomètre gauche
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

// Interruption de l'odomètre droit
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

