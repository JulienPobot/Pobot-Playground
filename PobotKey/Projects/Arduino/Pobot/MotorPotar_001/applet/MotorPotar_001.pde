#define PILOT_A 7     // entrée A du pont en H
#define PILOT_B 8     // entrée B du pont en H
#define PILOT_PWM 9   // entrée "Enable" du pont en H
#define PILOT_REF 0   // entrée analogique reliée au commun de la piste du potentiomètre

#define ECRART_V_DIV2 150 // rapport PWM min pour déplacement moteur potentiomètre

int cmdValeur;
int cmdChar;

/***************************************/

void direction (byte dir)
{
  if (dir == 0)
  {
    digitalWrite (PILOT_A, HIGH);
    digitalWrite (PILOT_B, LOW);
  }
  else
  {
    digitalWrite (PILOT_A, LOW);
    digitalWrite (PILOT_B, HIGH);
  }
}

void allerA (int consigne)
{
  int positionlue = analogRead (PILOT_REF);
  int vitesse = 0;
  
  analogWrite (PILOT_PWM, 0);
  
  if (consigne == positionlue) return;
  if (consigne > 1023) return;
  if (consigne < 0) return;

  if (consigne > positionlue)
  {
    direction(1);
    if (positionlue < (consigne-ECRART_V_DIV2))
	analogWrite (PILOT_PWM, 150);
	
    while (positionlue < (consigne-ECRART_V_DIV2))
	positionlue = analogRead (PILOT_REF);
	
    analogWrite (PILOT_PWM, 100);
    while (positionlue < consigne)
	positionlue = analogRead (PILOT_REF); 
	
    digitalWrite (PILOT_B, LOW);
  }
  else
  {
    direction(0);
    if (positionlue > (consigne+ECRART_V_DIV2))
	analogWrite (PILOT_PWM, 255);
	
    while (positionlue > (consigne+ECRART_V_DIV2))
	positionlue = analogRead (PILOT_REF);
	
    analogWrite (PILOT_PWM, 140);
    while (positionlue > consigne)
	positionlue = analogRead (PILOT_REF);
	
    digitalWrite (PILOT_A, LOW);
  }
  
  analogWrite (PILOT_PWM, 0);
};

/***************************************/
void setup()
{
  pinMode(PILOT_A, OUTPUT);
  pinMode(PILOT_B, OUTPUT);
  pinMode(PILOT_PWM, OUTPUT);
  Serial.begin(9600);
};

void loop ()
{
    while (serialAvailable())
    {
	cmdChar = serialRead ();
	printByte (cmdChar);        
	switch (cmdChar)
	{
	  case 'a' : // retour chariot commande
                Serial.println("..OK");
		allerA (cmdValeur);
		cmdValeur = 0;                
            break;
	  case '?' :
		Serial.println(analogRead(PILOT_REF));
	    break;
	  case '0' : 
	  case '1' : 
	  case '2' : 
	  case '3' : 
	  case '4' : 
	  case '5' : 
	  case '6' : 
	  case '7' : 
	  case '8' : 
	  case '9' : 
            cmdValeur = cmdValeur * 10 + (cmdChar-48);
            break;
          default:
            Serial.print(cmdChar, DEC);
	}
    }
};
