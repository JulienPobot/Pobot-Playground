/**
 * Effet de variation sur trois couleurs
 * 
 * On utilise des rubans de leds pilotés par PWM
 */

int rouge = 2;
int vert = 5;
int bleu = 6;

long index = 0;

void setup()
{

}

void loop()
{
  if (index <= 255) 
  {
    analogWrite(rouge,255-index);
    analogWrite(vert,index);
    analogWrite(bleu,0);
  } 
  else if (index <= 510) {
    analogWrite(rouge,0);
    analogWrite(vert,510-index);
    analogWrite(bleu,index-255);
  } 
  else if (index <= 764) {
    analogWrite(rouge,index-510);
    analogWrite(vert,0);
    analogWrite(bleu,765-index);
  }
  index++;
  if (index >= 765) {
    index = 0;
  }
  delay(30);
}




