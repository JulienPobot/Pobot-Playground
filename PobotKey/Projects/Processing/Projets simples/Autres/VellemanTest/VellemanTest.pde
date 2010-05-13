import org.xvolks.jnative.util.K8055D;

long start_ms = 0;
long total_ms = 0;
boolean mesure_encours = false;

void setup() 
{
  K8055D.initCard(0);
}

void draw()
{ 
  if (!mesure_encours && K8055D.digitalRead(1)) {
    // démarre le compte
    mesure_encours = true;
    start_ms = millis();
    K8055D.digitalClear();  
    println("Start");
  }
  if (mesure_encours && K8055D.digitalRead(2)) {
    // arrête le compte
    mesure_encours = false;
    total_ms = millis()-start_ms; 
    K8055D.digitalFull();  
    println("Stop : "+total_ms/1000+"\""+(total_ms%1000));
  }
  if (mesure_encours) {
    // compter les secondes
    K8055D.digitalWrite((int)(millis()-start_ms)/1000,true); 
    
  }
}











