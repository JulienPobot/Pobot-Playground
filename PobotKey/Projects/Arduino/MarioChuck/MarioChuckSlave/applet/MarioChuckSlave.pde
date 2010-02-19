#include <MegaServo.h>


#define YEUX_MIN  42
#define YEUX_MAX  97
#define YEUX_CENTER  56;

#define BOUCHE_CLOSE  10;
#define BOUCHE_OPEN  30;

#define ROLL_MIN  60
#define ROLL_MAX  120
#define ROLL_MED  90

#define PITCH_MIN  45
#define PITCH_MAX  135
#define PITCH_MED  90

MegaServo servo[4];

int consigne[4];

int position[4];

boolean marche[4];

int increment_yeux[4] = {
  1,2,4,8};

int indexIncrYeux = 0;

void setup()
{
  Serial.begin(9600);

  // initialisation des servos
  servo[0].attach(8); 
  servo[1].attach(9); 
  servo[2].attach(10); 
  servo[3].attach(11);

  // les consignes par défaut  
  consigne[0] = YEUX_CENTER;
  consigne[1] = BOUCHE_CLOSE;
  consigne[2] = ROLL_MED;
  consigne[3] = PITCH_MED;
}

void loop()
{
  // 1. mise à jour des positions
  if (position[0] < consigne[0])
  {
    position[0] += increment_yeux[indexIncrYeux];   
  }
  if (position[0] > consigne[0])
  {
    position[0] -= increment_yeux[indexIncrYeux];    
  }

  // 2. commande des servos

  // les yeux (seulement si activé)
  if (marche[0]) {
    servo[0].write(position[0]);
  }

  // la bouche (toujours, sans gestion de la position)
  servo[1].write(consigne[1]);
  servo[2].write(consigne[2]);
  servo[3].write(consigne[3]);

  // réception des consignes
  while (Serial.available()) {
    switch (Serial.read())
    {
    case 'z':
      marche[0] = !marche[0];
      break;
    case 'c':
      indexIncrYeux = (indexIncrYeux+1)%4;     
      break;
    case 'b':
      // il faut fermer la bouche
      consigne[1] = BOUCHE_CLOSE;  
      break;
    case 'd':
      // il faut ouvrir la obuche
      consigne[1] = BOUCHE_OPEN;
      break;      
    case 'r':
      {
        while (!Serial.available());
        int val = Serial.read();
        Serial.print("r: ");
        Serial.print(val);
        consigne[3] = ROLL_MIN+((ROLL_MAX-ROLL_MIN)*val/18);
        Serial.print(" ");        
        Serial.println(consigne[3]);
      }
      break;
    case 'p':
      {
        while (!Serial.available());
        int val = Serial.read();
        consigne[2] = PITCH_MIN-((PITCH_MAX-PITCH_MIN)*val/18);          
      }
      break;    
    default:
      break;
    }
  }

  // mise à jour des consignes

  if (abs(consigne[0]-position[0]) < increment_yeux[indexIncrYeux])
  {
    // changer de consigne sur les yeux
    if (consigne[0] == YEUX_MIN)
    {
      consigne[0] = YEUX_MAX;
    } 
    else {
      consigne[0] = YEUX_MIN;
    }
  }

  delay(50);


}
