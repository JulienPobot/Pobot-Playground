/**
 * Démonstration de l'utilisation d'un buzzer piezo avec une Arduino
 * 
 * (c) Julien Holtzer pour Pobot
 *
 */

// déclaration de la patte (digital) sur laquelle le buzzer est connecté (via une résistance d'1k)
int pin = 4;

// la fréquence variable de la note qui sera jouée par le buzzer
int freq = 0;

/**
 * Initialisation
 */
void setup()
{
  // rien de particulier
}

void loop()
{
  // on utilise une bibliothèque fournie avec la carte Arduino
  // qui a l'avantage d'être non bloquante (on peut continuer et gérer autre chose)
  
  // 3 paramètres : la patte de sortie, la fréquence et la durée du son à jouer
  tone(pin, freq, 50);
  
  // on doit ajouter un délai car l'appel précédent n'est pas bloquant
  // donc sinon on changerait de fréquence avant d'avoir entendu le son demandé
  // (on choisit un temps supérieur à la durée du son pour calmer nos oreilles)
  delay(250);
  
  // et on change la fréquence, par exemple en demandant une génération aléatoire
  freq = random(20,400);
  
}
