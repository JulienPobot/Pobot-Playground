/**
 * Ce sketch Arduino est destin� � �tre utilis� avec une t�l�commande sur liaison s�rie
 * qui envoit sur la liaison s�rie 3 octets : 
 * - 0xFF
 * - vitesse gauche (entre -100 et +100)
 * - vitesse droite (entre -100 et +100)
 * 
 * En retour, l'Arduino envoie sur la liaison s�rie les valeurs d'odom�trie
 */

#include <Servo.h>
#include <SoftwareServo.h>
#include <MsTimer2.h>

// constantes indiquant la position (en degr�s) neutre (roue immobile)
// � ajuster pour chacune des roues et chacun des robots
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
  // communication s�rie
  Serial.begin(115200);
  Serial.println("> Connection to Pobot Easy granted.");
  // connecter les servomoteurs
  servG.attach(10);
  servD.attach(9);
  servP.attach(11);
  servB.attach(12);
  // arr�ter le robot
  servG.write(NEUTRE_G);
  servD.write(NEUTRE_D);
  servP.write(90);
  servB.write(90);
  // initialiser les vitesses
  vg = 0;
  vd = 0;
  // d�clarer les interruptions externes
  attachInterrupt(0,gauche,CHANGE);
  attachInterrupt(1,droite,CHANGE);  
  // g�rer une interruption r�guli�re
  MsTimer2::set(50, feedback); // 50ms period
  MsTimer2::start();
}

void loop()
{
  // mise � jour des vitesses
  /*if (Serial.available() >= 3)
  {    
    if (Serial.read() != 0xFF) 
    {
      Serial.println("Erreur de r�ception !!");
      Serial.flush();
      return;      
    }*/
    //vg = Serial.read();
    //vd = Serial.read();
    move();
    // r�tablir les valeurs sign�es
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

// Envoyer une trame d'informations sur la liaison s�rie
void feedback()
{
  count_50ms++;
  
  // odom�trie
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

/** Gestion de l'odom�trie **/

// Interruption de l'odom�tre gauche
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

// Interruption de l'odom�tre droit
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

