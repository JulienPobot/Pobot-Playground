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
}

void loop()
{
  // sélection du sens de rotation
  digitalWrite(4,HIGH); //   
  // accélération
  for (int i=0; i<255; i++) {
    // on écrit la vitesse sur l'enable
    analogWrite(5,i);
    // le délai détermine la pente de l'accélération
    delay(100);
  };
  // arrêter
  digitalWrite(5,LOW); // disable
  digitalWrite(6,LOW); // disable
  // attendre
  delay(500);
    
}
