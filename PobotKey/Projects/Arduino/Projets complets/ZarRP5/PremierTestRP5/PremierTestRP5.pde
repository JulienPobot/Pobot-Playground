/**
 * Premier test du RP5 à chenilles
 *
 * avance et recule puis tourne
 *
 * matériel : une Romeo (driver L298)
 */
 
void setup()
{
 // sur la carte Roméo, le moteur A est sur D4 et D5
 // et le moteur B est sur D6 et D7
 pinMode(4,OUTPUT);
 pinMode(5,OUTPUT);
 pinMode(6,OUTPUT);
 pinMode(7,OUTPUT); 
}

void loop()
{
  // mettre en route  
  digitalWrite(5,HIGH); // enable
  digitalWrite(6,HIGH); // enable
  // avancer
  digitalWrite(4,HIGH); //   
  digitalWrite(7,LOW); //  
  // attendre
  delay(1000);
  // arrêter
  digitalWrite(5,LOW); // disable
  digitalWrite(6,LOW); // disable
  // attendre
  delay(500);
    
}
