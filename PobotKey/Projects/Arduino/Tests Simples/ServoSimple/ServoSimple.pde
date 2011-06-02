#include <Servo.h>

/** 
 * Programme de test d'un servomoteur
 * utilisation de la bibliothèque "Servo" pour Arduino
 * à partir de la version 0018
 */


// chaque servomoteur doit être déclaré dans une variable de type "Servo"
// le nom de la variable importe peu : s, s1, servo, bras, pince, etc.
// et on peut même utiliser un tableau pour gérer beaucoup de servos.
Servo servo;

// une variable conserve l'angle commandé au servomoteur
// cet angle varie entre 0 et 180 conformément à la bibliothèque Arduino
// disponible dans la doc : http://www.arduino.cc/en/Reference/Servo
int angle = 0;

// une variable conserve le sens actuel de variation de la commande du servomoteur
// i.e. si on est en train de croite (de 0 à 180) ou décroite (de 180 à 0)
int sens = 0;

/**
 * Initialisation du programme.
 * Cette fonction est automatiquement appelée à la mise sous tension de la carte
 * et est obligatoire. On peut ne rien y faire, mais généralement on initialise ici
 * toutes les fonctions (Servo, communication série, déclaration des entrées/sorties)
 */
void setup()
{
  // La variable servo qui représente le moteur doit être "attachée" au numéro de patte 
  // correspondant à l'entrée de la carte Arduino sur laquelle on a fixé le moteur.
  // Ici, 9 mais depuis l'amélioration de la librairie (> version 0017), 12 servomoteurs peuvent être utilisés
  servo.attach(10);
  // Des options sont disponibles pour préciser les bornes min et max réelles du moteur
  // mais dans le cas d'un servomoteur standard ce n'est pas nécessaire.
}

/**
 * Boucle infinie principale. Obligatoire et appelée automatiquement, en continu dès qu'on sort de cette fonction.
 * Il est possible de ne jamais en sortir et de tout contrôler à l'intérieur.
 */
void loop()
{
  // Première étape, mettre à jour la consigne
  if (sens == 0)
  {
    // de manière arbitraire on décide que 0 signifie sens croissant
    // donc il faut ici incrémenter la position souhaitée du servo (qui servira de commande)
    angle++;
  } 
  else {
    // bien sûr dans le cas contraire, on décroit    
    angle--;
  }

  // Seconde étape, donner cette consigne au moteur.
  // Le mot-clé choisi pour la bibliothèque est "write", comme pour écrire une consigne 
  // dans la mémoire du servomoteur. C'est un choix arbitraire, ce qu'il faut retenir c'est que 
  // c'est une consigne à atteindre, le servomoteur se chargeant de passer de sa position actuelle à cette consigne.
  servo.write(angle);
  
  // Troisième étape, s'assurer qu'on n'est pas arrivé en bout de course
  // Si c'est le cas, on change de sens
  if (angle <= 0)
  {
    // ici on teste si on est descendu en-dessous de zéro.
    // il est important de ne pas seulement tester "==0" car on pourrait plus tard
    // changer la valeur de l'incrément et du décrément (ici, angle-- fait descendre de 1 donc
    // on est assuré d'arriver à 0 donc que le test fonctionnera, mais c'est une règle de faire des tests plus sûrs.
    sens = 0;
  } 
  if (angle >= 180) {
    // le cas opposé, on est monté jusqu'à la limite supérieure, il faut redescendre
    sens = 1;
  }
  
  // Quatrième étape non obligatoire : attendre avant de changer de consigne
  // Un servomoteur est conçu pour atteindre "tout seul" une consigne donnée
  // alors que notre code commande degré par degré.
  // En utilisant un délai (ici, 10 millisecondes), on maitrise la vitesse de rotation du moteur.
  delay(10);

}

