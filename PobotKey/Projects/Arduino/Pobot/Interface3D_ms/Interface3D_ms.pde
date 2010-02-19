/**
 * Adaptation du programme Interface 3D pour retourner un nombre de millisecondes
 */

// la fréquence du courant EDF (50 Hz)
// pour neutraliser l'impact du 50Hz ambiant (lumière néon),
// on doit faire des échantillonnages au moins 2 fois plus longs
#define mains 50

// on analyse chaque capteur 40 millisecondes (2 périodes du 50 Hz)
#define refresh 2 * 1000000 / mains

/**
 * Initialisation du programme
 */
void setup() {

  // démarrage de la connexion série avec la carte 
  // à vitesse maximum : 115200 bits par seconde
  Serial.begin(115200);

  // mettre toutes les pattes numériques de la carte
  // à niveau bas en sortie pour éviter les perturbations
  for(int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  // les trois pattes des capteurs sont en sortie
  // même si ensuite on passera d'un mode à l'autre
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);

  startTimer();
}

void loop() {  
  Serial.print(time(2, B00000100), DEC);
  Serial.print(" ");
  Serial.print(time(3, B00001000), DEC);
  Serial.print(" ");
  Serial.print(time(4, B00010000), DEC);
  Serial.print(" ");
  Serial.print(time(5, B00100000), DEC);
  Serial.print(" ");
  Serial.print(time(6, B01000000), DEC);
  Serial.print(" ");
  Serial.print(time(7, B10000000), DEC);
  Serial.print(" ");
  Serial.print(time(8, B00000001), DEC);
  Serial.print(" ");
  Serial.print(time(9, B00000010), DEC);
  Serial.print(" ");
  Serial.println(time(10, B00000100), DEC);

} 

/**
 * Fonction d'acquisition d'une mesure
 * - pin est le numéro de connexion du capteur capacitif sur la carte
 * - mask indique également quelle patte du port B est connectée
 */
long time(int pin, byte mask) {
  // moyenne accumule le nombre de µs entre la mise à zéro et la charge
  unsigned long moyenne = 0;
  // mesures indique combien de charges successives ont été mesurées en 40 ms
  unsigned long mesures = 0;
  //
  unsigned long start = checkTimer();

  // tant qu'on n'a pas éteint
  while(start < refresh) {
    // mettre la patte à zéro volts
    pinMode(pin, OUTPUT);
    PORTB = 0;
    // puis se remettre en entrée pour attendre la charge du µC
    pinMode(pin, INPUT);
    // redémarrer par une prise du chrono en cours
    start = checkTimer();
    // attendre que le capteur soit suffisament chargé, 
    // i.e. la tension atteint la valeur de rupture de 0 à 1
    // sur l'entrée numérique du microcontrôleur, soit ... volts
    while((PINB & mask) == 0) {
      // rien, on attend que le capteur se charge
    }
    moyenne += checkTimer() - start;    
    mesures++;
  }
  // redémarrer le timer pour le suivant
  startTimer();
  // retourne la moyenne de toutes les valeurs
  return moyenne / mesures;
}

/**************************/
/* Gestion du chronomètre */
/**************************/

// compte du nombre d'overflows
extern volatile unsigned long timer0_overflow_count;

/**
 * Démarrer le compteur
 */
void startTimer() {
  timer0_overflow_count = 0;
  TCNT0 = 0;
}

/**
 * Lire la valeur du compteur (en microsecondes)
 */
unsigned long checkTimer() {
  return ((timer0_overflow_count << 8) + TCNT0) << 2;
}
