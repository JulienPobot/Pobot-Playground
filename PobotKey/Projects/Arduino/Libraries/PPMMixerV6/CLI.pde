byte contD,contT,contM;
extern float f2p,previousAngleP;

void cliParse() 
{
  byte command,verb=0,sub=0;

  if (Serial.available()) { 
    digitalWrite(13, LOW);
    command = Serial.read();
    if (Serial.available()) 
      verb = Serial.read(); // read or write
    if (Serial.available()) 
      sub = Serial.read();

    switch (command) {
      ///////////////////////////////  
    case 't': // trasmitter
      contT=true;
      break;

      ///////////////////////////////  
    case 'm': // motor
      contM=true;
      break;
      
      ////////////////////////
    case 'c': // Cycle
      Serial.println(cycleTime);
      break;
      
      ////////////////////////
    case 'f': // Free RAM
      Serial.print(freeRam());
      separator();
      Serial.println(allocRam());
      break;
      
      
      
      ////////////////////////
    case 'd': // Verify
      contD=true;
      break;
      
      ////////////////////////
    case 'x': // Stop output
    case ' ':
      contT=false;
      contM=false;
      break;
    }

    digitalWrite(13, HIGH);
  }

  ////////////////////////////////////////////////////////////////////
  if (contM)
  {
    for(int i;i<MAXCHANOUT;i++)
    {
      Serial.print(channelOut[i]);
      separator();
    }
    Serial.println();
  }
  if (contT)
  {
    for(int i;i<MAXCHANIN;i++)
    {
      Serial.print(channelIn[i]);
      separator();
    }
    Serial.println();
  }
  
}


















































