/////////////////////////////////////////////////////////////////////////////////////
// REALISE PAR : LOÏC BEAUFILS
//
// VERSION IHM 2.0
//
// MODIFIE LE : 16/09/2011
//
/////////////////////////////////////////////////////////////////////////////////////

// LIBRAIRIE
#include <Servo.h>


//DECLARATION
int ledPin =  13;
int ledPin2 =  12;
int ledPin3 =  11;
int ledPin4 =  10;
int ledPin5 =  9;
int ledPin6 =  8;
int ledPin7 =  7;
int ledPin8 =  6;
int ledPin9 =  5;
int ledPin10 =  4;
int ledPin11 =  3;
int ledPin12 =  2;
int i=0;
int j=0;
int m=0;
int z=0;
int RESU;
int analog=0;
int analog1=0;
int analog2=0;
int analog3=0;
int analog4=0;
int analog5=0;

int sensorPin = 0;
int tension;
int tension1;
int tension2;
int tension3;
int tension4;
int tension5;
char* val2;
char* val3;
char* val4;
char* val5;
char* val6;
char* val7;
char* val8;
char* val9;
char* val10;
char* val11;
char* val12;
char* val13;
int buttonState13 = 0;
int buttonState11 = 0;
int in13=0;
int in11=0;



const int ENTREE13 = 13;
const int ENTREE12 = 12;
const int ENTREE11 = 11;
const int ENTREE10 = 10;
const int ENTREE9 = 9;
const int ENTREE8 = 8;
const int ENTREE7 = 7;
const int ENTREE6 = 6;
const int ENTREE5 = 5;
const int ENTREE4 = 4;
const int ENTREE3 = 3;
const int ENTREE2 = 2;
Servo mon_entree13;
Servo mon_entree12;
Servo mon_entree11;
Servo mon_entree10;
Servo mon_entree9;
Servo mon_entree8;
Servo mon_entree7;
Servo mon_entree6;
Servo mon_entree5;
Servo mon_entree4;
Servo mon_entree3;
Servo mon_entree2;
char Res[10];
char ReceptionOctet;
char ReceptionNombre;

// SETUP D'INTITIALISATION
void setup()   
{                
  Serial.begin(115200);  
}

// LOOP : BOUCLE DU PROGRAMME
void loop()                     
{
  //SI CARACTERE DANS LA FILE D'ATTENTE
  if (Serial.available()>0) {

  //LECTURE DU MESSAGE
  while (Serial.available()>0) { 
    if(i<=5)
    {
      ReceptionOctet=Serial.read(); 
      Res[i]=ReceptionOctet;    
      i++;
    }
    delay(1); // pause pour laisser le temps au Serial.available de recevoir les caractères
  } // fin while
  
// MESSAGE $AD001 PERMET D'ACTIVER EN 5V LA SORTIE 13.
if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='D')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree13.detach();
    pinMode(ledPin, OUTPUT); 
    digitalWrite(ledPin, HIGH);   // set the LED on
    val13="$A=D001\n";
    Serial.print("$A=D001\n");    
  }
// MESSAGE $AD000 PERMET DE DESACTIVER EN 0V LA SORTIE 13.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='D')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree13.detach();
    pinMode(ledPin, OUTPUT); 
    digitalWrite(ledPin, LOW);    // set the LED off
    val13="$A=D000\n";
    Serial.print("$A=D000\n");
  }
// MESSAGE $AC001 PERMET D'ACTIVER EN 5V LA SORTIE 12.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='C')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree12.detach();
    pinMode(ledPin2, OUTPUT); 
    digitalWrite(ledPin2, HIGH);   // set the LED on
    val12="$A=C001\n";
    Serial.print("$A=C001\n");

  }
// MESSAGE $AC000 PERMET DE DESACTIVER EN 0V LA SORTIE 12.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='C')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree12.detach();
    pinMode(ledPin2, OUTPUT); 
    digitalWrite(ledPin2, LOW);    // set the LED off
    val12="$A=C000\n";
    Serial.print("$A=C000\n");
  }
// MESSAGE $AB001 PERMET D'ACTIVER EN 5V LA SORTIE 11.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='B')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree11.detach();
    pinMode(ledPin3, OUTPUT); 
    digitalWrite(ledPin3, HIGH);   // set the LED on
    val11="$A=B001\n";
    Serial.print("$A=B001\n");

  }
// MESSAGE $AB000 PERMET DE DESACTIVER EN 0V LA SORTIE 11.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='B')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree11.detach();
    pinMode(ledPin3, OUTPUT); 
    digitalWrite(ledPin3, LOW);    // set the LED off
    val11="$A=B000\n";
    Serial.print("$A=B000\n");
  }
// MESSAGE $AA001 PERMET D'ACTIVER EN 5V LA SORTIE 10.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='A')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree10.detach();
    pinMode(ledPin4, OUTPUT); 
    digitalWrite(ledPin4, HIGH);   // set the LED on
    val10="$A=A001\n";
    Serial.print("$A=A001\n");

  }
// MESSAGE $AA000 PERMET DE DESACTIVER EN 0V LA SORTIE 10.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='A')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree10.detach();
    pinMode(ledPin4, OUTPUT); 
    digitalWrite(ledPin4, LOW);    // set the LED off
    val10="$A=A000\n";
    Serial.print("$A=A000\n");
  }
// MESSAGE $A9001 PERMET D'ACTIVER EN 5V LA SORTIE 9.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='9')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree9.detach();
    pinMode(ledPin5, OUTPUT); 
    digitalWrite(ledPin5, HIGH);   // set the LED on
    val9="$A=9001\n";
    Serial.print("$A=9001\n");

  }
// MESSAGE $A9000 PERMET DE DESACTIVER EN 0V LA SORTIE 9.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='9')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree9.detach();
    pinMode(ledPin5, OUTPUT); 
    digitalWrite(ledPin5, LOW);    // set the LED off
    val9="$A=9000\n";
    Serial.print("$A=9000\n");
  }
// MESSAGE $A8001 PERMET D'ACTIVER EN 5V LA SORTIE 8.
if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='8')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree8.detach();
    pinMode(ledPin6, OUTPUT); 
    digitalWrite(ledPin6, HIGH);   // set the LED on
    val8="$A=8001\n";
    Serial.print("$A=8001\n");

  }
// MESSAGE $A8000 PERMET DE DESACTIVER EN 0V LA SORTIE 8.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='8')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree8.detach();
    pinMode(ledPin6, OUTPUT); 
    digitalWrite(ledPin6, LOW);    // set the LED off
    val8="$A=8000\n";
    Serial.print("$A=8000\n");
  }
// MESSAGE $A7001 PERMET D'ACTIVER EN 5V LA SORTIE 7.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='7')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree7.detach();
    pinMode(ledPin7, OUTPUT); 
    digitalWrite(ledPin7, HIGH);   // set the LED on
    val7="$A=7001\n";
    Serial.print("$A=7001\n");

  }
// MESSAGE $A7000 PERMET DE DESACTIVER EN 0V LA SORTIE 7.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='7')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree7.detach();
    pinMode(ledPin7, OUTPUT); 
    digitalWrite(ledPin7, LOW);    // set the LED off
    val7="$A=7000\n";
    Serial.print("$A=7000\n");
  }
// MESSAGE $A6001 PERMET D'ACTIVER EN 5V LA SORTIE 6.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='6')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree6.detach();
    pinMode(ledPin8, OUTPUT); 
    digitalWrite(ledPin8, HIGH);   // set the LED on
    val6="$A=6001\n";
    Serial.print("$A=6001\n");

  }
// MESSAGE $A6000 PERMET DE DESACTIVER EN 0V LA SORTIE 6.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='6')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree6.detach();
    pinMode(ledPin8, OUTPUT); 
    digitalWrite(ledPin8, LOW);    // set the LED off
    val6="$A=6000\n";
    Serial.print("$A=6000\n");
  }
// MESSAGE $A5001 PERMET D'ACTIVER EN 5V LA SORTIE 5.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='5')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree5.detach();
    pinMode(ledPin9, OUTPUT); 
    digitalWrite(ledPin9, HIGH);   // set the LED on
    val5="$A=5001\n";
    Serial.print("$A=5001\n");

  }
// MESSAGE $A5000 PERMET DE DESACTIVER EN 0V LA SORTIE 5.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='5')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree5.detach();
    pinMode(ledPin9, OUTPUT); 
    digitalWrite(ledPin9, LOW);    // set the LED off
    val5="$A=5000\n";
    Serial.print("$A=5000\n");
  }
// MESSAGE $A4001 PERMET D'ACTIVER EN 5V LA SORTIE 4.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='4')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree4.detach();
    pinMode(ledPin10, OUTPUT); 
    digitalWrite(ledPin10, HIGH);   // set the LED on
    val4="$A=4001\n";
    Serial.print("$A=4001\n");

  }
// MESSAGE $A4000 PERMET DE DESACTIVER EN 0V LA SORTIE 4.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='4')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree4.detach();
    pinMode(ledPin10, OUTPUT); 
    digitalWrite(ledPin10, LOW);    // set the LED off
    val4="$A=4000\n";
    Serial.print("$A=4000\n");
  }
// MESSAGE $A3001 PERMET D'ACTIVER EN 5V LA SORTIE 3.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='3')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree3.detach();
    pinMode(ledPin11, OUTPUT); 
    digitalWrite(ledPin11, HIGH);   // set the LED on
    val3="$A=3001\n";
    Serial.print("$A=3001\n");

  }
// MESSAGE $A3000 PERMET DE DESACTIVER EN 0V LA SORTIE 3.  
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='3')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree3.detach();
    pinMode(ledPin11, OUTPUT); 
    digitalWrite(ledPin11, LOW);    // set the LED off
    val3="$A=3000\n";
    Serial.print("$A=3000\n");
  }
// MESSAGE $A2001 PERMET D'ACTIVER EN 5V LA SORTIE 2.
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='2')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    mon_entree2.detach();
    pinMode(ledPin12, OUTPUT); 
    digitalWrite(ledPin12, HIGH);   // set the LED on
    val2="$A=2001\n";
    Serial.print("$A=2001\n");

  }
// MESSAGE $A2000 PERMET DE DESACTIVER EN 0V LA SORTIE 2.  
  if((Res[0]=='$')&&(Res[1]=='A')&&(Res[2]=='2')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='0')){
    mon_entree2.detach();
    pinMode(ledPin12, OUTPUT); 
    digitalWrite(ledPin12, LOW);    // set the LED off
    val2="$A=2000\n";
    Serial.print("$A=2000\n");
  }
  
  //MESSAGE QUI PERMET DE VERIFIER LES SORTIE DIGITAL ACTIF.
  if((Res[0]=='$')&&(Res[1]=='Z')&&(Res[2]=='0')&&(Res[3]=='0')&&(Res[4]=='0')&&(Res[5]=='1')){
    Serial.print(val2);
    Serial.print(val3);
    Serial.print(val4);
    Serial.print(val5);
    Serial.print(val6);
    Serial.print(val7);
    Serial.print(val8);
    Serial.print(val9);
    Serial.print(val10);
    Serial.print(val11);
    Serial.print(val12);
    Serial.print(val13);  
  }

//MESSAGE $BBXXX PERMET DE COMMANDER LA SORTIE 11 EN PWM
  if((Res[0]=='$')&&(Res[1]=='B')&&(Res[2]=='B')){
    mon_entree11.detach();
   pinMode(11,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   analogWrite(11, RESU);
   
   if(RESU<10){
     Serial.print("$B=B00");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$B=B0");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$B=B");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
   
  }
//MESSAGE $BAXXX PERMET DE COMMANDER LA SORTIE 10 EN PWM  
    if((Res[0]=='$')&&(Res[1]=='B')&&(Res[2]=='A')){
      mon_entree10.detach();
   pinMode(10,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   analogWrite(10, RESU);
   
   if(RESU<10){
     Serial.print("$B=A00");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$B=A0");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$B=A");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
  
 //MESSAGE $B9XXX PERMET DE COMMANDER LA SORTIE 9 EN PWM 
    if((Res[0]=='$')&&(Res[1]=='B')&&(Res[2]=='9')){
      mon_entree9.detach();
   pinMode(9,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   analogWrite(9, RESU);
   
   if(RESU<10){
     Serial.print("$B=900");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$B=90");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$B=9");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    
 //MESSAGE $B6XXX PERMET DE COMMANDER LA SORTIE 6 EN PWM  
    if((Res[0]=='$')&&(Res[1]=='B')&&(Res[2]=='6')){
      mon_entree6.detach();
   pinMode(6,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   analogWrite(6, RESU);
   
   if(RESU<10){
     Serial.print("$B=600");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$B=60");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$B=6");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    
 //MESSAGE $B5XXX PERMET DE COMMANDER LA SORTIE 5 EN PWM   
    if((Res[0]=='$')&&(Res[1]=='B')&&(Res[2]=='5')){
      mon_entree5.detach();
   pinMode(5,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   analogWrite(5, RESU);
   
   if(RESU<10){
     Serial.print("$B=500");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$B=50");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$B=5");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    
 //MESSAGE $B3XXX PERMET DE COMMANDER LA SORTIE 3 EN PWM 
    if((Res[0]=='$')&&(Res[1]=='B')&&(Res[2]=='3')){
      mon_entree3.detach();
   pinMode(3,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   analogWrite(3, RESU);
   
   if(RESU<10){
     Serial.print("$B=300");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$B=30");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$B=3");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
   
 // LES COMMANDES SERVOS.
 /////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////
 if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='D')){
   mon_entree13.attach(ENTREE13);
   pinMode(ENTREE13,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree13.write(RESU);
   
   if(RESU<10){
     Serial.print("$D=D00");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=D0");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=D");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='C')){
   mon_entree12.attach(ENTREE12);
   pinMode(ENTREE12,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree12.write(RESU);
   if(RESU<10){
     Serial.print("$D=C00");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=C0");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=C");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='B')){
   mon_entree11.attach(ENTREE11);
   pinMode(ENTREE11,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree11.write(RESU);
   if(RESU<10){
     Serial.print("$D=B00");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=B0");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=B");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='A')){
   mon_entree10.attach(ENTREE10);
   pinMode(ENTREE10,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree10.write(RESU);
   if(RESU<10){
     Serial.print("$D=A00");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=A0");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=A");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='9')){
   mon_entree9.attach(ENTREE9);
   pinMode(ENTREE9,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree9.write(RESU);
   if(RESU<10){
     Serial.print("$D=900");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=90");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=9");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='8')){
   mon_entree8.attach(ENTREE8);
   pinMode(ENTREE8,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree8.write(RESU);
   if(RESU<10){
     Serial.print("$D=800");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=80");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=8");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='7')){
   mon_entree7.attach(ENTREE7);
   pinMode(ENTREE7,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree7.write(RESU);
   if(RESU<10){
     Serial.print("$D=700");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=70");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=7");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='6')){
   mon_entree6.attach(ENTREE6);
   pinMode(ENTREE6,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree6.write(RESU);
   if(RESU<10){
     Serial.print("$D=600");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=60");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=6");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='5')){
   mon_entree5.attach(ENTREE5);
   pinMode(ENTREE5,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree5.write(RESU);
   if(RESU<10){
     Serial.print("$D=500");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=50");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=5");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='4')){
   mon_entree4.attach(ENTREE4);
   pinMode(ENTREE4,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree4.write(RESU);
   if(RESU<10){
     Serial.print("$D=400");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=40");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=4");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='3')){
   mon_entree3.attach(ENTREE3);
   pinMode(ENTREE3,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree3.write(RESU);
   if(RESU<10){
     Serial.print("$D=300");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=30");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=3");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }
    if((Res[0]=='$')&&(Res[1]=='D')&&(Res[2]=='2')){
   mon_entree2.attach(ENTREE2);
   pinMode(ENTREE2,OUTPUT);
   RESU = ((Res[3]-48)*100) + ((Res[4]-48)*10) + ((Res[5]-48)*1);
   mon_entree2.write(RESU);
   if(RESU<10){
     Serial.print("$D=200");
     Serial.print(RESU);
   }
   if(RESU<100 && RESU>=10){
     Serial.print("$D=20");
     Serial.print(RESU);
   }
   if(RESU>=100){
     Serial.print("$D=2");
     Serial.print(RESU);
   }
   Serial.print("\n"); 
  }

i=0;      
    
 }
 
}
