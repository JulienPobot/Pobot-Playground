//--------------------------------------------------------------------------
// Titre         : Bateau en navigation autonome
//--------------------------------------------------------------------------
// Fonction      : Permet au bateau de naviguer de façon totalement autonome en prenant en compte l'évitement d'obstacles
// Configuration :
//--------------------------------------------------------------------------
// Processeur    : ATmega8
// Fréquence	 : 3.6864 MHz
// Langage       : C
// Date          : 30.05.2011
// Version       : 0.4
// Auteur        : Mathieu BOLO
//--------------------------------------------------------------------------

//Inclusions des librairies nécéssaires au projet---------------------------
#include <avr/io.h>		//Registres AVR et définition des constantes
#include <util/delay.h>		//Librairie contenant les fonctions d'attente
#include <avr/interrupt.h>	//Librairie contenant les fonctions d'interruption
//--------------------------------------------------------------------------

//#define 	F_CPU 3686400	//Fréquence de fonctionnement de la carte myAVR
#define NBR_CAPT	8
#define	CAPT_A	PD0
#define	CAPT_B	PD1
#define	CAPT_C	PD4
#define	CAPT_D	PB6
#define	CAPT_E	PB7
#define	CAPT_F	PD5
#define	CAPT_G	PD6
#define	CAPT_H	PD7

#define	RELAY_1	PC1  //gouvernail
#define	RELAY_2	PC2  //Gouvernail
#define	CAPT_ILS	PC3  //detecteur de position central gouvernail ( tres moyen a revoir)

#define	RUDDER	PB1  // signal de 1 a 2 ms pour avant et arr plus vitesse.ocr a 255 avant toute,a 180 arret,a 105 arr toute		

#define DELAY_ECHAP	2000		//DELAY_ECHAP correpond au temps necéssaire pour contourner un obstacle
#define DELAY_POSITION	200		//DELAY_POSITION correspond au delai proposé pour rechercher la présence du capteur ILS et ainsi définir la position originale du gouvernail du bateau
#define DELAY_LOOP	1000		//DELAY_LOOP correspond �  la fréquence de lancement de la boucle du "main"
#define DELAY_ROT	3000		//DELAY_ROT correspond au temps necéssaire pour une rotation du gouvernail de 45° (π/8)
#define DELAY_ARR	3000		//DELAY_ARR correspond au temps durant lequel le bateau va effectuer une marche arrière
#define DELAY_STOP	500			//DELAY_STOP correspond au temps nécessaire d'une propulsion inverse pour permettre l'arrêt du bateau (Si le bateau va en avant, toujours plein gaz, DELAY_STOP est le temps qu'il faudra permettre pour une propulsion arrière afin que le bateau se stoppe)
//TODO: trouver une formule mathématiques qui determine le temps nécéssaire de déclenchement d'une propulsion inverse, afin de stopper le bateau, et ce par rapport �  sa vitesse initiale

#define LEFT	255		//Inclinaison du gouvernail �  babord (gauche)       faux a revoir
#define RIGHT	100		//Inclinaison du gouvernail �  tribord (droite)      faux a revoir
#define	ORIGIN	180		//Inclinaison du gouvernail par défaut (tout droit) faux a revoir
//--------------------------------------------------------------------------


//Fonction PortInitialization-------------------------------------------------------------
void PortInitialization()
{
	//PORTD: PD0,PD1,PD4,PD5,PD6,PD7 sont déclarés en entrée (capteurs infrarouges) / PD2,PD3 sont réservés pour INTERRUPTION
	DDRD |= 0b00000000;	//DDRD |= 0x00;

	//PORTC: PC1, PC2 sont déclarés en sortie (relais 1 & 2) / PC3 déclaré en entrée (capteur ILS)
	DDRC |=	0b00000110;	//DDRC |= 0x06;
	PORTC |= (1 << CAPT_ILS);		//On "push-pull" l'ILS (car on veut lui faire détecter une masse plutot qu'une tension --> moins de parasites, voire pas du tout)

	//PORTB: PB1 en sortie (variateur) / PB6 et PB7 en entrée (capteurs infrarouges)
	DDRB |= 0b00000010;	//DDRB |= 0x02;
}
//--------------------------------------------------------------------------


//Fonction TimerInitialization-------------------------------------------------------------
void TimerInitialization(int val)
{
	TCCR1A = _BV(COM1A1) | _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
	ICR1 = 2000;

	OCR1A = val;		//À 180 le variateur est arreté (STOP) / À 255, le variateur va en avant (AV) / À 105, le variateur va en arrière (ARR)

/*
	{Pas necessaire car déj�  pris en compte dans PortInitialization()!}
	DDRB |= 0x02;		//Le port du variateur (PB1) doit être déclaré en sortie
*/

	//_delay_ms(3000);
}
//--------------------------------------------------------------------------


//Fonction MoveForward-------------------------------------------------------------faux
void MoveBack()
{
	//Aller en marche arrière, c'est inactiver RELAY_1 et activer RELAY_2
	PORTC &= (0 << RELAY_1);
	PORTC |= (1 << RELAY_2);
}
//--------------------------------------------------------------------------


//Fonction MoveForward-------------------------------------------------------------faux
void MoveForward()
{
	//Aller en marche avant, c'est inactiver RELAY_2 et activer RELAY_1
	PORTC &= (0 << RELAY_2);
	PORTC |= (1 << RELAY_1);
}
//--------------------------------------------------------------------------


//Fonction Stop-------------------------------------------------------------faux
void Stop()
{
	//Stopper dépend de la vitesse initiale du bateau.

	int relay1 = PINC & _BV(RELAY_1);		//Lecture de la patte RELAY_1
	int relay2 = PINC & _BV(RELAY_2);		//Lecture de la patte RELAY_2

	//Si le bateau avance (RELAY_1 en travail, RELAY_2 en repos), on donne un petit coup de marche arrière (RELAY_1 en repos, RELAY_2 en travail) pour arrêter sa progression
	if (relay1 == 1) {MoveBack(); _delay_ms(DELAY_STOP);}

	//Sinon si le bateau recule (RELAY_2 en travail, RELAY_1 en repos), on donne un petit coup de marche avant (RELAY_2 en repos, RELAY_1 en travail) pour arrêter sa progression
	else if (relay2 == 1) {MoveForward(); _delay_ms(DELAY_STOP);}

	//Puis on inactive RELAY_1 et RELAY_2
	PORTC &= (0 << RELAY_1) & (0 << RELAY_2);
}
//--------------------------------------------------------------------------


//Fonction Rotate-------------------------------------------------------------faux
void Rotate(int delay)
{
	PORTB |= (1 << RUDDER); _delay_ms(delay);		//On active la patte du gouvernail pendant le délai souhaité
	PORTB &= (0 << RUDDER);		//Puis on désactive la patte du gouvernail
}
//--------------------------------------------------------------------------


//Fonction TurnOnLeft-------------------------------------------------------------faux
void TurnOnLeft(int delay)
{
	OCR1A = LEFT;		//on change la fréquence de découpage du timer pour que l'interprétation faite par le second ATMEGA fasse que le gouvernail tourne �  gauche
	Rotate(delay);
}
//--------------------------------------------------------------------------


//Fonction TurnOnRight-------------------------------------------------------------faux
void TurnOnRight(int delay)
{
	OCR1A = RIGHT;		//on change la fréquence de découpage du timer pour que l'interprétation faite par lesecond ATMEGA fasse que le gouvernail tourne �  droite
	Rotate(delay);
}
//--------------------------------------------------------------------------


//Fonction GetSensorValues-------------------------------------------------------------
void GetSensorValues(int *ptr)
{
	ptr[0] = PIND & _BV(CAPT_A);
	ptr[1] = PIND & _BV(CAPT_B);
	ptr[2] = PIND & _BV(CAPT_C);
	ptr[3] = PINB & _BV(CAPT_D);
	ptr[4] = PINB & _BV(CAPT_E);
	ptr[5] = PIND & _BV(CAPT_F);
	ptr[6] = PIND & _BV(CAPT_G);
	ptr[7] = PIND & _BV(CAPT_H);
}
//--------------------------------------------------------------------------


//Scenario pour 0 capteurs activés---------------------------------------
void ScenarioFor0Sensors()
{
	MoveForward();
}
//--------------------------------------------------------------------------


//Scenario pour 1 capteur activé---------------------------------------
void ScenarioFor1Sensor(int tab[])
{
	//On cherche �  savoir quel capteur a été activé en parcourant la liste des elements de tab[]
	int indice = 0;
	for (int i=0; i<NBR_CAPT;i++) {
		if (tab[i]==1) indice=i;
	}
	/*
	 * cf. GetSensorValues()
	 * Indice 0 --> CAPT_A
	 * Indice 1 --> CAPT_B
	 * Indice 2 --> CAPT_C
	 * Indice 3 --> CAPT_D
	 * Indice 4 --> CAPT_E
	 * Indice 5 --> CAPT_F
	 * Indice 6 --> CAPT_G
	 * Indice 7 --> CAPT_H
	*/

	if (indice>=0 && indice <=3) {		//Si l'indice est compris entre 0 et 3 (inclus), on tournera le gouvernail �  gauche
		/*
		 * Le délai �  appliquer pour la rotation dépend du capteur activé
		 * CAPT_A --> 90° babord (=2*DELAY_ROT)
		 * CAPT_B,CAPT_C,CAPT_D --> 45° babord (=DELAY_ROT)
		*/
		if (indice==0) {TurnOnLeft(2*DELAY_ROT); _delay_ms(DELAY_ECHAP); TurnOnRight(2*DELAY_ROT);}		//On tourne �  gauche de 90° pendant un certain temps, puis on revient en position initiale
		else {TurnOnLeft(DELAY_ROT); _delay_ms(DELAY_ECHAP); TurnOnRight(DELAY_ROT);}		//On tourne �  gauche de 45° pendant un certain temps, puis on revient en position initiale
	}

	else if (indice>=4 && indice <=7) {		//Sinon si l'indice est compris entre 4 et 7 (inclus), on tournera le gouvernail �  droite
		/*
		 * Le délai �  appliquer dépend du capteur activé
		 * CAPT_E --> 90° tribord (=2*DELAY_ROT)
		 * CAPT_F,CAPT_G,CAPT_H --> 45° tribord (=DELAY_ROT)
		*/
		if (indice==4) {TurnOnRight(2*DELAY_ROT); _delay_ms(DELAY_ECHAP); TurnOnLeft(2*DELAY_ROT);}		//On tourne �  droite de 90° pendant un certain temps, puis on revient en position initiale
		else {TurnOnRight(DELAY_ROT); _delay_ms(DELAY_ECHAP); TurnOnLeft(DELAY_ROT);}		//On tourne �  droite de 45° pendant un certain temps, puis on revient en position initiale
	}

}
//--------------------------------------------------------------------------


//Scenario pour 2 capteurs activés---------------------------------------
void ScenarioFor2Sensors(int tab[])
{
	//On cherche �  savoir quels capteurs ont été activés en parcourant la liste des elements de tab[]
	int ind1 = -1;
	int ind2 = -1;
	for (int i=0; i<NBR_CAPT;i++) {
		if (tab[i]==1) {
			if (ind1!=-1) ind2=i;		//Si ind1 n'a plus sa valeur par defaut, alors c'est qu'il a été alimenté, donc on peut alimenter ind2
			else ind1=i;		//Sinon on alimente ind1;
		}
	}
	/*
	 * cf. GetSensorValues()
	 * Indice 0 --> CAPT_A
	 * Indice 1 --> CAPT_B
	 * Indice 2 --> CAPT_C
	 * Indice 3 --> CAPT_D
	 * Indice 4 --> CAPT_E
	 * Indice 5 --> CAPT_F
	 * Indice 6 --> CAPT_G
	 * Indice 7 --> CAPT_H
	*/

	if ((ind1==0 && ind2==1) || (ind1==0 && ind2==2) || (ind1==1 && ind2==7)) {
		Stop();		//On stoppe le bateau
		TurnOnLeft(3*DELAY_ROT);		//On tourne �  babord de 135°
		MoveForward(); _delay_ms(DELAY_ECHAP);		//On s'échappe pendant un temps
		Stop();		//On stoppe �  nouveau le bateau
		TurnOnRight(3*DELAY_ROT);		//On se remet en position initiale
		MoveForward();		//Et on repart...
	}

	else if ((ind1==1 && ind2==2) || (ind1==1 && ind2==3)) {
		Stop();		//On stoppe le bateau
		TurnOnLeft(2*DELAY_ROT);		//On tourne �  babord de 90°
		MoveForward(); _delay_ms(DELAY_ECHAP);		//On s'échappe pendant un temps
		Stop();		//On stoppe �  nouveau le bateau
		TurnOnRight(2*DELAY_ROT);		//On se remet en position initiale
		MoveForward();		//Et on repart...
	}

	else if ((ind1==2 && ind2==3) || (ind1==2 && ind2==4)) {
		Stop();		//On stoppe le bateau
		TurnOnLeft(1*DELAY_ROT);		//On tourne �  babord de 45°
		MoveForward(); _delay_ms(DELAY_ECHAP);		//On s'échappe pendant un temps
		Stop();		//On stoppe �  nouveau le bateau
		TurnOnRight(1*DELAY_ROT);		//On se remet en position initiale
		MoveForward();		//Et on repart...
	}

	else if ((ind1==0 && ind2==7) || (ind1==0 && ind2==6)) {
		Stop();		//On stoppe le bateau
		TurnOnRight(3*DELAY_ROT);		//On tourne �  tribord de 135°
		MoveForward(); _delay_ms(DELAY_ECHAP);		//On s'échappe pendant un temps
		Stop();		//On stoppe �  nouveau le bateau
		TurnOnLeft(3*DELAY_ROT);		//On se remet en position initiale
		MoveForward();		//Et on repart...
	}

	else if ((ind1==5 && ind2==7) || (ind1==6 && ind2==7)) {
		Stop();		//On stoppe le bateau
		TurnOnRight(2*DELAY_ROT);		//On tourne �  tribord de 90°
		MoveForward(); _delay_ms(DELAY_ECHAP);		//On s'échappe pendant un temps
		Stop();		//On stoppe �  nouveau le bateau
		TurnOnLeft(2*DELAY_ROT);		//On se remet en position initiale
		MoveForward();		//Et on repart...
	}

	else if ((ind1==5 && ind2==6) || (ind1==4 && ind2==6)) {
		Stop();		//On stoppe le bateau
		TurnOnRight(1*DELAY_ROT);		//On tourne �  tribord de 45°
		MoveForward(); _delay_ms(DELAY_ECHAP);		//On s'échappe pendant un temps
		Stop();		//On stoppe �  nouveau le bateau
		TurnOnLeft(1*DELAY_ROT);		//On se remet en position initiale
		MoveForward();		//Et on repart...
	}
}
//--------------------------------------------------------------------------


//Scenario pour 3 capteurs activés (ou plus)---------------------------------------
void ScenarioFor3Sensors()
{
	Stop();		//On stoppe le bateau
}
//--------------------------------------------------------------------------


/*
//Fontion PositionInitialization---------------------------------------
void PositionInitialization()
{
	//On fait varier la position du gouvernail de gauche �  droite en augmentant le temps de rotation, tant que l'ILS n'est pas activé
	int delay = 0;
	int sens = LEFT;
	while ((PINC & _BV(CAPT_ILS))!=0) {
		delay += DELAY_POSITION;
		switch (sens) {
		case LEFT :
			TurnOnLeft(delay);
			sens = RIGHT;
			_delay_ms(100);
			break;

		case RIGHT :
			TurnOnRight(delay);
			sens = LEFT;
			_delay_ms(100);
			break;
		}
	}
}
*/


//Fonction Main-------------------------------------------------------------
int main(void)
{
	TimerInitialization(ORIGIN);		//Initialisation du timer (qui va permettre la rotation du gouvernail selon le creneau envoyé au second ATMEGA)
	PortInitialization();		//Initialisation des ports pour l'ATMEGA8

/*
	{Pas necessaire car beaucoup trop aleatoire!}
	PositionInitialization();		//Initlialisation de la position originale du bateau
*/

	while (1) {

		//Initialisation d'un tableau de (8) bits contenant les valeurs de nos capteurs infrarouges
		int tab[NBR_CAPT];
		GetSensorValues(tab);

		// On compte alors le nombre capteurs infrarouges activés (--> 1 capteur infrarouge activé possède une valeur �  0)
		int cpt=0;
		for(int i=0;i<NBR_CAPT;i++) {
			if (tab[i]==0) cpt++;
		}

		if(cpt >= 3) ScenarioFor3Sensors();		//Si 3 capteurs (au moins) sont activés, on déclenche le scenario associé
		else if(cpt == 2) ScenarioFor2Sensors(tab);		//Sinon si 2 capteurs (et seulement 2) sont activés, on déclenche le scenario associé
		else if(cpt == 1) ScenarioFor1Sensor(tab);		//Sinon si 1 capteur (et seulement 1) est activé, on déclenche le scenario associé
		else if(cpt == 0) ScenarioFor0Sensors();		//Sinon, aucun capteur n'est activé, on déclenche le scenario associé

		_delay_ms(DELAY_LOOP);
	}

	return 0;
}
//--------------------------------------------------------------------------


