#include "NoobDetector.h"



//================ INITIALISATION DU HARDWARE ==============================\\


void NoobDetector::initServo(){
	// il faut initialiser la position du servo

#ifdef REVERSEDSENCE
        myServo->write(180);
#else
	myServo->write(0);
#endif
}

void NoobDetector::initSensor(){
	// il faut lancer le enable
	// rien a faire ici normalement
}


//================ PARTIE LOGICIELLE ==============================\\


bool NoobDetector::proceedCalculation(int dt){
	bool found=false;
	int bDetections[4];
	int newPosServo;
        int servoAngle;

	// ici on calcule la position

	// deja, on lit les capteurs du detecteur de balise

	for(int i=0;i<4;i++){
		bDetections[i]= digitalRead(beaconPinOffset+i);
		if(bDetections[i]){
			found = true;
			break;
		}
	}



	if(found){
		dt=0;
#define RETPOS(offset) { _position = (servoAngle + offset)%360; return true;}
		// il faut checker NORTH, SOUTH, WEST , EAST et en fonction de l'angle
		if (  bDetections[0] ) {	// Nord
			RETPOS(0);
		}else if (bDetections[1]){	// Sud
			RETPOS(180);
		}else if (bDetections[2]){	// Ouest
			RETPOS(270);
		}else if (bDetections[3]){	// Est
			RETPOS(90);
		}

	}else if(_lastTime+dt <= 30000){
		_lastTime+=dt;
		// Il faut dÃ©placer les cervos pour balayer. (en fonction du sens)
		// on avance de beaconAngleDetection en beaconAngleDetection.

		// est-ce qu'on est Ã? la fin ?

		if ( servoAngle == ((balayageSens==-1)?0:180)){
			balayageSens*=-1;
			newPosServo = servoAngle += beaconAngleDetection;
		}else{
			// on peut continuer Ã? tourner.
			// faut faire gaffe Ã? pas aller en dessous de 0 ou au dessus de 180
			newPosServo = servoAngle += beaconAngleDetection;
			if(newPosServo<0){
					newPosServo=0;
			}else if (newPosServo > 180){
					newPosServo=180;
			}
		}
		// on envoit la sauce aux servos.
#ifdef REVERSEDSENCE
	    myServo->write(180-newPosServo);
#else
	    myServo->write(newPosServo);
#endif

	return false;
	}

}

