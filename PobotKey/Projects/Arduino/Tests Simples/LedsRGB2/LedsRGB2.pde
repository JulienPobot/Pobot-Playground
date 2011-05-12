int rouge = 2;   
int vert = 5;   
int bleu = 6;   

int lap = 1;

void setup()
{
  Serial.begin(9600);
  // attention, la déclaration des sorties est OBLIGATOIRE
  pinMode(rouge,OUTPUT);
  pinMode(vert,OUTPUT);
  pinMode(bleu,OUTPUT);
}


void loop()
{
  for (int i = 1; i <= 255; i++)
  {
    for (int k = 1; k <=50; k++)
    {
      digitalWrite(rouge,HIGH);
      digitalWrite(vert,LOW);
      digitalWrite(bleu,LOW); 
      for (int j = 0; j <= i; j++)
      {
        delayMicroseconds(lap);
      }
      digitalWrite(rouge,LOW);
      digitalWrite(vert,HIGH);
      digitalWrite(bleu,LOW);
      for (int j = i; j <= 255; j++)
      {
        delayMicroseconds(lap);
      }    

    }    
  }
  //
  for (int i = 1; i <= 255; i++)
  {
    for (int k = 1; k <=50; k++)
    {
      digitalWrite(rouge,LOW);
      digitalWrite(vert,LOW);
      digitalWrite(bleu,HIGH);
      for (int j = 0; j <= i; j++)
      {
        delayMicroseconds(lap);
      }    
      digitalWrite(rouge,HIGH);
      digitalWrite(vert,LOW);
      digitalWrite(bleu,LOW);  
      for (int j = i; j <= 255; j++)
      {
        delayMicroseconds(lap);
      }    
    }    
  }
  //
  for (int i = 1; i <= 255; i++)
  {
    for (int k = 1; k <=50; k++)
    {
      digitalWrite(rouge,LOW);
      digitalWrite(vert,HIGH);
      digitalWrite(bleu,LOW);
      for (int j = 0; j <= i; j++)
      {
        delayMicroseconds(lap);
      }
      digitalWrite(rouge,LOW);
      digitalWrite(vert,LOW);
      digitalWrite(bleu,HIGH);
      for (int j = i; j <= 255; j++)
      {
        delayMicroseconds(lap);
      }    
    }    
  }


}






