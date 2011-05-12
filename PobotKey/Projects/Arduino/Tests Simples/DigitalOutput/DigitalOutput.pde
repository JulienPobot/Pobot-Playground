int rouge = 3;   
int vert = 5;   
int bleu = 6;   


void setup()
{
 noInterrupts();
 detachInterrupt(0);
 pinMode(rouge,OUTPUT);
 pinMode(vert,OUTPUT);
 pinMode(bleu,OUTPUT);
 digitalWrite(rouge,HIGH); 
 digitalWrite(vert,HIGH);  
 digitalWrite(bleu,HIGH); 
}

void loop()
{
  
}
