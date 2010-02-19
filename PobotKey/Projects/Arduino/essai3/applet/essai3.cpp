#include <Wire.h>

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include <MsTimer2.h>

#define DIRECTION 0
#define ENABLE 1
#define PULSE 2

#include "WProgram.h"
void setup();
void loop();
inline int moyenneCapteur(int val);
inline void AfficheValeur(int valAffiche);
inline int zone( int capteur);
inline void feedback();
inline void creneau_pulse();
inline void stop();
inline void start();
inline void envoi_commande(bool MoteurG ,bool MoteurD, int vitesse);
inline void deplacement(bool MoteurG ,bool MoteurD, int vitesse );
inline void repere_robot();
inline void calculimpulsion(bool MoteurG, bool MoteurD);
inline void accelerometre();
inline void increment_macommande_seuil();
inline void decrement_macommande_seuil();
inline void   initialisation_champ();
int motors[2][3]={
  {
    7,5,2                 }
  ,{
    6,4,3                   }
};

typedef struct Commande{
  unsigned int Seuil;
  bool MoteurGauche;
  bool MoteurDroit;
}
Commande;

Commande MaCommande;
Commande ReceptionConsigne;
Commande EtatPrecedent;


unsigned  periodeMoteur = 1; // ms
unsigned capteurg;
unsigned zoneY;

unsigned  ticks=0;
int  NumberPulse=0;
int NumberPulseDroit=0;
int NumberPulseGauche=0;
unsigned long cptTime=0;

unsigned int cptD=0;
unsigned int cptI=0;
unsigned int fff=0;
unsigned int ffr=0;
unsigned int ffd=0;
unsigned int ffg=0;

long delta=0;
long dTeta=0;
int dRight=0;
int dLeft=0;
int dx=0;
int dy=0;
int X=0;
int Y=0;
long Teta=0;
long r=0;
int dmoyen=0;
unsigned int cptTime2=0;
bool Affiche  ;
int X2=0;
float pi = 3.141592653589;
float DistanceParPulse=683; /*683 micron pour un pulse*/



bool zone_deceleration;
bool zone_acceleration;



void setup()
{

  for(int moteur=0;moteur<=1;moteur++){ // moteur= 0 equivaut \u00e0 moeur gauche, se posittioner derriere le le robot
    pinMode(motors[moteur][PULSE],OUTPUT);
    pinMode(motors[moteur][ENABLE],OUTPUT);
    pinMode(motors[moteur][DIRECTION],OUTPUT);

    digitalWrite(motors[moteur][ENABLE],HIGH);
    digitalWrite(motors[moteur][DIRECTION],HIGH);
  }
  initialisation_champ();
 
  MsTimer2::set(periodeMoteur, feedback); 
  MsTimer2::start();


  Serial.begin(9600);
}



void loop()
{
  if(MaCommande.MoteurDroit)
  ffr=1;
  else
  ffr=0;
  
  
  if(MaCommande.MoteurGauche)
  fff=1;
  else
  fff=0;
  
  if(ReceptionConsigne.MoteurDroit)
  ffd=1;
  else
  ffd=0;
  

  if(ReceptionConsigne.MoteurGauche)
  ffg=1;
  else
  ffg=0;
  
  

  // Gestion de la r\u00e9ception de commandes
  if (Serial.available() >= 3) 
  {
    boolean gauche = (Serial.read()-48 == 1);
    boolean droite = (Serial.read()-48 == 1);
    int sp = Serial.read()-48;

    Serial.print("Cg: ");
    Serial.print(gauche?1:0);
    Serial.print(" Cd: ");
    Serial.print(droite?1:0);
    Serial.print(" Sp: ");
    Serial.print(sp);
    Serial.println();
    envoi_commande(gauche,droite,sp);
  }  
  
  delay(5);
  
  /*
   Serial.print("C:");
   AfficheValeur(ffg); 
   Serial.print(",");
   AfficheValeur(fff); 
   Serial.print("\t");
   AfficheValeur(MaCommande.Seuil); 
   Serial.print("\t");
   AfficheValeur(ReceptionConsigne.Seuil); 
   Serial.print("\t");
   Serial.print("\t");
   AfficheValeur(ffr); 
   Serial.print("\t");
   AfficheValeur(ffd); 
  */
  
  /*capteurg=moyenneCapteur(analogRead(0));
   //Serial.print("\t");
   //AfficheValeur(capteurg);
   zoneY=zone(capteurg);
   */

}

inline int moyenneCapteur(int val){
  long somme=0;
  for(int numlecture=0;numlecture<20;numlecture++){
    somme+=val; 
  }
  return(somme/20);
}


inline void AfficheValeur(int valAffiche){
  Serial.print(valAffiche,DEC);
}  



inline int zone( int capteur){
  if(capteurg>110){
    if(capteurg>140)
      return(1);
    else
      return(2);
  }
  else
    return(3);
}

inline void feedback()
{  if(ReceptionConsigne.Seuil!=100){
  ticks++;
  cptTime++;
  cptTime2++;
  if(ticks>=MaCommande.Seuil){
    NumberPulse++;
    ticks=0;
    creneau_pulse();
    accelerometre();
    calculimpulsion(MaCommande.MoteurGauche, MaCommande.MoteurDroit);
    deplacement(MaCommande.MoteurGauche, MaCommande.MoteurDroit, MaCommande.Seuil);
  }

  if(cptTime2%MaCommande.Seuil==0 && cptTime2>=20){
    repere_robot();
    cptTime2=0;
    NumberPulseGauche=0;
    NumberPulseDroit=0;
  }
 }

}

inline void creneau_pulse(){
  digitalWrite(motors[0][PULSE],HIGH);
  digitalWrite(motors[1][PULSE],HIGH);
  delayMicroseconds(200);
  digitalWrite(motors[0][PULSE],LOW);
  digitalWrite(motors[1][PULSE],LOW);
}

inline void stop(){
  digitalWrite(motors[0][ENABLE],LOW);
  digitalWrite(motors[1][ENABLE],LOW);
}

inline void start(){
  digitalWrite(motors[0][ENABLE],HIGH);
  digitalWrite(motors[1][ENABLE],HIGH);
}


inline void envoi_commande(bool MoteurG ,bool MoteurD, int vitesse){
  ReceptionConsigne.MoteurGauche=MoteurG;
  ReceptionConsigne.MoteurDroit=MoteurD;
  ReceptionConsigne.Seuil=vitesse; 
}



inline void deplacement(bool MoteurG ,bool MoteurD, int vitesse ){


  if(vitesse==0){    // If vitesse=0 the system stop
    stop();
  }
  else{
    start();      // we start
    if(MoteurG){
      if(MoteurD){
        digitalWrite(motors[0][DIRECTION],HIGH);
        digitalWrite(motors[1][DIRECTION],HIGH);
      } 
      else{
        digitalWrite(motors[0][DIRECTION],HIGH);
        digitalWrite(motors[1][DIRECTION],LOW);
      }
    }
    else{
      if(MoteurD){
        digitalWrite(motors[0][DIRECTION],LOW);
        digitalWrite(motors[1][DIRECTION],HIGH);
      } 
      else{
        digitalWrite(motors[0][DIRECTION],LOW);
        digitalWrite(motors[1][DIRECTION],LOW);
      }
    }
  }
}


inline void repere_robot(){
  /* 
   On travaille en micron voir meme nanometre
   Test de 10s
   Je connais nombre de pulse \u00e0 gauche et nombre de pulse \u00e0 droite
   Je sais que pour 2488 pulse il parcour 170 cm soit 1700mm.
   Qui fait une moyenne de 0.683 mm par pulse soit 683 microns par pulse
   donc,
   dLeft=NumberPulseGauche*683  dlefte distance du moteur gauche en 20 ms*/
  long entraxe=227; /* entraxe 20 cm soit 200 mm */


  dLeft=NumberPulseGauche*(DistanceParPulse); /* sur 20 ms: si on a v=4, on aura 5 pulses le robot avancera de 5*683=3415 micrometre...*/
  dRight=NumberPulseDroit*(DistanceParPulse);

  /* Le deplacement moyen du centre de rotation de la roue qui se situe au milieu de l'entraxe des 2  roues est \u00e9gale \u00e0 la moyenne de d\u00e9placement des 2 roues.*/
  dmoyen=(dRight+dLeft)/2;

  /* On sait que 2*Pi*R=Perimetre d un cercle, on supose dTeta tr\u00e9s petit, donc l angle dTeta= delta/entraxe, delta est un delta du perimetre du cercle.*/

  delta=(dRight-dLeft); /* pour t=20 ms l ecart delata du robot est de 6380 micrometre */

  dTeta=(delta*1000)/entraxe;
  /* (6830 *1000) um/ 200 mm= 34150 *10^-3 mradiant, soit 34.150 mradiant soit 
   0.034150 rad.
   On veut garder le 6830 pour etre precis et travailler en int donc on le multipli par 1000 et on divise par la taille de l entraxe en laissant des mm pour la precision.
   On reajustera le tout \u00e0 la  lecture des donn\u00e9es.
   chaque 20 ms dTeta sera de 34150 microradiant. Ne pas oubli\u00e9 de mettre dteta et teta en long et non pas int*/



  Teta+=dTeta;
  if(Teta>=(2*pi*1000000))/* 2 pi= 6.283185 rad*/
    //Teta=0;

    dx=cos(dTeta)*dmoyen;
  dy=sin(dTeta)*dmoyen;
  r+=dmoyen;  /* r est calcul\u00e9 en micron on l affiche en mm*/




}

inline void calculimpulsion(bool MoteurG, bool MoteurD){
  if(MoteurD){
    NumberPulseDroit++;
    if(MoteurG)
      NumberPulseGauche++;          
    else
      NumberPulseGauche--;         
  }
  else{
    NumberPulseDroit--;
    if(MoteurG)
      NumberPulseGauche++;          
    else
      NumberPulseGauche--; 
  }
}



inline void accelerometre(){
        
  if(MaCommande.Seuil<=10){
    if(zone_acceleration || zone_deceleration ){
          if(  zone_deceleration ){                           
              increment_macommande_seuil();
              
              if(MaCommande.Seuil==10){
                  zone_deceleration=false;
                  zone_acceleration=true;  
              }
         }else{ 
          MaCommande.MoteurGauche=ReceptionConsigne.MoteurGauche;     
          MaCommande.MoteurDroit=ReceptionConsigne.MoteurDroit;   
              decrement_macommande_seuil();
                zone_acceleration=false;      
        }    
   EtatPrecedent=MaCommande;
       
   }else {
              if(EtatPrecedent.MoteurGauche!=ReceptionConsigne.MoteurGauche || EtatPrecedent.MoteurDroit!=ReceptionConsigne.MoteurDroit ){
                zone_deceleration=true;
                MaCommande=EtatPrecedent;                    
              }
              else{
                     if(MaCommande.Seuil!=ReceptionConsigne.Seuil){
                          if(MaCommande.Seuil>ReceptionConsigne.Seuil )
                          decrement_macommande_seuil();
                          else
                          increment_macommande_seuil();
                          
              MaCommande.MoteurGauche=ReceptionConsigne.MoteurGauche;     
              MaCommande.MoteurDroit=ReceptionConsigne.MoteurDroit; 
              EtatPrecedent=MaCommande;
                     }else{            
                          MaCommande=ReceptionConsigne;
                          EtatPrecedent=ReceptionConsigne;
                      }  
              }
         }
  }
}  

inline void increment_macommande_seuil(){
     cptI++;
     if(cptI>=(1000/MaCommande.Seuil)){
       MaCommande.Seuil++;
       cptI=0;
     }
}

inline void decrement_macommande_seuil(){
     cptD++;
     if(cptD>=(1000/MaCommande.Seuil)){
       MaCommande.Seuil--;
       cptD=0;
     }
}

inline void   initialisation_champ(){
  MaCommande.Seuil=10;
  MaCommande.MoteurGauche=true;
  MaCommande.MoteurDroit=true;
 
  ReceptionConsigne.Seuil=100;
  ReceptionConsigne.MoteurGauche=true;
  ReceptionConsigne.MoteurDroit=true;
 
  EtatPrecedent.Seuil=10;
  EtatPrecedent.MoteurGauche=true;
  EtatPrecedent.MoteurDroit=true;
  
  zone_deceleration=false;
  zone_acceleration=false;
}


int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

