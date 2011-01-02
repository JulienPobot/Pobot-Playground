/***
 * Exemple simple d'utilisation de DuinOS
 * basé sur l'exemple fourni dans la v0.2 "MoreComplexBlinking"
 * Il s'agit tout simplement de faire clignoter deux leds en parallèle à des rythmes différents.
 *
 * Based on the original Blink code by David Cuartielles 
 *
 */

// on place les leds entre le 3,3 volts et les pattes analog 0 et 1 soit digital 14 et 15
int ledPinRed =  14;
int ledPinGreen =  15;

// Déclaration des deux tâches parallèles

taskLoop(redLED)
{
  digitalWrite(ledPinRed,HIGH);
  delay(200);
  digitalWrite(ledPinRed,LOW);
  delay(200);
}

taskLoop(greenLED)
{
  digitalWrite(ledPinGreen, HIGH);
  delay(500);
  digitalWrite(ledPinGreen, LOW);  // set the LED off
  delay(500);
}

// Fonctions habituelles

void setup()   
{                
  // Initialisation des deux sorties pour les leds
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  
  // Création des deux tâches
  createTaskLoop(redLED, NORMAL_PRIORITY);
  createTaskLoop(greenLED, NORMAL_PRIORITY);

  //This initializes the main loop's with a different priority (default is LOW_PRIORITY):
  initMainLoopPriority(NORMAL_PRIORITY);
}

void loop()                     
{
  // Deux tâches en parallèle pendant 2 secondes
  resumeTask(greenLED);
  resumeTask(redLED);
  delay(2000);
  // Puis seulement la verte pendant 2 secondes 
  suspendTask(redLED);
  delay(2000);
  // Puis seulement la rouge
  resumeTask(redLED);
  suspendTask(greenLED);
  delay(2000);
  // et on recommence (loop)
}

