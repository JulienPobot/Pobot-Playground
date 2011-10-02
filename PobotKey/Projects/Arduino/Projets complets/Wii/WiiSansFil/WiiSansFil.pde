// Utilisation d'un nunchuck sans-fil (manette Wii)
// à partir de différents codes dont celui de Michael Dreher

#define ADRESSE  0x52

#include <Wire.h>
#include <utility\twi.h>

// tableau de stockage en sortie
int outbuf[6]; 

void setup()
{
  Serial.println("Test nunchuck sans-fil");
  Serial.begin(9600);

  Wire.begin(); 

  // il faut redéfinir la vitesse I2C  à 400 kHz au lieu des 100 kHz habituels
  // pour cela on accède directement au registre TWBR du micro-contrôleur ATmega328
  TWBR = ((CPU_FREQ / 400000L) - 16) / 2;

  // il faut désactiver le cryptage en initialisant le nunchuck différemment
  // qu'on le fait avec un nunchuck Nintendo
  Serial.println("Initialisation du nunchuck");

  // boucle infinie (jusqu'à ce que ça ait marché)
  while(true)
  {
    Serial.print(".");

    // deux octets doivent être écrits dans la mémoire 
    // - 0x55 à l'adresse 0xF0
    // - 0x00 à l'adresse 0x00

    // comme la communication peut être difficile, il faut essayer jusqu'à ce que ça fonctionne
    // donc on utilise le test de transmission fournit par la fonction Wire.endTransmission
    // 0 signifie le succès (4 autres codes désignent différents types d'erreur, cela ne nous intéresse pas)

    // premier octet
    Wire.beginTransmission(ADRESSE); 
    Wire.send(0xF0); // adresse
    Wire.send(0x55); // donnée.

    // si le premier est passé, on essaie le second
    if (Wire.endTransmission() == 0) 
    {
      Wire.beginTransmission(ADRESSE);
      Wire.send(0xFB); 
      Wire.send(0x00); 
      // on teste le second
      if (Wire.endTransmission() == 0) 
      {
        // tout va bien, l'initialisation est terminée, on sort de la boucle
        Serial.println("C'est parti !");
        return;
      }
    }
  }
}

void loop()
{
  // Lancer une lecture de 6 valeurs
  Wire.requestFrom(0x52,6); 

  for(int i = 0; (i < 6) && Wire.available (); i++)
  {
    outbuf[i] = Wire.receive();
  }

  // Vider le buffer I2C
  while (Wire.available()) {
    Wire.receive();
  }

  // Afficher les valeurs du Nunchuck
  printNunchuck();

  // Réinitialiser le Nunchuck
  // C'est la partie qui m'a longtemps manquée
  // et que j'ai trouvée sur le forum Windmeadow Labs !
  // sans trop d'explications sur ces 3-0
  // peut être les ingénieurs de Nintendo sont fans de la Coupe du Monde 1998 ?
  for(byte i = 0; i < 3; i++)
  {
    Wire.beginTransmission(ADRESSE); 
    Wire.send(0x00);
    Wire.endTransmission(); 
  }
  
  // Attente
  delay(20);
}

// Affichage des informations du nunchuck
void printNunchuck()
{
  // le joystick pouce (un octet par axe)
  int joy_x_axis = outbuf[0];
  int joy_y_axis = outbuf[1];
  
  //  l'accéléromètre 3 axes : un octet pour les 8 bits de poids fort  
  int accel_x_axis = outbuf[2] << 2;
  int accel_y_axis = outbuf[3] << 2;
  int accel_z_axis = outbuf[4] << 2;

  int z_button = 0;
  int c_button = 0;

  // un dernier octet pour les deux boutons
  // ainsi que les 2 bits de poids faible des 3 axes de l'accéléromètre
  if ((outbuf[5] >> 0) & 1)
  {
    z_button = 1;
  }
  if ((outbuf[5] >> 1) & 1)
  {
    c_button = 1;
  }

  if ((outbuf[5] >> 2) & 1)
  {
    accel_x_axis += 2;
  }
  if ((outbuf[5] >> 3) & 1)
  {
    accel_x_axis += 1;
  }

  if ((outbuf[5] >> 4) & 1)
  {
    accel_y_axis += 2;
  }
  if ((outbuf[5] >> 5) & 1)
  {
    accel_y_axis += 1;
  }

  if ((outbuf[5] >> 6) & 1)
  {
    accel_z_axis += 2;
  }
  if ((outbuf[5] >> 7) & 1)
  {
    accel_z_axis += 1;
  }

  Serial.print(joy_x_axis, DEC);
  Serial.print("\t");

  Serial.print(joy_y_axis, DEC);
  Serial.print("\t");

  Serial.print(accel_x_axis, DEC);
  Serial.print("\t");

  Serial.print(accel_y_axis, DEC);
  Serial.print("\t");

  Serial.print(accel_z_axis, DEC);
  Serial.print("\t");

  Serial.print(z_button, DEC);
  Serial.print("\t");

  Serial.print(c_button, DEC);
  Serial.print("\t");

  Serial.println();
}





