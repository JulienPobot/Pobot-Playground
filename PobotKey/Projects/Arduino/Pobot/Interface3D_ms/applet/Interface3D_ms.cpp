/**
 * Adaptation du programme Interface 3D pour retourner un nombre de millisecondes
 */

// la fr\u00e9quence du courant EDF (50 Hz)
// pour neutraliser l'impact du 50Hz ambiant (lumi\u00e8re n\u00e9on),
// on doit faire des \u00e9chantillonnages au moins 2 fois plus longs
#define mains 50

// on analyse chaque capteur 40 millisecondes (2 p\u00e9riodes du 50 Hz)
#define refresh 2 * 1000000 / mains

/**
 * Initialisation du programme
 */
#include "WProgram.h"
void setup();
void loop();
long time(int pin, byte mask);
void startTimer();
unsigned long checkTimer();
void setup() {

  // d\u00e9marrage de la connexion s\u00e9rie avec la carte 
  // \u00e0 vitesse maximum : 115200 bits par seconde
  Serial.begin(115200);

  // mettre toutes les pattes num\u00e9riques de la carte
  // \u00e0 niveau bas en sortie pour \u00e9viter les perturbations
  for(int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  // les trois pattes des capteurs sont en sortie
  // m\u00eame si ensuite on passera d'un mode \u00e0 l'autre
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);

  startTimer();
}

void loop() {  
  Serial.print(time(8, B00000001), DEC);
  Serial.print(" ");
  Serial.print(time(9, B00000010), DEC);
  Serial.print(" ");
  Serial.println(time(10, B00000100), DEC);

} 

/**
 * Fonction d'acquisition d'une mesure
 * - pin est le num\u00e9ro de connexion du capteur capacitif sur la carte
 * - mask indique \u00e9galement quelle patte du port B est connect\u00e9e
 */
long time(int pin, byte mask) {
  // moyenne accumule le nombre de \u00b5s entre la mise \u00e0 z\u00e9ro et la charge
  unsigned long moyenne = 0;
  // mesures indique combien de charges successives ont \u00e9t\u00e9 mesur\u00e9es en 40 ms
  unsigned long mesures = 0;
  //
  unsigned long start = checkTimer();

  // tant qu'on n'a pas \u00e9teint
  while(start < refresh) {
    // mettre la patte \u00e0 z\u00e9ro volts
    pinMode(pin, OUTPUT);
    PORTB = 0;
    // puis se remettre en entr\u00e9e pour attendre la charge du \u00b5C
    pinMode(pin, INPUT);
    // red\u00e9marrer par une prise du chrono en cours
    start = checkTimer();
    // attendre que le capteur soit suffisament charg\u00e9, 
    // i.e. la tension atteint la valeur de rupture de 0 \u00e0 1
    // sur l'entr\u00e9e num\u00e9rique du microcontr\u00f4leur, soit ... volts
    while((PINB & mask) == 0) {
      // rien, on attend que le capteur se charge
    }
    moyenne += checkTimer() - start;    
    mesures++;
  }
  // red\u00e9marrer le timer pour le suivant
  startTimer();
  // retourne la moyenne de toutes les valeurs
  return moyenne / mesures;
}

/**************************/
/* Gestion du chronom\u00e8tre */
/**************************/

// compte du nombre d'overflows
extern volatile unsigned long timer0_overflow_count;

/**
 * D\u00e9marrer le compteur
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

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

