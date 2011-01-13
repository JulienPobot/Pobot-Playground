/**
 * Exemple d'utilisation d'un capteur flexible
 * de type "Spectra Symbol"
 *
 * Test 1 : faire tourner un moteur
 *
 */

// la valeur minimale qu'il faudra retrancher pour avoir une gamme raisonnable
int zero = 0;

void setup() 
{
  // Entrée analogique pour le capteur
  pinMode(14,INPUT); // 14 = analog 0
  // Sortie numérique pour le moteur
  pinMode(9,OUTPUT); 

  // Initialisation de la liaison série (vitesse en paramètre)
  Serial.begin(9600);  
  Serial.println("Init : ne pas toucher pendant 5s");

  // Moyenne sur 10 échantillons
  for (int i = 0; i < 10; i++) {
    // division par le nombre d'échantillons
    zero += analogRead(0)/10; 
    delay(500);
  }
  // 
  Serial.print("Zero = ");
  Serial.println(zero);
}

void loop()
{
  // Affichage de la valeur reçue  
  //Serial.println(analogRead(0));
  
  // Génération d'un PWM sur la patte 9
  // connecté à la borne + d'un petit moteur
  // la borne - est à la masse
  analogWrite(9,analogRead(0)-zero);
  
  // Pause pour laisser le temps de visualisation
  // delay(100);
}


