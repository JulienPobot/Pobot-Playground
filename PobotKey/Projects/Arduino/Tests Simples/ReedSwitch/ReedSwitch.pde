/**
 * Interrupteur magnétique ("reed switch")
 *
 * Simple utilisation : changement de la led.
 *
 * (c) 2010 - Julien Holtzer pour Pobot
 */
 
// la led déjà placée sur une Arduino classique (DIGITAL 13)
int ledPin = 13;

// la patte de connexion de l'interrupteur (DIGITAL 2 car 0 et 1 sont utilisés par la série)
int inputPin = 2;

// statut de l'interrupteur
int val = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);
}

void loop(){
  // lire l'entrée
  val = digitalRead(inputPin);
  // 
  if (val == HIGH) {          
    // si l'interrupteur est ouvert, éteindre la led
    digitalWrite(ledPin, LOW);
  } else {
    // s'il est fermé, allumer la led
    digitalWrite(ledPin, HIGH);
  }
}
