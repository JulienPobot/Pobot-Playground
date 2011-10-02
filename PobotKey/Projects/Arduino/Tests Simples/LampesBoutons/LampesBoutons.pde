
// nombre de lampes
const int nombre = 7;

// déclaration des lampes (pattes en sortie)
int lampes[nombre] = {
  10,11,12,13,18,19,20};

// déclaration des lampes (pattes en entrée)
int boutons[nombre] = {
  31,32,33,40,41,42,43};
  
// interrupteur global
int special = 25;

// déclaration de la mémoire d'état (allumé ou éteint)
boolean mem_etat_lampe[nombre];
boolean mem_front_bouton[nombre];

boolean mem_etat_special;
boolean mem_front_special;

/**
 * Configuration initiale
 */
void setup ()
{
  // mettre les boutons en entrée et les lampes en sortie
  for (int i = 0; i < nombre; i++) {
    pinMode (boutons[i],INPUT);
    // activer la résistance de pull-up interne 
    digitalWrite (boutons[i],HIGH); 
    pinMode (lampes[i],OUTPUT);
  }
  // plus le bouton spécial pour tout éteindre
  pinMode(special,INPUT);
  digitalWrite(special,HIGH);  
}

/**
 * Boucle infinie
 */
void loop()
{
  // gérer tous les boutons
  for (int i=0; i<nombre; i++) 
  {
    // lecture du bouton
    int etat = digitalRead(boutons[i]);

    // poussé et lampe éteinte et nouvelle action
    if (etat && !mem_etat_lampe[i] && !mem_front_bouton[i]) {
      // allumer la lampe (relais)
      digitalWrite(lampes[i],HIGH);
      // mémoriser que la lampe est allumée
      mem_etat_lampe[i]=true;
      // mémoriser qu'on vient de prendre en compte l'action
      mem_front_bouton[i]=true;
    }
    // poussé et lampe allumée et nouvelle action
    else if(etat && mem_etat_lampe[i] && !mem_front_bouton[i]) {
      // éteindre la lampe (relais)
      digitalWrite(lampes[i],LOW);
      // mémoriser que la lampe est éteinte
      mem_etat_lampe[i]=false;
      // mémoriser qu'on vient de prendre en compte l'action      
      mem_front_bouton[i]=true; 
    }

    // si le bouton est relâché, on a terminé le travail et on s'endort jusqu'au prochain "front" (appui sur le bouton)
    if (!etat) {
      // remise à zéro de l'action
      mem_front_bouton[i]=false; 
    }    
  }

  // on surveille les interrupteurs dix fois par seconde
  delay(100);

}




