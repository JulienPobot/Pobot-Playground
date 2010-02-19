

void setup(void)
{
   Serial.begin(9600);  
   byte val2 = -5; 
   
   byte val3 = 0b01000000;
   byte val4 = 0b10000000;
   
   Serial.print("> -5 : ");
   Serial.println(val2,BIN);
   
   Serial.print("> -5*2 : ");
   Serial.println(2*val2,BIN);
   

}

void loop(void)
{


}

