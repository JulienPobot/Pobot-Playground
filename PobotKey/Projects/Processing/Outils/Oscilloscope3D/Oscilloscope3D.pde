import processing.serial.*;

Serial serial;
int serialPort = 1;

int sen = 3; // sensors

byte[] inBuffer;

int val;      

int[] valuesX;
int[] valuesY;
int[] valuesZ;

void setup() 
{
  size(640, 480);
  frameRate(25);

  // Open the port that the board is connected to and use the same speed (9600 bps)

  println(Serial.list());
  serial = new Serial(this, Serial.list()[serialPort], 115200);

  valuesX = new int[width];
  valuesY = new int[width];
  valuesZ = new int[width];
  smooth();
}

int getY(int val) {
  return (int)(val / 1023.0f * height) - 1;
}

void draw()
{ 
  updateSerial();
}

void updateSerial() {
  String cur = serial.readStringUntil('\n');
  if (cur != null)
  {
    if (cur.length() < 16) {
      return;
    }
    // valeurs brutes
    println(cur);
    String[] parts = split(cur, " ");
    
    int[] xyz = new int[sen];
    for(int i = 0; i < sen; i++) {
      xyz[i] = int(float(parts[i]));
    }
    
    
    // gérer les X

    val = (xyz[0] - 3300)*2;    // ajuster la valeur
        
    for (int i=0; i<width-1; i++)
      valuesX[i] = valuesX[i+1];
    valuesX[width-1] = val;
    background(250);
    stroke(150,0,0);
    for (int x=1; x<width; x++) {
      line(width-x,   height-1-getY(valuesX[x-1]), 
      width-1-x, height-1-getY(valuesX[x]));
    }

    // gérer les Y

    val = (xyz[1] - 3800)*2;
    for (int i=0; i<width-1; i++)
      valuesY[i] = valuesY[i+1];
    valuesY[width-1] = val;
    stroke(0,150,0);
    for (int x=1; x<width; x++) {
      line(width-x,   height-1-getY(valuesY[x-1]), 
      width-1-x, height-1-getY(valuesY[x]));
    }

    // gérer les Z

    val = (xyz[2] - 3400)*2;
    for (int i=0; i<width-1; i++)
      valuesZ[i] = valuesZ[i+1];
    valuesZ[width-1] = val;
    stroke(0,0,150);
    for (int x=1; x<width; x++) {
      line(width-x,   height-1-getY(valuesZ[x-1]), 
      width-1-x, height-1-getY(valuesZ[x]));
    }
  }
}
