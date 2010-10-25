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
#include <SoftwareServo.h>
#include <MsTimer2.h>

// constantes indiquant la position (en degrés) neutre (roue immobile)
// à ajuster pour chacune des roues et chacun des robots
#define NEUTRE_G  76
#define NEUTRE_D  77

// nombre de vitesses autour du neutre
#define VAR  5

Servo servG;
Servo servD;

SoftwareServo servP;
SoftwareServo servB;

// vitesses (entre -100 et 100)
int vg;
int vd;

// encodeurs
long countG;
long countD;

int angle = 0;

// la gestion du temps
int count_sec = 0;
int count_50ms = 0;

int flag_envoi = false;

int cpt=0;

void setup()
{
  // communication série
  Serial.begin(115200);
  Serial.println("> Connection to Pobot Easy granted.");
  // connecter les servomoteurs
  servG.attach(10);
  servD.attach(9);
  servP.attach(11);
  servB.attach(12);
  // arrêter le robot
  servG.write(NEUTRE_G);
  servD.write(NEUTRE_D);
  servP.write(90);
  servB.write(90);
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
  /*if (Serial.available() >= 3)
  {    
    if (Serial.read() != 0xFF) 
    {
      Serial.println("Erreur de réception !!");
      Serial.flush();
      return;      
    }*/
    //vg = Serial.read();
    //vd = Serial.read();
    move();
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
  //}
  delay(20);
  SoftwareServo::refresh();
  
  cpt++;
  // traitement de l'envoi d'infos toutes les secondes
  /*if (flag_envoi)
  {
    flag_envoi = false;
    Serial.print(vg,DEC);
    Serial.print(" ");
    Serial.print(vd,DEC);
    Serial.print(" ");
    Serial.print(angle,DEC);
    Serial.print(" ");
    Serial.print(countG,DEC);
    // TODO: les capteurs par exemple
    Serial.println();
  }*/
  Serial.print(countG,DEC);
}

void move() {
  if (countG>-500) {
    vg = -100 + (countG-countD);
    vd = -100 - (countG-countD);
    /*
    if (countG > countD) {
      vg=-99;
    } else {
      vg=-100;
    }
    if (countG < countD) {
      vd=-99;
    } else {
      vd=-100;
    }
    */
  } else {
    vg=0;
    vd=0;
  }
}

/** Gestion de la communication en retour  **/

// Envoyer une trame d'informations sur la liaison série
void feedback()
{
  count_50ms++;
  
  // odométrie
  angle += countD-countG;
  
  
  //
  //countG = 0;
  //countD = 0;

  if (count_50ms >= 20) 
  {
    count_50ms = 0;
    count_sec++;
    flag_envoi = true;
  }

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

