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

#define SAMPLES  10

// pattes analogiques sur lesquelles les Sharp sont connectés
int pinLeft = 1; 
int pinRight = 2;

int pin = pinLeft;

int valuesL[SAMPLES];
int valuesR[SAMPLES];

int index = 0;

// la moyenne pour gauche et droite

int valueLeft = 0;
int valueRight = 0;

// la moyenne
int moyenne = 0;

// l'angle qu'on souhaite connaitre
int result = 0;

int incr = 0;

long last = 0;

void setup()
{
  // liaison série
  Serial.begin(38400);


}

void loop()
{
  // remplir un tableau de valeurs
  if (pin == pinLeft) {
    valuesL[index] = analogRead(pin);
  } 
  else {
    valuesR[index] = analogRead(pin);
  }
  index++;

  if (index >= SAMPLES) {
    // le tableau est plein, on analyse son contenu
    moyenneLeft();
    moyenneRight();
    // calcul de la moyenne
    moyenne = (valueLeft + valueRight) / 2;

    //Serial.print(millis());
    //Serial.print("\t");
    Serial.print(valueLeft);
    Serial.print(" ");
    Serial.print(valueRight);
    Serial.print(" ");
    Serial.print(0);
    Serial.print(incr);
    Serial.println();

    // repartir
    index = 0;

    if (pin == pinLeft) {
      pin = pinRight; 
    } 
    else {
      pin = pinLeft; 
    }

    if (millis() > last + 5000) {
      // toutes les deux secondes
      incr++;
      last = millis();
    }
  }

  // en déduire la valeur fusionnée qu'on utilisera
  // soit pour de l'évitement, soit pour de la cible

  // on va d'abord tester la fuite
  // d'un angle égal (pour être parallèle à l'obstacle)

  if (valueRight < valueLeft) {
    // fuite à gauche

  } 
  else {
    // fuite à droite

  }

}

int moyenneLeft(void)
{
  valueLeft = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    valueLeft += valuesL[i];   
  }
  valueLeft /= SAMPLES;
}

int moyenneRight(void)
{
  valueRight = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    valueRight += valuesR[i];   
  }
  valueRight /= SAMPLES;
}











