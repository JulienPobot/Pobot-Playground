/*  
 *  Copyright XD team(2009)
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
// code for XD robot's opponent detection (2009)
//
// Written by Nicolas Saugnier <nicolas.saugnier@esial.net>
// Based on or heavilly inspired by :
//  ...


// Uses the "Servo" software servo library found at http://www.arduino.cc/playground/ComponentLib/Servo , renamed SoftServo to avoid name conflicts.

#include "Wire.h" // I is a minion <_<"
#include "SoftServo.h" // software servo library for the beacon orientation
// copy it to arduino-xxxx/hardware/library/SoftServo


//-------------------

// pins declarations

#define sensorFrontRight 0  // 2Y0A02
#define sensorBackRight   1  // GP2D12
#define sensorFrontLeft  2  // 2Y0A02
#define sensorBackLeft   3  // GP2D12
#define sensorFront      6  // GP2D120
#define SENSOR_MASK(x) (1<<x)

#define BEEP_PIN  7
#define KEY_INTER_PIN 2

#define servoBeaconPin         8

#define beaconEnable           13
#define beaconNorth      3
#define beaconSouth      4
#define beaconWest       5
#define beaconEast       6


#define beaconPinOffset  beaconNorth  // the first of the direction pins, for easier
  //use in direction-independant calculations

// --------------------
// communication declarations

#define I2C_DETECTION_BOARD_ADRESS 5


#define STATUS_NO_OBSTACLE 50
#define STATUS_WARNING     51


#define ORDER_USE_BEACON     50    
#define ORDER_SELECT_SENSOR  51
#define ORDER_SELECT_BEEP_SEQ 54
#define ORDER_SOFT_RESET 55

#define REQUEST_NOTHING          0 // nothing to say... :/

#define REQUEST_DETECTION_STATUS        51 // status + recap of which sensor has reached its threshold, 2 bytes
    // for recap : one bit per sensor, 0 = no pb with this one, 1 = something is too close!
    // Each sensor has the bit correspinding to its pin number.
#define REQUEST_DETECTION_VALUE         52 // the distance, in mm (int) , 2 bytes
#define REQUEST_OPPONENT_ORIENTATION    53 // the relative orientation of the other robot, if the beacon is active in degrees (int) , 2 bytes
#define REQUEST_KEY_INTER_STATE         54 // the state of the key interrupter... nothing more here

//---------------------
// Sharp declartions

//sharp types
#define SHARP_D120 40
#define SHARP_D12 41
#define SHARP_Y0A02 42
// minimum threshold : we don't use values below (applies to the value read, which is between 0 and 1203)
#define SHARP_D120_MIN_TRESH 80
#define SHARP_D12_MIN_TRESH 80
#define SHARP_Y0A02_MIN_TRESH 120

//---------------------
// Basic obstcle detection thresholds (in mm, FROM THE SENSOR)
#define SENSOR_FRONT_RIGHT_TRESH 280
#define SENSOR_FRONT_LEFT_TRESH 280
#define SENSOR_BACK_RIGHT_TRESH -300
#define SENSOR_BACK_LEFT_TRESH -300
#define SENSOR_FRONT_TRESH 300

//----------------------
// soft pseudo-reset

void soft_reset(){
  // TODO

}



//---------------------
// beep seq
unsigned char beepSeqRewind[] = {90, 60, 0};
unsigned char beepSeqLol[] = {1, 1, 0};
unsigned char beepSeqWainting[] = {8, 43, 8, 130, 0};
unsigned char *beepSeqences[] = {NULL, beepSeqRewind, beepSeqLol, beepSeqWainting};

// use setSequence for set that
unsigned char *currentBeepSeq = NULL;
char cursorBeepSeq = -1;
char currentBeepValue = 0;
char beepState = 0;

void setBeepSequence(unsigned char* seq) {
  cli();
  currentBeepSeq = seq;
  if (seq!=NULL && seq[0]!=0) {
    cursorBeepSeq = -1;
    currentBeepValue = 0;
  } else {
    currentBeepSeq = NULL;
    digitalWrite(BEEP_PIN, LOW);
  }
  beepState = 0;
  sei();
}

void setNoBeepSeqence(unsigned char no) {
  int n = no;
  if (no<0 || no>sizeof(beepSeqences)/2) n = 0;
  setBeepSequence(beepSeqences[no]);
}

void sequenceBeepTick() {
  if (currentBeepSeq!=NULL) {
    if (currentBeepValue==0) {	// next
      cursorBeepSeq++;
      if (currentBeepSeq[cursorBeepSeq]==0) {	// sentinel
        cursorBeepSeq = 0;
	beepState = 1;
      } else {
        beepState = 1 - beepState;
      }
      currentBeepValue = currentBeepSeq[cursorBeepSeq];
      if (beepState) {
        digitalWrite(BEEP_PIN, HIGH);
      } else {
        digitalWrite(BEEP_PIN, LOW);
      }
    }
    currentBeepValue--;
  }
  /*Serial.print("cur : ");
  Serial.print(cursorBeepSeq);
  Serial.print("\tcv : ");
  Serial.println(currentBeepValue);*/
}   


//---------------------

class SharpSensor {
private:
  char pin;
  int treshold;
  char type;
  int delta;
  char dir;
  static SharpSensor* sensors[]; // 'sensor registry', to simplify acces to sensors, knowing their pin number
  static bool sensors_init;
public:
  SharpSensor(char pin, int type, int delta, char dir) {
    this->pin = pin;
    this->type = type;
    this->delta = delta;
    this->dir = dir;
    if(!sensors_init){
      for(int i = 0; i< 8; i++){
        sensors[i] = NULL;
      }
      sensors_init = true;  
    }
    sensors[pin] = this;
  }
  
  int sensorRead() {
    int value;
    switch (type) {
      case SHARP_D120:
        value = getSharpD120(pin);
        break;
      case SHARP_D12:
        value = getSharpD12(pin);
        break;
      case SHARP_Y0A02:
        value = getSharpY0A02(pin);
        break;
      default:
        value = 0;
        break;
    }
    return value*dir + delta;
  }
  
  void setTreshold(int tresh) {
    treshold = tresh;
  }
  
  bool isOverTreshold() {
    return dir*sensorRead() < treshold*dir;
  }
  
  static int readSensor(int sensor_pin){
    if(sensors[sensor_pin] != NULL){
      return sensors[sensor_pin]->sensorRead();  
    } else {
      return 0;
    }
  }
};
SharpSensor* SharpSensor::sensors[8];
bool SharpSensor::sensors_init = false;


//---------------------
SharpSensor sharpSensorFrontRight(sensorFrontRight, SHARP_Y0A02, -50, 1);
SharpSensor sharpSensorFrontLeft(sensorFrontLeft, SHARP_Y0A02, -50, 1);
SharpSensor sharpSensorBackRight(sensorBackRight, SHARP_D12, -50, -1);
SharpSensor sharpSensorBackLeft(sensorBackLeft, SHARP_D12, -50, -1);
SharpSensor sharpSensorFront(sensorFront, SHARP_D120, 100, 1);


unsigned char control_timer_begining; // value at which the timer begins in order to achieve the desired frequency with its overflow interruption

#define CONTROL_FREQ 100.0
#define TIMER2_CLOCK_FREQ 15625.0 // 15625Hz for /1024 prescale from 16MHz



// detection variables
char sensor_recap = 0; // recap of the status of the sensors, one bit per sensor
char status = STATUS_NO_OBSTACLE; 
int opponent_orientation = 0; // relative orientation of the other robot in degrees

// communication variables
boolean with_beacon = false; // do we use the beacon ?
char selected_sensor = 0; // which sensor value is used to answer the get_sensor_value request ?
volatile char requested_message = REQUEST_NOTHING;



// analogread filtered on 4 readings. Takes around 0.5ms
int filteredAnalogRead(int pin){
  int sum = analogRead(pin);
  sum += analogRead(pin);
  sum += analogRead(pin);
  sum += analogRead(pin);
  return sum / 4;
}

void set_use_beacon(bool use){
  with_beacon = use;
  digitalWrite(beaconEnable,use);
}

/*int readDistance(int sensor){
  int distance;
  switch(sensor){
    case sensorFrontRight:
    case sensorFrontLeft:
      distance = getSharpY0A02(sensor) - 50;
      break;
    case sensorBackRight:
    case sensorBackLeft:
      distance = - getSharpD12(sensor) - 50;
      break;
    case sensorFront:
      distance = getSharpD120(sensor) - 100;
      break;
    default:
      distance = 0;  
      break;
  }
  return distance;  
}*/


//------------------------------
// Sharp IR detectors range :
// 2Y0A02 : from 20 to 180 cm (FrontLeft and FrontRight)
// GP2D12 : from 10 to 80 cm  (BackLeft and BackRight)
// GP2D120 : from 3 to 40 cm  (Front)

int getSharpD120(int pin) {
  int V = filteredAnalogRead(pin);
  if (V<SHARP_D120_MIN_TRESH) V = SHARP_D120_MIN_TRESH;
  //if (V>SHARP_D120_MAX_TRESH) V = SHARP_D120_MAX_TRESH;
  int res = (29140 / (V + 5)) + 6;
  return res;
}

int getSharpD12(int pin) {
  int V = filteredAnalogRead(pin);
  if (V<SHARP_D12_MIN_TRESH) V = SHARP_D12_MIN_TRESH;
  int res = (67870L / (V - 3)) - 30;
  return res;
}

int getSharpY0A02(int pin) {
  int V = filteredAnalogRead(pin);
  if (V<SHARP_Y0A02_MIN_TRESH) V = SHARP_Y0A02_MIN_TRESH;
  int res = (158410L / (V + 8)) - 100;
  return res;
}


//------------------------------
// communication


char receive_char(){
  while(Wire.available()<1);
  return Wire.receive();
}  

int receive_int(){
  while(Wire.available() < 2);
  int i = Wire.receive();  
  int j = Wire.receive()<<8;
  j |= i;
  return j;
}

// clean the next nb bytes...
void receive_all(int nb){
  while(Wire.available()<nb);
  for(int i = 0; i < nb; i++){
    Wire.receive();
  }
}


void i2c_receive(int nb_received){
  // nb_received has the number of bytes we are going to have in the end
  bool cleanup = false;
  
  if(nb_received == 0) return; // is it even possible ?
  if(nb_received == 1){ //a one-byte message tells us what will be requested next
    requested_message = receive_char();
  } else {
    char cmd = receive_char();
    
    switch(cmd){
      case ORDER_USE_BEACON :
        if(nb_received == 2){
          set_use_beacon(receive_char());
        }else{cleanup=true;}
        break;
      case ORDER_SELECT_SENSOR:
        if(nb_received == 2){
          selected_sensor = receive_char();
        }else{cleanup=true;}
        break;
      case ORDER_SELECT_BEEP_SEQ:
        if(nb_received == 2){
	  setNoBeepSeqence(receive_char());
        } else {cleanup=true;}
        break;
      case ORDER_SOFT_RESET:
        soft_reset();
	break;
      default:
        cleanup = true;
        break;
    }
    if(cleanup){
      receive_all(nb_received-1);
      return;
    }
  }
}

// to send multiple-byte values via I2C...
void multisend(int val){
  int data[1];
  data[0] = val;
  Wire.send((byte*)data,sizeof(int));
}
void multisend(char val1, char val2){
  char data[2];
  data[0] = val1;
  data[1] = val2;
  Wire.send((byte*)data,2*sizeof(char));
}

void i2c_request(){
  // to know WHAT is requested, look at the requested_message var...
  switch(requested_message){
  case REQUEST_DETECTION_STATUS :
    multisend(status,sensor_recap);
    break;
  case REQUEST_DETECTION_VALUE :
    multisend( SharpSensor::readSensor(selected_sensor) ); 
    break;  
  case REQUEST_KEY_INTER_STATE :
    Wire.send( digitalRead(KEY_INTER_PIN) );
    break;
  case REQUEST_OPPONENT_ORIENTATION :
    multisend(opponent_orientation);
    break;
  case REQUEST_NOTHING :
  default:
    break; 
  }
  requested_message = REQUEST_NOTHING;
}


void init_i2c(){
  Wire.begin(I2C_DETECTION_BOARD_ADRESS); // join the I2C bus as a slave
  // register the handlers
  Wire.onReceive(i2c_receive);
  Wire.onRequest(i2c_request);
}


//-------------------------------------------

void init_control(){
  init_timer(CONTROL_FREQ); //control frequency 
}

// sets the values necessary to have the timer ticking (with its overflow interrupt) at the desired frequency
void init_timer(float frequency){
  //Calculate the timer load value
  control_timer_begining = (int)((256.0-(TIMER2_CLOCK_FREQ/frequency))+0.5); //the 0.5 is for rounding;
  
  // Timer2 setup
  TCCR2A = 0;         // normal mode
  TCCR2B = 1<<CS22 | 1<<CS21 | 1<<CS20;  // 1 1 1 sets the prescaler to /1024, cf ATmega168 datasheet
}

void control_start(){
  // Timer2 overflow interrupt enable;
  TIMSK2 = 1<<TOIE2;
  //load the timer for its first cycle
  TCNT2 = control_timer_begining; 
}

void control_stop(){
  // Timer2 overflow interrupt disable;
  TIMSK2 = 0<<TOIE2;
}


//Timer2 overflow interrupt vector handler
ISR(TIMER2_OVF_vect) {
  if(with_beacon){
    SoftServo::refresh(); 
  }

  sequenceBeepTick();

  TCNT2 = control_timer_begining; // reload timer
}

SoftServo beacon_servo;

void setup_beacon(){
  set_use_beacon(true);
  
  // for servo
  init_control();  
  control_start();
  beacon_servo.attach(servoBeaconPin);
  
  
  pinMode(beaconEnable, OUTPUT);
  digitalWrite(beaconEnable,LOW); // not enabled by default
  
  pinMode(beaconNorth, INPUT);
  digitalWrite(beaconNorth,HIGH); // pullup
  pinMode(beaconSouth, INPUT);
  digitalWrite(beaconSouth,HIGH); 
  pinMode(beaconEast, INPUT);
  digitalWrite(beaconEast,HIGH);
  pinMode(beaconWest, INPUT);
  digitalWrite(beaconWest,HIGH);
}

void setup_sensor_treshold() {
  sharpSensorFrontRight.setTreshold(SENSOR_FRONT_RIGHT_TRESH);
  sharpSensorFrontLeft.setTreshold(SENSOR_FRONT_LEFT_TRESH);
  sharpSensorBackRight.setTreshold(SENSOR_BACK_RIGHT_TRESH);
  sharpSensorBackLeft.setTreshold(SENSOR_BACK_LEFT_TRESH);
  sharpSensorFront.setTreshold(SENSOR_FRONT_TRESH);
}

void performe_detection(){
  if(with_beacon){
    // TODO do all the needed stuff to update orientation 
    // and also maybe use this information to modify the thresholds
  }
  
  sensor_recap = 0;
  bitWrite(sensor_recap, sensorFrontRight, sharpSensorFrontRight.isOverTreshold());
  bitWrite(sensor_recap, sensorFrontLeft, sharpSensorFrontLeft.isOverTreshold());
  bitWrite(sensor_recap, sensorBackRight, sharpSensorBackRight.isOverTreshold());
  bitWrite(sensor_recap, sensorBackLeft, sharpSensorBackLeft.isOverTreshold());
  bitWrite(sensor_recap, sensorFront, sharpSensorFront.isOverTreshold());
  
  if (sensor_recap!=0) {
    status = STATUS_WARNING;
  } else {
    status = STATUS_NO_OBSTACLE;
  }
}

//-----------------------------
//       sequences
//------------------------------
void seq_print_sensors(){
  Serial.print( (float)SharpSensor::readSensor( sensorFrontRight )/10);
  Serial.print("\t");
  Serial.print( (float)SharpSensor::readSensor( sensorFrontLeft )/10);
  Serial.print("\t");
  Serial.print( (float)SharpSensor::readSensor( sensorBackRight )/10);
  Serial.print("\t");
  Serial.print( (float)SharpSensor::readSensor( sensorBackLeft )/10);
  Serial.print("\t");
  Serial.println( (float)SharpSensor::readSensor( sensorFront )/10);
  delay(100);
}

void seq_servo_basic(){
  delay(1000);
  beacon_servo.write(10);
  delay(1000);
  beacon_servo.write(140);
}

void seq_print_sensor_recap() {
  Serial.print("sensor_recap : ");
  Serial.println(sensor_recap, BIN);
  delay(100);
}

//---------------------------
//    MAIN
//---------------------------

void (*sequence)() = NULL;

void setup(){
  Serial.begin(38400);
  Serial.println("start");
  init_i2c();
  setup_beacon();
  setup_sensor_treshold();
  
  //key interrupter
  pinMode(KEY_INTER_PIN, INPUT);
  digitalWrite(KEY_INTER_PIN, HIGH);
  
  // buzzer
  pinMode(BEEP_PIN, OUTPUT);
  //setBeepSequence(beepSeqRewind);
  
  set_use_beacon(false); // beacon inactive by default to save energy...
}


void loop(){
  performe_detection();
  
  if(Serial.available()){
    char cmd = Serial.read();
    delay(10);
    switch(cmd){
      case 's': 
        {
          Serial.print("Sensor Front Right ");
          Serial.println(filteredAnalogRead( sensorFrontRight ));
          Serial.print("Sensor Front Left  ");
          Serial.println(filteredAnalogRead( sensorFrontLeft ));
          Serial.print("Sensor Back Right  ");
          Serial.println(filteredAnalogRead( sensorBackRight ));
          Serial.print("Sensor Back Left   ");
          Serial.println(filteredAnalogRead( sensorBackLeft ));
        }
        break;
      case 'c':
        sequence = &seq_print_sensors;
        break;  
      case 'b':
        sequence = &seq_servo_basic;
        break;  
      case 'p':
	sequence = &seq_print_sensor_recap;
	break;
      case 'r':
        sequence = NULL;
        break;
      case 'y':
        setNoBeepSeqence(Serial.read()-'0');
        break;
      default:
        break;
    }
  }
  
  if (sequence !=NULL){
    (*sequence)();
  }
  
}










