#include "mavin.h"
#include "ModuleFunction.h"



u08 ligne1[8] = "        ";

int main(void)
{	
	// initialisation du robot (obligatoire)
	Initialize();		
	
	// affichage d'un grand classique de la programmation
	LcdDisplay("hello","world");
	u08 tempMic;
	
	
	while (1)
	{
		if(fMicSample){
			ADC_CHANNEL(MIC_IN);		
			tempMic = GetADC();			// AD start	
			fMicSample = 0;	 	        
		}
		
		sprintf(ligne1,"%d",ForwardPhotoSensor);
		LcdDisplay(ligne1,"capteur");
	}

}
