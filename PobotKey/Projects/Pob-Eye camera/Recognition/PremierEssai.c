#include "pob-eye.h"

#include "pattern.h"

char* transformeNom(UInt8 id);

int main (void)
{	
  int i=0;
  int nombre=0;  
  UInt8 * rgbFrame;
  Form listeFormes[MAX_OF_FORM];

  // initialisations obligatoires
  InitPobeye2();
  InitUART0(115200); 
  InitI2C(I2C_100_KHZ);
  InitCameraPobeye2();

  rgbFrame = GetRGBFrame();

  while(1)
  {		
    // attendre le déclenchement
    GetByteFromUART0();  

    // prendre la photo
    GrabRGBFrame();				

    // transformer en binaires
    BinaryRGBFrame(rgbFrame); 

    // chercher les formes parmi les "patterns" créés
    nombre=IdentifyForm(rgbFrame,listeFormes,pattern);	

    // traiter les formes une à une
    for (i=0;i<nombre;i++)
    {
      // afficher son nom
      PrintToUart0("Vu: ");
	 PrintToUart0(transformeNom(listeFormes[i].id));  	
      PrintToUart0(" à %d,%d\n",listeFormes[i].x,listeFormes[i].y);
    }	

    // si rien n'est reçu
    if (nombre == 0) {
      SendString("Rien !\n");  
    }
  }
  return 0;
}

char* transformeNom(UInt8 id)
{
  switch(id)
  {
  case IDP_CIRCLE:
    return "cercle";
  case IDP_CROSS:
    return "croix";
  case IDP_TRIANGLE:
    return "triangle";
  default: 
    return "inconnu";
  }
}
