import processing.serial.*;

Serial serial;
int serialPort = 1;
int baudrate = 9600;

int sen = 3; // sensors

int[] valuesX;
int[] valuesY;
int[] valuesZ;

String trame;
String[] parts;

void setup() 
{
  size(640, 480);
  frameRate(25);

  // Open the port that the board is connected to and use the same speed (9600 bps)

  println(Serial.list());
  serial = new Serial(this, Serial.list()[serialPort], baudrate); 
  serial.bufferUntil(10); // line feed

  valuesX = new int[width];
  valuesY = new int[width];
  valuesZ = new int[width];
  smooth();
}

int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}

/**
 * Réception d'une trame
 */
void serialEvent(Serial p) {
  // traiter la chaine de caractères
  trame = serial.readString();
  // vérifier la longueur
  if (trame.length() < 9) {
    print("e");
    return;
  }   
  parts = split(trame, " ");
  // décaler les valeurs précédentes  
  for (int i=0; i<width-1; i++) {
    valuesX[i] = valuesX[i+1];
    valuesY[i] = valuesY[i+1];
    valuesZ[i] = valuesZ[i+1];    
  }

  // enregistrer les nouvelles valeurs filtrées
  valuesX[width-1] = int(float(parts[0]));
  valuesY[width-1] = int(float(parts[1]));
  valuesZ[width-1] = int(float(parts[2]));
  
}

void draw()
{ 
  // déclencher l'affichage
  updateOscillo();
  
}

void updateOscillo() {
  background(250);
  for (int x=1; x<width; x++) {
    stroke(150,0,0);
    line(width-x,   height-1-getY(valuesX[x-1]), 
    width-1-x, height-1-getY(valuesX[x]));
    //
    stroke(0,150,0);
    line(width-x,   height-1-getY(valuesY[x-1]), 
    width-1-x, height-1-getY(valuesY[x]));
    //    
    stroke(0,0,150);
    line(width-x,   height-1-getY(valuesZ[x-1]), 
    width-1-x, height-1-getY(valuesZ[x]));
  }
}

