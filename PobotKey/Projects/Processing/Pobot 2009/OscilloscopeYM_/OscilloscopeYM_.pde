import processing.serial.*;

PFont font;

// Valeurs pour la liaison série : 
Serial serial;
int serialPort = 1; // changer selon la machine

boolean flag_reception = false;
String trame;
String[] parts;

// Valeurs pour la capture : 

int sen = 3; // sensors

int[] valuesW;
int[] valuesP;
int[] valuesR;
int[] valuesX;
int[] valuesY;

int averageW = 0;
int averageP = 0;
int averageR = 0;
int averageX = 0;
int averageY = 0;

int ratioW = 30;
int ratioP = 30;
int ratioR = 30;
int ratioX = 1;
int ratioY = 1;

// Valeurs pour le chrono :

long temps = 0;

// Valeurs de l'échantillonnage pour moyenne :
int accu_mean = 0;
long accuW = 0;
long accuP = 0;
long accuR = 0;
long accuX = 0;
long accuY = 0;

// Valeurs pour les calculs d'IMU :

boolean flag_startIMU = false;

int angleW = 0;
int angleP = 0;
int angleR = 0;

int angleRK_W = 0;
int angleRK_P = 0;
int angleRK_R = 0;

int positionX = 0;
int positionY = 0;


/** 
 * Configuration
 */
void setup() 
{
  size(640, 600,P3D);
  frameRate(25);

  font = loadFont("ArialMT-20.vlw"); 
  textFont(font, 32); 

  // Open the port that the board is connected to and use the same speed (9600 bps)

  println(Serial.list());
  serial = new Serial(this, Serial.list()[serialPort], 115200);  
  serial.bufferUntil(10); // line feed

  valuesW = new int[width];
  valuesP = new int[width];
  valuesR = new int[width];
  valuesX = new int[width];
  valuesY = new int[width];
  smooth();
}


int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}

/**
 * Réception d'une trame
 */
void serialEvent(Serial p) {

  flag_reception = true;

  // juste pour vérifier qu'on arrive à gérer le temps delta
  // println(millis()-temps);
  temps = millis();

  // traiter la chaine de caractères
  trame = p.readString();
  // vérifier la longueur
  if (trame.length() < 15) {
    print("e");
    return;
  }   

  parts = split(trame, " ");

  // décaler les valeurs précédentes  
  for (int i=0; i<width-1; i++) {
    valuesW[i] = valuesW[i+1];
    valuesP[i] = valuesP[i+1];
    valuesR[i] = valuesR[i+1];    
    valuesX[i] = valuesX[i+1];
    valuesY[i] = valuesY[i+1];
  }

  // enregistrer les nouvelles valeurs filtrées
  valuesW[width-1] = int(float(parts[1])) - averageW;
  valuesP[width-1] = int(float(parts[2])) - averageP;
  valuesR[width-1] = int(float(parts[3])) - averageR;
  valuesX[width-1] = int(float(parts[4])) - averageX;
  valuesY[width-1] = int(float(parts[5])) - averageY;

  // nouvelle valeur, traiter l'IMU
  if (flag_startIMU) 
  {    
    // 5000 pour 360%
    
    // intégration du signal
    angleW += valuesW[width-1]/100;
    angleP += valuesP[width-1]/100;
    angleR += valuesR[width-1]/100;
    
    // Runge-Kutta
    angleRK_W += (valuesW[width-1]+2*valuesW[width-2]+2*valuesW[width-3]+valuesW[width-4])/6/100;
    angleRK_P += (valuesP[width-1]+2*valuesP[width-2]+2*valuesP[width-3]+valuesP[width-4])/6/100;
    angleRK_R += (valuesR[width-1]+2*valuesR[width-2]+2*valuesR[width-3]+valuesR[width-4])/6/100;
    
    positionX += valuesX[width-1]/10;
    positionY += valuesY[width-1]/10;
    
    //println("("+angleW*360/5000+","+angleP*360/5000+","+angleR*360/5000+") & ("+positionX+","+positionY+")");
    println("("+angleRK_W*360/5000+","+angleRK_P*360/5000+","+angleRK_R*360/5000+") & ("+positionX+","+positionY+")");
    

  }


}

void draw()
{ 
  if (flag_reception && accu_mean <= 100) {
    accu_mean++;
    print(".");
    // gestion du démarrage
    if (accu_mean < 50) {
      // ne rien faire les milles premières fois
      return;
    } 
    else if (accu_mean < 100) {      
      accuW +=  valuesW[width-1];
      accuP +=  valuesP[width-1];
      accuR +=  valuesR[width-1];    
      accuX +=  valuesX[width-1];
      accuY +=  valuesY[width-1];
      return ;
    } 
    else {
      averageW = (int) accuW / 50;
      averageP = (int) accuP / 50;
      averageR = (int) accuR / 50;   
      averageX = (int) accuX / 50;
      averageY = (int) accuY / 50;   
      println(); 
      println("Moyennes calculées.");
      //
      flag_startIMU = true;
    } 
  } 
  else {   
    updateViewAngles(); 
    //updateViewOscillo(); 
  }
}

void updateViewAngles() {
  
  background(250);
  fill(250,0,0);
  
  //translate(width/2+positionX, height/2+positionY, 0);
  translate(width/2, height/2, 0);
  pushMatrix();
  rotateY(angleW*2*PI/5000);
  rotateX(angleP*2*PI/5000);
  rotateZ(angleR*2*PI/5000);
  box(150);
  popMatrix();
  
  pushMatrix();
  fill(0,250,0);
  rotateY(angleRK_W*2*PI/5000);
  rotateX(angleRK_P*2*PI/5000);
  rotateZ(angleRK_R*2*PI/5000);
  box(150);
  popMatrix();
  
}


void updateViewOscillo() {

  background(250);

  for (int x=1; x<width; x++) {
    //
    stroke(150,0,0);
    line(width-x,   height-100-getY((valuesW[x-1]/ratioW)), 
    width-1-x, height-100-getY((valuesW[x]/ratioW)));
    //    
    stroke(0,150,0);
    line(width-x,   height-200-getY((valuesP[x-1]/ratioP)), 
    width-1-x, height-200-getY((valuesP[x]/ratioP)));
    //
    stroke(0,0,150);
    line(width-x,   height-300-getY((valuesR[x-1]/ratioR)), 
    width-1-x, height-300-getY((valuesR[x]/ratioR)));
    //
    stroke(0,150,150);
    line(width-x,   height-400-getY((valuesX[x-1]/ratioX)), 
    width-1-x, height-400-getY((valuesX[x]/ratioX)));
    //
    stroke(150,150,0);
    line(width-x,   height-500-getY((valuesY[x-1]/ratioY)), 
    width-1-x, height-500-getY((valuesY[x]/ratioY)));
  }

  // afficher la valeur courante  
  fill(0);
  text(valuesW[width-1]/30, width-100, 475);
  text(valuesP[width-1]/30, width-100, 375);
  text(valuesR[width-1]/30, width-100, 275);
  text(valuesX[width-1], width-100, 175);
  text(valuesY[width-1], width-100, 75);
  
 

}















