/**
 * Test d'utilisation sur Pobot Easy de capteurs Sharp
 * (et pas un test de capteur Sharp, c.f. "oscilloscope"
 * pour visualiser le signal sortant du Sharp)
 *
 * Matériel : 
 *  - un robot Pobot Easy
 *  - deux capteurs 2YOA21
 *
 * (c) Julien Holtzer pour Pobot - mai 2010
 */

// pattes analogiques sur lesquelles les Sharp sont connectés
int pinLeft = 1; 
int pinRight = 2;

int valueLeft = 0;
int valueRight = 0;

// la moyenne
double moyenne = 0;

// l'angle qu'on souhaite connaitre
double result = 0;

void setup()
{
  // liaison série
  Serial.begin(38400);


}

void loop()
{
  // mettre à jour les valeurs des Sharps
  valueLeft = analogRead(pinLeft);
  valueRight = analogRead(pinRight);

  // calcul de la moyenne
  moyenne = (valueLeft + valueRight) / 2;
  
  Serial.print( 0xff, BYTE);
  Serial.print( (val >> 8) & 0xff, BYTE);
  Serial.print( val & 0xff, BYTE);


  // en déduire la valeur fusionnée qu'on utilisera
  // soit pour de l'évitement, soit pour de la cible

  // on va d'abord tester la fuite
  // d'un angle égal (pour être parallèle à l'obstacle)

  if (valueRight < valueLeft) {
    // fuite à gauche
    
  } else {
    // fuite à droite
    
  }
  
}




