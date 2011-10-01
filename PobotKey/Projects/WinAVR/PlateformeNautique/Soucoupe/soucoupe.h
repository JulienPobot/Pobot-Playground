//--------------------------------------------------------------------------

//#define 	F_CPU 3686400	//Fréquence de fonctionnement de la carte myAVR
#define NBR_CAPT	8	//version pour logic de lecture des capteurs ????????????????????????????????????????????????????????????????????????
#define	CAPT_A	PD0
#define	CAPT_B	PD1
#define	CAPT_E	PD4
#define	CAPT_F	PD5
#define	CAPT_G	PD6
#define	CAPT_H	PD7
#define	CAPT_C	PB6
#define	CAPT_D	PB7

#define	RELAY_1	PC1  //gouvernail
#define	RELAY_2	PC2  //Gouvernail
#define	CAPT_ILS	PC3  //detecteur de position central gouvernail ( tres moyen a revoir)

#define	RUDDER	PB1  // signal de 1 a 2 ms pour avant et arr plus vitesse.ocr a 255 avant toute,a 180 arret,a 105 arr toute		

#define DELAY_ECHAP	2000		//DELAY_ECHAP correpond au temps necéssaire pour contourner un obstacle
#define DELAY_POSITION	200		//DELAY_POSITION correspond au delai proposé pour rechercher la présence du capteur ILS et ainsi définir la position originale du gouvernail du bateau
#define DELAY_LOOP	1000		//DELAY_LOOP correspond à la fréquence de lancement de la boucle du "main"
#define DELAY_ROT	3000		//DELAY_ROT correspond au temps necéssaire pour une rotation du gouvernail de 45° (p/8)
#define DELAY_ARR	3000		//DELAY_ARR correspond au temps durant lequel le bateau va effectuer une marche arrière
#define DELAY_STOP	500			//DELAY_STOP correspond au temps nécessaire d'une propulsion inverse pour permettre l'arrêt du bateau (Si le bateau va en avant, toujours plein gaz, DELAY_STOP est le temps qu'il faudra permettre pour une propulsion arrière afin que le bateau se stoppe)
//TODO: trouver une formule mathématiques qui determine le temps nécéssaire de déclenchement d'une propulsion inverse, afin de stopper le bateau, et ce par rapport à sa vitesse initiale

#define LEFT	255		//Inclinaison du gouvernail à babord (gauche)       faux a revoir
#define RIGHT	100		//Inclinaison du gouvernail à tribord (droite)      faux a revoir
#define	ORIGIN	180		//Inclinaison du gouvernail par défaut (tout droit) faux a revoir

//--------------------------------------------------------------------------

void inline PortInitialization(void);

void inline TimerInitialization(int);

void MoveBack(void);

void MoveForward(void);

void Stop(void);

void Rotate(int);

void TurnOnLeft(int);

void GetSensorValues(int *);

void ScenarioFor3Sensors(void);

void ScenarioFor1Sensor(int tab[]);