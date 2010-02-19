/**
 * Adaptation du programme Interface 3D pour retourner un nombre de millisecondes
 */

// la fr�quence du courant EDF (50 Hz)
// pour neutraliser l'impact du 50Hz ambiant (lumi�re n�on),
// on doit faire des �chantillonnages au moins 2 fois plus longs
#define mains 50

// on analyse chaque capteur 40 millisecondes (2 p�riodes du 50 Hz)
#define refresh 2 * 1000000 / mains

/**
 * Initialisation du programme
 */
void setup() {

  // d�marrage de la connexion s�rie avec la carte 
  // � vitesse maximum : 115200 bits par seconde
  Serial.begin(115200);

  // mettre toutes les pattes num�riques de la carte
  // � niveau bas en sortie pour �viter les perturbations
  for(int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  // les trois pattes des capteurs sont en sortie
  // m�me si ensuite on passera d'un mode � l'autre
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
 * - pin est le num�ro de connexion du capteur capacitif sur la carte
 * - mask indique �galement quelle patte du port B est connect�e
 */
long time(int pin, byte mask) {
  // moyenne accumule le nombre de �s entre la mise � z�ro et la charge
  unsigned long moyenne = 0;
  // mesures indique combien de charges successives ont �t� mesur�es en 40 ms
  unsigned long mesures = 0;
  //
  unsigned long start = checkTimer();

  // tant qu'on n'a pas �teint
  while(start < refresh) {
    // mettre la patte � z�ro volts
    pinMode(pin, OUTPUT);
    PORTB = 0;
    // puis se remettre en entr�e pour attendre la charge du �C
    pinMode(pin, INPUT);
    // red�marrer par une prise du chrono en cours
    start = checkTimer();
    // attendre que le capteur soit suffisament charg�, 
    // i.e. la tension atteint la valeur de rupture de 0 � 1
    // sur l'entr�e num�rique du microcontr�leur, soit ... volts
    while((PINB & mask) == 0) {
      // rien, on attend que le capteur se charge
    }
    moyenne += checkTimer() - start;    
    mesures++;
  }
  // red�marrer le timer pour le suivant
  startTimer();
  // retourne la moyenne de toutes les valeurs
  return moyenne / mesures;
}

/**************************/
/* Gestion du chronom�tre */
/**************************/

// compte du nombre d'overflows
extern volatile unsigned long timer0_overflow_count;

/**
 * D�marrer le compteur
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
