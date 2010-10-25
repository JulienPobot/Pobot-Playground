/*
This is driver/utility program to use  with mindsensors.com DIST-NX sensor.
V1.2
This Demo use the adress of 0x02 for the DIST-nxt

written by Dr. Nitin Patil
(c) mindsensors.com 2006, 2007
*/

/*

NXT I2C register storage

location      Rd  contents          Wr  contents
--------------------------------------------------
0-7        v2.0              -
8-f        mndsnsrs            -
10-18      DIST              -


0x41                                  command
0x42      lsb Distance data            -
0x43      msb Distance data            -
0x44      lsb Voltage data            -
0x45      msb Voltage data            -

0x50  CUSTOM_curve no of DataPoint
0x51  CUSTOM_curve  minDistance  LSB
0x52  CUSTOM_curve  minDistance  MSB

0x53 CUSTOM_curve  maxDistance  LSB
0x54 CUSTOM_curve  maxDistance  MSB

0x55 CUSTOM_curve  Distance  LSB          first Point
0x56 CUSTOM_curve  Distance  MSB     }
0x57 CUSTOM_curve  Voltage  LSB        }
0x58 CUSTOM_curve  Voltage  MSB      }

:
:
:
:
:
....... rest  Data points time.



Commands                Action
ASCII   Hex
N      0x4E            Set ADPA mode On
O      0x4F            Set ADPA mode Off (default)
E      0x45            Sensor Power ON
D      0x44            SEnsor Power Off

1      0x31       GP2D12
2      0x32       GP2D120
3      0x33       GP2YA21
4      0x34       GP2YA02
5      0x35       CUSTOM curve



DIST-Nx uses user configurable I2C bus address. If you would like the set different
address than shipped one then sequence of 0xA0, 0xAA, 0xA5, <new I2C address> needs
to be written on the command register without any brake (read operation).
New address is effective immediately


*/

#define          DistID                0x02
#define          DistCommandReg        0x41
#define          DistReadResult        0x42
#define          DistReadDistance      0x42
#define          DistReadVoltage       0x44

#define          DISTON                   0x45
#define          DISTOFF                  0x44
#define          GP2D12                   0x31
#define          GP2D120                  0x32
#define          GP2YA21                  0x33
#define          GP2YA02                  0x34
#define          CUSTOM                   0x35

#define          SHORT                  0x32
#define          MEDIUM                  0x33
#define          LONG                  0x34


#define           DistPort                S1    // Connect sensor to this Port!!

//
// Counter to keep track of the number of times a button is pressed.
//
int nLeftButton  = 0;
int nRightButton = 0;
int nEnterButton = 0;
int nExitButton  = 0;


task ButtonTask()
{
  //
  // Set task to highest priority so that it will run to completion. This will minimze the small
  // chance of task execution beeing delayed until after button is released; if this happens, then
  // the keypress will be lost.
  //
  nSchedulePriority   = kHighPriority;

  //
  // Update count of times each button is pushed. If using this program as a template, replace these
  // statements with your own custom action.
  //
  switch (nNxtButtonPressed)
  {
    case kLeftButton:    ++nLeftButton;         break;
    case kRightButton:  ++nRightButton;        break;
    case kEnterButton:  ++nEnterButton;        break;
    case kExitButton:    ++nExitButton;        break;
  }
  return;
}


//////////////////////////////////////////////////////////////////////////////
//
//     Read the Distance  data from Dist sensor
//
//
/////////////////////////////////////////////////////////////////////////////

int  GetDistance()
{
      byte replyMsg[2];
      int Distance;
      byte  distMsg[5];
      const byte MsgSize = 0;
      const byte DistAddress = 1;
      const byte ReadAddress = 2;
      distMsg[MsgSize] = 2;
      distMsg[DistAddress] = DistID;
      distMsg[ReadAddress] = DistReadDistance;

      while (nI2CStatus[DistPort] == STAT_COMM_PENDING);           // Wait for I2C bus to be ready
      if ( nI2CStatus[DistPort] != NO_ERR ) {                     // probably sensor is missing.
        nxtDisplayTextLine(6, ">>00E: %d", nI2CStatus[DistPort]);
        return (-1);
      }

      sendI2CMsg(DistPort, distMsg[0], 2);                        // Send the message

      while (nI2CStatus[DistPort] == STAT_COMM_PENDING);          // Wait for I2C bus to be ready
      if ( nI2CStatus[DistPort] != NO_ERR ) {                     // probably sensor is missing.
        nxtDisplayTextLine(6, ">>00E: %d", nI2CStatus[DistPort]);
        return (-1);
      }

      readI2CReply(DistPort, replyMsg[0], 2);
      if ( replyMsg[0] == -1 ) { // -1 is reserved to report errors, so
        Distance = 0; // change it to zero.
      }
      else{
        Distance = ( 0x00FF & replyMsg[0] );
        Distance += ( (0x00FF & replyMsg[1]) <<8 );
      }

      return(Distance);
}

//////////////////////////////////////////////////////////////////////////////
//
//      Read the Distance  data from Dist sensor
//
//
/////////////////////////////////////////////////////////////////////////////

int  GetVoltage()
{
  byte replyMsg[2];
  int Voltage;
  byte distMsg[5];
  const byte MsgSize      = 0;
  const byte DistAddress  = 1;
  const byte ReadAddress  = 2;

  distMsg[MsgSize] = 2;
  distMsg[DistAddress] = DistID ;
  distMsg[ReadAddress] = DistReadVoltage ;

  while (nI2CStatus[DistPort] == STAT_COMM_PENDING);          // Wait for I2C bus to be ready
  if ( nI2CStatus[DistPort] != NO_ERR ) {                     // probably sensor is missing.
    return (-1);
  }

  sendI2CMsg(DistPort, distMsg[0], 2);                        // Send the message

  while (nI2CStatus[DistPort] == STAT_COMM_PENDING);          // Wait for I2C bus to be ready
  if ( nI2CStatus[DistPort] != NO_ERR ) {                     // probably sensor is missing.
    return (-1);
  }

  readI2CReply(DistPort, replyMsg[0], 2);
  if ( replyMsg[0] == -1 ) { // -1 is reserved to report errors, so
    Voltage = 0; // change it to zero.
  }
  else{
    Voltage = ( 0x00FF & replyMsg[0] );
    Voltage += ( (0x00FF & replyMsg[1]) <<8 );
  }
  return(Voltage);
}


//////////////////////////////////////////////////////////////////////////////
//
//     send command to  Dist sensor
//
//
/////////////////////////////////////////////////////////////////////////////

char distCommand(byte distCommand)
{
  byte distMsg[5];
  const byte MsgSize            = 0;
  const byte DistAddress        = 1;
  const byte ReadAddress        = 2;
  const byte CommandAddress     = 2;
  const byte Command = 3;

  // Build the I2C message
  distMsg[MsgSize] = 3;
  distMsg[DistAddress]  = DistID;
  distMsg[CommandAddress] = DistCommandReg ;
  distMsg[Command] = distCommand;

  while (nI2CStatus[DistPort] == STAT_COMM_PENDING);                // Wait for I2C bus to be ready

  if ( nI2CStatus[DistPort] != NO_ERR ) {                           // probably sensor is missing.
    return (-1);
  }
  sendI2CMsg(DistPort, distMsg[0], 0);                              // Send the message
  return (1);
}

void i2c_flush(tSensors s)
{
  int n;
  byte dump[8];

  while (nI2CStatus[s] == STAT_COMM_PENDING)
    ;
  n = nI2CBytesReady[s];
  while (n > 0) {
    while (nI2CStatus[s] == STAT_COMM_PENDING)
      ;
    readI2CReply(s, dump[0], n);
    while (nI2CStatus[s] == STAT_COMM_PENDING)
      ;
    n = nI2CBytesReady[s];
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//      Display the heading on Nxt screen
//
/////////////////////////////////////////////////////////////////////////////

task main()
{
  nNxtButtonTask    = ButtonTask;
  nNxtExitClicks  = 3; // Triple clicking EXIT button will terminate program
  int Distance,Voltage;
  SensorType[DistPort] = sensorI2CCustom9V;
  nI2CBytesReady[DistPort] = 0;

  i2c_flush ( DistPort );

/* ***********************************
 * NOTE:
 * The sensor type must be configured properly for 
 * the sensor to provide meaningful readings.
 * select the correct sensor type below
 * by un-commenting appropriate line.
 * ************************************
 */
  distCommand(LONG);  //use this for long range sensor.
  //    distCommand(MEDIUM);  //use this for Medium range sensor.
//      distCommand(SHORT);  //use this for short range sensor.

  // power the sensor ON.
  distCommand(DISTON);
  nxtDisplayTextLine(0, "mindsensors.com");
  nxtDisplayTextLine(1, "Dist sensor Demo");
  nxtDisplayTextLine(2, "use Port: 1");

  while(!nExitButton)
  {
    Distance = 0;
    Distance = GetDistance();                                         //read the distance  in mm
    nxtDisplayTextLine(4,"Distance= %d mm", Distance);               // display the distance
    Voltage = 0;
    Voltage = GetVoltage();                                           //read the voltage in mV
    nxtDisplayTextLine(5,"Voltage= %d mV", Voltage);                 //display the voltage
  }

  // power of the sensor.
  distCommand(DISTOFF);
  StopAllTasks();
}
