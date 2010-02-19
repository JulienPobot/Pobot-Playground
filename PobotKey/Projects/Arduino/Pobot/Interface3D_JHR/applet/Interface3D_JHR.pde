#define resolution 8
#define mains 50 // 60: north america, japan; 50: most other places

#define refresh 2 * 1000000 / mains

void setup() {
  Serial.begin(9600);

  // unused pins are fairly insignificant,
  // but pulled low to reduce unknown variables
  for(int i = 2; i < 14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  for(int i = 8; i < 11; i++)
    pinMode(i, INPUT);

  // for a glitch  
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW); 
  
  // for a glitch  
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW); 

  startTimer();
}


void loop() {    
  time(8, B00000001);
} 

long time(int pin, byte mask) {
  unsigned long count = 0, total = 0;
  digitalWrite(2,HIGH);
  while(checkTimer() < refresh) {
    // pinMode is about 6 times slower than assigning
    // DDRB directly, but that pause is important
    pinMode(pin, OUTPUT);
    PORTB = 0;
    pinMode(pin, INPUT);
    digitalWrite(3,HIGH);
    while((PINB & mask) == 0)
      count++;
    digitalWrite(3,LOW);
    Serial.println(count);
    total++;
  }  
  digitalWrite(2,LOW);
  startTimer();
  //Serial.println(count/total);
  return (count << resolution) / total;
}

extern volatile unsigned long timer0_overflow_count;

void startTimer() {
  timer0_overflow_count = 0;
  TCNT0 = 0;
}

unsigned long checkTimer() {
  return ((timer0_overflow_count << 8) + TCNT0) << 2;
}
