#ifndef _NOOB_DETECTOR_H
#define _NOOB_DETECTOR_H


#define beaconNorth      3
#define beaconSouth      4
#define beaconWest       5
#define beaconEast       6
#define beaconPinOffset  beaconNorth  // the first of the direction pins, for easier





#define beaconAngleDetection	20 // a modifier TODO . NE PAS LE FAIRE DEPASSER 88 degres !!!!!!!


#define waitAfterInit		500 // temps considere requis pour l'initialisation en position 0 des moteurs, etc


// decommenter la ligne du dessous si le moteur ne tourne pas dans le sens horaire (nord , est, sud, ouest)
// #define REVERSEDSENCE




#include "Wire.h"
#include "SoftServo.h"

// La classe NoobDetector
// Elle fonctionne sur un servo DEJA initialise sur lequel est le detecteur de balise.
// lui aussi deja initialise
// Appeler INIT apres l'instantiation
class NoobDetector{

	public:


		NoobDetector(SoftServo * myServo){
			_lastTime=30000;
			_position=0;
#ifdef REVERSEDSENCE
                        balayageSens=-1;
#else
			balayageSens=1; // on commence a balayer dans le sens horaire
#endif
		}

		void init(){
			initServo();
			initSensor();
                        delay(waitAfterInit);
		}

		// Retourne le temps abstrait (somme des dt)
		// pendant laquelle la position n'a pas pu etre determinee
		// maximum : 30 000
		int getlastFoundTime(){
				return _lastTime;
		}

		// Retourne la  position de l'objet detecte en degres de 0 a 359 par rapport a l'avant du robot
		// Avant du robot = 0
		// Note : ne pas croire le resultat si getLastFoundTime() est trop important
		int getNoobPosition(){
				return _position;
		}

		// La fonction "rapide" a apeller dans le loop.
		// Dt est une quantite de temps abstraite
		// qui symbolise le temps ecoule entre 2 appels de proceedCalculation
		// vous DEVEZ apeller init avant de lancer cette fonction
		// Note : dt n'est pas accru au dela de 30 000
		// Retourne TRUE si la cible a ete trouvee
		bool proceedCalculation(int dt);




	private:

		// Variables
		SoftServo *myServo;
		int _lastTime;
		int _position;

		int balayageSens;


                
		void initServo();
		void initSensor();





};

#endif


