/**
 * Test de vitesse du RP5 à chenilles
 *
 * utilisation d'analogWrite pour faire varier la vitesse
 *
 * matériel : une Romeo (driver L298)
 */

void setup()
{
  // sur la carte Roméo, le moteur A est sur D4 et D5
  // et le moteur B est sur D6 et D7
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);

  // on change la fréquence pour éviter d'entendre le sifflement.
  // le paramètre prend les valeurs 1, 8, 64, 1024
  setPwmFrequency(1);

}

void loop()
{
  // sélection du sens de rotation
  digitalWrite(4,HIGH); //   
  // accélération
  for (int i=0; i<256; i++) {
    // on écrit la vitesse sur l'enable
    analogWrite(5,i);
    // le délai détermine la pente de l'accélération
    delay(100*8);
  };
  // une fois la vitesse max atteinte, la maintenir 1 seconde
  digitalWrite(5,HIGH);
  delay(1000*8);
  // puis décélérer  
  for (int i=255; i>=0; i--) {
    // on écrit la vitesse sur l'enable
    analogWrite(5,i);
    // le délai détermine la pente de l'accélération
    delay(100*64);
  };
  // arrêter
  digitalWrite(5,LOW); // disable
  digitalWrite(6,LOW); // disable
  // attendre
  delay(1000*64);

}

/**
 * Fonction de modification du timer pour les pattes 5 et 6
 */
void setPwmFrequency(int divisor) {
  byte mode;
  switch(divisor) {
  case 1: 
    mode = 0x01; 
    break;
  case 8: 
    mode = 0x02; 
    break;
  case 64: 
    mode = 0x03; 
    break;
  case 256: 
    mode = 0x04; 
    break;
  case 1024: 
    mode = 0x05; 
    break;
  default: 
    return;
  }
  // le code important : changement du prescaler du comparateur B du timer 0
  TCCR0B = TCCR0B & 0b11111000 | mode;    
}



