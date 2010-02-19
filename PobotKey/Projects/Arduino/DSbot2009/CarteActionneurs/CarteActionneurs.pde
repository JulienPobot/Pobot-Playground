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
// code for XD robot's motion control (2009)
//
// Written by Nicolas Saugnier <nicolas.saugnier@esial.net>
// Based on or heavilly inspired by :
//    Pololu Serial Servo Controller code by Mike Crist (2009-01-03)

#include "Wire.h" // I is teh MASTER \o/
#include <SoftwareSerial.h>
#include "Match.h"
#include "StatusList.h"

// uncomment this to enter in debug mode
#define DEBUG

#ifdef DEBUG
#define PRINT(PRINT__X, ...)	Serial.print(PRINT__X, ## __VA_ARGS__)
#define PRINTLN(PRINT__X, ...)	Serial.println(PRINT__X, ## __VA_ARGS__)
#define SERIAL_AVAILABLE()	Serial.available()
#else
#define PRINT(PRINT__X, ...)		
#define PRINTLN(PRINT__X, ...)	
#define SERIAL_AVAILABLE()	false
#endif

//digital pins
#define servoControllerRxPin     12
#define servoControllerTxPin     13
#define servoControllerResetPin  9

#define startingCordPin   6
#define colorSwitch       7
#define ledGreen         11
#define ledRed           10

#define contactorFrontLeft   2
#define contactorFrontRight  3
#define contactorBackLeft    4
#define contactorBackRight   5

//analog
#define presenceElement 0
#define sharpElementRight 1
#define sharpElementLeft 2
#define voltageBatteryPin 3

//servos on the pololu controller
#define servoFrontLeft 0
#define servoFrontRight 1
#define servoBackLeft 2
#define servoBackRight 3
// 4 and 5 free
#define servoLifting 6
#define servoPliers 7

//----------------------------------------
#define I2C_MOTOR_BOARD_ADRESS 3
#define I2C_DETECTION_BOARD_ADRESS 5


// communication with the control card

#define USE_ANGLE_PID 0x01
#define USE_DIST_PID  0x02

#define SEARCH_LINE   0x01
#define STOP_ON_LINE_FOUND 0x02

// things to read :
#define REQUEST_NOTHING       0 // nothing to say... :/

#define REQUEST_THETA         3 // angle in 1/100 of degrees, 2 bytes
#define REQUEST_XY            4 // x, y in mm, 4 bytes
#define REQUEST_XYTHETA       5 // x, y in mm, angle in 1/100 of degrees, 6 bytes

#define REQUEST_STATUS        6 // current status of the controler, 1 byte

#define REQUEST_LINE_POS      8 // position of the last line seen (or 0, 0 if not found), (x,y) in mm, 4 bytes

#define REQUEST_PID_USAGE     9 // uses dist pid?(byte 1), uses angle pid(byte 0), 1 char

#define REQUEST_LINE_SENSORS  11 // values of the line sensors (l, r, c), 6 bytes


// all orders have 1 byte for the order_id, and the rest if for data
#define ORDER_SET_MODE   1  // set the current mode (aka the controller, 2 bytes
#define ORDER_FIND_LINE  2  // find line (bit0)? stop on line ?(bit1), 2 bytes 

#define ORDER_SET_X      3 // set x in mm, 3 bytes
#define ORDER_SET_Y      4 // set y in mm, 3 bytes
#define ORDER_SET_THETA  5 // set theta in degrees, 3bytes

#define ORDER_PID_USAGE  6 // use dist pid?(byte 1), use angle pid(byte 0), 2 bytes

#define ORDER_MOVEMENT_CMD 7 // the commend id(char), and a param(int) , 4 bytes

#define ORDER_MOTOR_SOFT_RESET 8	// performe a soft reset on motor board

#define ORDER_QUAD_DIST_FIRST_ORDER 9	// change max speed value
#define ORDER_QUAD_ANGLE_FIRST_ORDER 10 // change max speed value


#define MODE_STANDBY         's'
#define MODE_CONSIGN         'c'
#define MODE_LINE_FOLLOWING  'l'
#define MODE_ALIGNMENT       'a'


#define CMD_STOP 1
#define CMD_TURN 2
#define CMD_TURN_ABS 3
#define CMD_GO   4
#define CMD_WAIT 5

#define sensorFrontRight 0  // 2Y0A02
#define sensorBackRight  1  // GP2D12
#define sensorFrontLeft  2  // 2Y0A02
#define sensorBackLeft   3  // GP2D12
#define sensorFront      6  // GP2D120
#define SENSOR_MASK(x) (1<<x)

#define STATUS_NO_OBSTACLE 50
#define STATUS_WARNING     51

#define ORDER_USE_BEACON     50    
#define ORDER_SELECT_SENSOR  51
#define ORDER_SELECT_BEEP_SEQ 54
#define ORDER_SENSOR_SOFT_RESET 55	// performe a soft reset on sensor board

#define REQUEST_DETECTION_STATUS 51 // status + recap of which sensor has reached its threshold, 2 bytes
// for recap : one bit per sensor, 0 = no pb with this one, 1 = something is too close!
// Each sensor has the bit correspinding to its pin number.
#define REQUEST_DETECTION_VALUE       52 // the distance, in mm (int) , 2 bytes
#define REQUEST_OPPONENT_ORIENTATION  53 // the relative orientation of the other robot, if the beacon is active in degrees (int) , 2 bytes
#define REQUEST_KEY_INTER_STATE       54

#define MIN_BATTERY_VOLTAGE 7.1

char detection_sensors_status; // updated when we request the detection status
char detection_sensors_recap; // updated when we request the detection status

//=========================
// value of speed
#define DIST_QUAD_1ST_ACC 13000
#define ANGLE_QUAD_1ST_ACC 10000
#define DIST_MAX_SPEED 312
#define ANGLE_MAX_SPEED 240


//====================
// sensor of element presence

#define presenceElementNothing	0
#define presenceElementGreen	1
#define presenceElementRed	2
#define presenceElementHere	3

#define presenceElementNG	600
#define presenceElementGR	300
#define presenceElementRH	50


//==========
#define PHASE_TEST               0
#define PHASE_WAITING_TO_START  1
#define PHASE_IN_GAME            2
#define PHASE_THIS_IS_THE_END    3
#define PHASE_VERIF              4
#define PHASE_SETUP              5

volatile char game_phase = PHASE_TEST;

volatile unsigned long time_end_match;

unsigned char control_timer_begining = 0; // value at which the timer begins in order to achieve the desired frequency with its overflow interruption

#define CONTROL_FREQ 80.0
#define TIMER2_CLOCK_FREQ 15625.0 // 15625Hz for /1024 prescale from 16MHz

Match match; // the match we are playing...


//----------------------------------------

//----------------------------------------
//fonction to set max speed value
//----------------------------------------
void setQuadDistFirst(int speed_) {
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_QUAD_DIST_FIRST_ORDER, speed_);
}

void setQuadAngleFirst(int speed_) {
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_QUAD_ANGLE_FIRST_ORDER, speed_);
}

//----------------------------------------
//fonction to perform a soft reset on boards
//----------------------------------------
void softResetMotorBoard() {
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOTOR_SOFT_RESET, (char)0);
}

void softResetDetectionBoard() {
  send_order(I2C_DETECTION_BOARD_ADRESS, ORDER_SENSOR_SOFT_RESET, (char)0);
}

//----------------------------------------
// fonction to read the sharp
// ---------------------------------------
#define SHARP_D12_MIN_TRESH 80

int getSharpD12(int pin) {
  int V = filteredAnalogRead(pin);
  if (V<SHARP_D12_MIN_TRESH) V = SHARP_D12_MIN_TRESH;
  int res = (67870L / (V - 3)) - 30;
  return res;
}

//-----------------------------------------
// fonction to set sequence beep
//-----------------------------------------
void setBeepSequence(char seq) {
  send_order(I2C_DETECTION_BOARD_ADRESS, ORDER_SELECT_BEEP_SEQ, seq );
}

//---------------------------------------
// fonction to read the battery voltage
// --------------------------------------

double getBatteryVoltage() {
  int value = filteredAnalogRead(voltageBatteryPin);
  return (double)value/83.5;
}

//----------------------------------------
// fonction to get position of side element
//----------------------------------------

char getPosSideElement(int sensor) {
  int value = getSharpD12(sensor);
  char res;
  if (value<180) {
    res = 1;
  } 
  else if (value<430) {
    res = 2;
  } 
  else {  // nothing
    res = 3;
  }
  return res;
}

// more high level
char get_colorwise_pos_side_elem() {
  if (match.get_color() == COLOR_GREEN) {
    return getPosSideElement(sharpElementLeft);
  } 
  else {
    return getPosSideElement(sharpElementRight);
  }
}

//---------------------------------------
// fonction to get the status of element presense
//----------------------------------------

int getPresenceElement() {
  int value = filteredAnalogRead(presenceElement);
  int res;
  if (value>presenceElementNG) {
    res = presenceElementNothing;
  } 
  else if (value>presenceElementGR) {
    res = presenceElementGreen;
  } 
  else if (value>presenceElementRH) {
    res = presenceElementRed;
  } 
  else {
    res = presenceElementHere;
  }

  return res;
}

//------------------------------------------
//           color selection
//------------------------------------------

char get_color_switch(){
  return (digitalRead(colorSwitch) == SWITCH_TO_COLOR);
}

void show_color(){
  if(match.get_color() == COLOR_GREEN){
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledRed, LOW);
  } 
  else {
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);
  }  
}

void set_color(char col){
  match.set_color(col);
  show_color();
}

//-----------------------------------------
//                SERVOS
//-----------------------------------------

class ServoPos {
public:
  int init;
  int medium;
  int final;
};

//------------------------------------------
//   descrition of the ranges of the servos
//------------------------------------------

ServoPos servo_pos[8];

void init_servo_pos(){

  servo_pos[servoFrontLeft].init = 1340;
  servo_pos[servoFrontLeft].medium = 3430;
  servo_pos[servoFrontLeft].final = 4800;

  servo_pos[servoFrontRight].init = 4760;
  servo_pos[servoFrontRight].medium = 1250;
  servo_pos[servoFrontRight].final = 950;

  servo_pos[servoBackLeft].init = 1560;
  servo_pos[servoBackLeft].medium = 2320;
  servo_pos[servoBackLeft].final = 4180;

  servo_pos[servoBackRight].init = 4260;
  servo_pos[servoBackRight].medium = 3470; 
  servo_pos[servoBackRight].final = 1790;

  servo_pos[4].init = 3000;
  servo_pos[4].medium = 3000;
  servo_pos[4].final = 3000;

  servo_pos[5].init = 3000;
  servo_pos[5].medium = 3000;
  servo_pos[5].final = 3000;

  servo_pos[servoLifting].init = 4900;
  servo_pos[servoLifting].medium = 2100;
  servo_pos[servoLifting].final = 500;

  servo_pos[servoPliers].init = 4380;
  servo_pos[servoPliers].medium = 3350;
  servo_pos[servoPliers].final = 2030;

}



//-----------------------------

void grabbers_reset();
void grabbers_first_time();
void grabbers_second_time();
void grabbers_before_push_second_element();
void grabbers_push_second_element();

void pushers_reset();
void pushers_lock_first_element();
void pushers_push_center();
void pushers_push_right();
void pushers_push_left();

bool closer_released = true;
void closer_reset();
void closer_close();

void lifter_reset();
void lifter_lift();
void lifter_drop();

void reset_all();


void reset_all(){
  grabbers_reset();
  pushers_reset();
  closer_reset();
  lifter_reset();
}



void grabbers_reset(){
  set_speed(servoFrontLeft, 0);
  set_speed(servoFrontRight, 0);
  position_absolute(servoFrontLeft,servo_pos[servoFrontLeft].init);
  position_absolute(servoFrontRight,servo_pos[servoFrontRight].init);
}
void grabbers_first_time(){
  set_speed(servoFrontLeft, 40);
  set_speed(servoFrontRight, 80);
  position_absolute(servoFrontLeft,servo_pos[servoFrontLeft].final);
  position_absolute(servoFrontRight,servo_pos[servoFrontRight].final);
}
void grabbers_second_time(){
  set_speed(servoFrontLeft, 50);
  set_speed(servoFrontRight, 100);
  position_absolute(servoFrontLeft,servo_pos[servoFrontLeft].medium);
  position_absolute(servoFrontRight,servo_pos[servoFrontRight].medium);
}
void grabbers_before_push_second_element(){
  set_speed(servoFrontLeft, 30);
  set_speed(servoFrontRight, 30);
  position_absolute(servoFrontLeft,servo_pos[servoFrontLeft].medium-150);
  position_absolute(servoFrontRight,servo_pos[servoFrontRight].medium+150);
}

void grabbers_push_second_element(){
  set_speed(servoFrontLeft, 20);
  set_speed(servoFrontRight, 20);
  position_absolute(servoFrontLeft,servo_pos[servoFrontLeft].final);
  position_absolute(servoFrontRight,servo_pos[servoFrontRight].final);
}

void pushers_reset(){
  set_speed(servoBackLeft, 80);
  set_speed(servoBackRight, 80);
  position_absolute(servoBackLeft,servo_pos[servoBackLeft].init);
  position_absolute(servoBackRight,servo_pos[servoBackRight].init);
}
void pushers_lock_first_element(){
  set_speed(servoBackLeft, 60);
  set_speed(servoBackRight, 60);
  position_absolute(servoBackLeft,servo_pos[servoBackLeft].medium);
  position_absolute(servoBackRight,servo_pos[servoBackRight].medium);
}
void pushers_push_center(){
  set_speed(servoBackLeft, 80);
  set_speed(servoBackRight, 85);
  position_absolute(servoBackRight,servo_pos[servoBackRight].final);
  position_absolute(servoBackLeft,servo_pos[servoBackLeft].final);
}
void pushers_push_right(){
  set_speed(servoBackLeft, 80);
  set_speed(servoBackRight, 80);
  position_absolute(servoBackLeft,servo_pos[servoBackLeft].final);
  delay(600);
  position_absolute(servoBackRight,servo_pos[servoBackRight].final);
}
void pushers_push_left(){
  set_speed(servoBackLeft, 80);
  set_speed(servoBackRight, 80);
  position_absolute(servoBackRight,servo_pos[servoBackRight].final);
  delay(600);
  position_absolute(servoBackLeft,servo_pos[servoBackLeft].final);
}


void closer_reset(){
  set_speed(servoPliers, 0);
  position_absolute(servoPliers,servo_pos[servoPliers].init);
  closer_released = true;
}
void closer_close(){
  set_speed(servoPliers, 60);
  position_absolute(servoPliers,servo_pos[servoPliers].final);
  closer_released = false;
}
bool closer_is_released() {
  return closer_released;
}



void lifter_reset(){
  set_speed(servoLifting, 0);
  position_absolute(servoLifting,servo_pos[servoLifting].init);
}
void lifter_lift(){
  set_speed(servoLifting, 85);
  position_absolute(servoLifting,servo_pos[servoLifting].final);
}
void lifter_drop(){
  set_speed(servoLifting, 55);
  position_absolute(servoLifting,servo_pos[servoLifting].medium);
}
void lifter_clear(){
  set_speed(servoLifting, 55);
  position_absolute(servoLifting,servo_pos[servoLifting].init - 1400);
}


// take the first column element
// pre: empty, grabbers and pushers are reset and their is an element in place for taking.
// post: one element inside, pushers locking inside the element in pos 1, grabbers are reset.
void take_first(){
  grabbers_first_time();
  delay(2000); 
  pushers_lock_first_element();
  delay(100);
  grabbers_reset();
}

// take the second column element
// pre: one elmement inside, pushers locking the element in pos 1, grabbers are reset, and their is an element present for taking
//post: full, pushers locking element in pos 1, grabbers locking the element in pos 2.
void take_second(){
  grabbers_second_time();
  delay(1000);
}

// promote the second column element to the first position
//pre: one element in pos 2, pushers reset, grabbers locking element in pos 2.
//post: one element in pos 1, pushers locking element in pos 1, grabbers reset
void promote_second(){
  grabbers_push_second_element();
  delay(1000);
  pushers_lock_first_element();
  delay(100);
  grabbers_reset();
}

//-------------------------------
// push sequences are the complete sequences to create one storey  
//-------------------------------

// Two element- storey : right, then promote, then left....
void push_normal(){
  pushers_push_right();
  delay(2000);
  promote_second();
  pushers_push_left();
  delay(1000);
}
//one elemnt-storey, in the middle
void push_middle(){
  // TODO
}

//
//========================

// resets the pliers : open and down
void pliers_reset(){
  lifter_reset();
  closer_reset();
  delay(500);
}

// drops carefully what has been lifted
//pre: pliers closed and up
//post: pliers open and down
void pliers_drop(){
  lifter_drop(); // lower enough for smooth dropping
  delay(500);
  closer_reset();
  delay(1000);
  lifter_reset();
  delay(500);
}

// close pliers
// pre: pliers open
//post: pliers  closed
void pliers_close(){
  closer_close();
  delay(2000);
}

//lift pliers
//pre: pliers down
//post:pliers up
void pliers_lift(){
  lifter_lift();
  delay(1000);
}

// put_xxx are the topmost sequences
//===============================
void put_first_time(){
  pliers_lift();
  push_normal();
  pliers_reset();
}

// close, lift, put lright then left, release
void put_normal(){
  pliers_close();
  pliers_lift();
  push_normal();
  pliers_drop();
}

// close, lift, put middle, release, and do it again.
void put_middle(){
  pliers_close();
  pliers_lift();
  push_middle();
  pliers_drop();

  promote_second(); // can be improved by  paralelising this action with the pliers_drop/close/lift

  pliers_close();
  pliers_lift();
  push_middle();
  pliers_drop();
}



// set up a new serial port for Pololu Servo Controller
SoftwareSerial ServoSerial =  SoftwareSerial(servoControllerRxPin, servoControllerTxPin);

//-----------------------------------------------

// analogread filtered on 4 readings. Takes around 0.5ms
int filteredAnalogRead(int pin){
  int sum = analogRead(pin);
  sum += analogRead(pin);
  sum += analogRead(pin);
  sum += analogRead(pin);
  return sum / 4;
}


int getPresenceValue(){
  return analogRead(presenceElement);
}

boolean startingCordIsIn(){
  return digitalRead(startingCordPin) == HIGH ;
}


boolean contactorIsPressed(int contactor){
  return digitalRead(contactor) == LOW;
}

boolean elementPresent(){
  int val = getPresenceValue();
  return val > 512;
  //return val > 512; // modified with interrupter
}

boolean isDocked(){
  return contactorIsPressed(contactorBackLeft) && contactorIsPressed(contactorBackRight);
}

boolean isOnBorder(){
  return contactorIsPressed(contactorFrontLeft) && contactorIsPressed(contactorFrontRight);
}


//----------------------------

char nb_elements = 0;

bool isEmpty(){
  return nb_elements <= 0;
}

bool isFull(){
  return nb_elements >= 2;
}



//------------------------------------------------
// Communication with Pololu Serial Servo Controller
//------------------------------------------------

void set_speed(byte servo, byte speedVal){
  //this function uses pololu mode command 1 to set speed
  //servo is the servo number (typically 0-7)
  //speedVal is servo speed (1=slowest, 127=fastest, 0=full)
  //set speedVal to zero to turn off speed limiting

  speedVal = speedVal & 0x7f; //take only lower 7 bits of the speed

  //Send a Pololu Protocol command
  cli();
  ServoSerial.print(0x80,BYTE);     //start byte
  ServoSerial.print(0x01,BYTE);     //device id
  ServoSerial.print(0x01,BYTE);     //command number
  ServoSerial.print(servo,BYTE);    //servo number
  ServoSerial.print(speedVal,BYTE); //speed
  sei();
}

// setting position. this directly sets on the servo.
void position_absolute(byte servo, int angle){
  //this function uses pololu mode command 4 to set absolute position
  //servo is the servo number (typically 0-7)
  //angle is the absolute position from 500 to 5500

  unsigned int temp;
  byte pos_hi,pos_low;

  temp = angle & 0x1f80;  //get bits 8 thru 13 of position
  pos_hi = temp >> 7;     //shift bits 8 thru 13 by 7
  pos_low = angle & 0x7f; //get lower 7 bits of position

  //Send a Pololu Protocol command
  cli();
  ServoSerial.print(0x80, BYTE);    //start byte
  ServoSerial.print(0x01, BYTE);    //device id
  ServoSerial.print(0x04, BYTE);    //command number
  ServoSerial.print(servo, BYTE);   //servo number
  ServoSerial.print(pos_hi, BYTE);  //bits 8 thru 13
  ServoSerial.print(pos_low, BYTE); //bottom 7 bits
  sei();
}

void set_servo_off(byte servo){
  //this function uses command 0 to stop a servo 

    //Send a Pololu Protocol command
  cli();
  ServoSerial.print(0x80,BYTE);       //start byte
  ServoSerial.print(0x01,BYTE);       //device id
  ServoSerial.print(0x00,BYTE);       //command number
  ServoSerial.print(servo,BYTE);       //servo number
  ServoSerial.print(0x00,BYTE); // 0, off, forward, and the 5 last = range = 0
  sei();
}

void reset_servo_controller(){
  digitalWrite(servoControllerResetPin, LOW);
  delay(100);
  digitalWrite(servoControllerResetPin, HIGH);
}

void stop_all_servos(){
  for(int i = 0; i<8; i++){
    set_servo_off(i);
  }  
}

//-------------------------------------------
//            Actuators Sequences
//-------------------------------------------


// for each servo,  go to each position alternatively
void seq_0(){
  for(int i = 0; i<8; i++){
    position_absolute(i,servo_pos[i].init);
  }
  delay(3000);
  for(int i = 0; i<8; i++){
    position_absolute(i,servo_pos[i].medium);
  }
  delay(3000);
  for(int i = 0; i<8; i++){
    position_absolute(i,servo_pos[i].final);
  }
  delay(3000);
}

// grab one, then two, push them left and right, reset
void seq_1(){
  grabbers_reset(); 
  pushers_reset(); 
  delay(3000); 
  grabbers_first_time(); 
  delay(3000); 
  pushers_lock_first_element(); 
  delay(500); 
  grabbers_reset(); 
  delay(3000); 
  grabbers_second_time(); 
  delay(3000); 
  pushers_push_right(); 
  delay(3000);
  grabbers_before_push_second_element();
  pushers_reset(); 
  delay(3000); 
  grabbers_push_second_element(); 
  delay(2000); 
  pushers_lock_first_element(); 
  delay(500); 
  grabbers_reset(); 
  delay(3000); 
  pushers_push_left(); 
  delay(3000); 
}

// take what is there, lift, wait 2s, drop and reset
void seq_2(){
  closer_reset();
  lifter_reset();
  delay(3000);
  closer_close();
  delay(700);
  lifter_lift();
  delay(1600); // putting the elements
  lifter_drop();
  delay(300);
  closer_reset();
  delay(300);
  lifter_reset();
}

// grabbs two and when docked pushes the two left-right (no pliers action)
void seq_3(){
  delay(50);
  if(elementPresent() && !isFull() ){
    if(isEmpty()){
      grabbers_first_time();
      delay(1000);
      pushers_lock_first_element();
      grabbers_reset();
    } 
    else {
      grabbers_second_time();
      delay(800);
    }
    nb_elements++;
  }

  if (isFull() && isDocked()){
    delay(100);
    pushers_push_right();
    delay(1700);
    grabbers_before_push_second_element();
    pushers_reset();
    delay(1000);
    grabbers_push_second_element();
    delay(800);
    pushers_lock_first_element();
    delay(200);
    //grabbers_reset();
    //delay(100);
    pushers_push_left();
    delay(1700);
    grabbers_reset();
    pushers_reset();
    nb_elements = 0;
    delay(1000);
  }
}

//grabbs two, and when docked creates a 2-element storey below the existing temple
void seq_4(){
  delay(50);
  if(elementPresent() && !isFull() ){
    if(isEmpty()){
      grabbers_first_time();
      delay(1000);
      pushers_lock_first_element();
      grabbers_reset();
    } 
    else {
      grabbers_second_time();
      delay(800);
    }
    nb_elements++;
  }

  if (isFull() && isDocked()){
    delay(100);
    closer_close();
    delay(1000);
    lifter_lift();
    delay(1000);

    pushers_push_right();
    delay(1700);
    grabbers_before_push_second_element();
    pushers_reset();
    delay(1000);
    grabbers_push_second_element();
    delay(800);
    pushers_lock_first_element();
    delay(200);
    //grabbers_reset();
    //delay(100);
    pushers_push_left();
    delay(1700);
    grabbers_reset();
    pushers_reset();

    lifter_drop();
    delay(600);
    closer_reset();
    delay(600);
    lifter_reset();
    nb_elements = 0;
  }
}

//grabbs two, and when docked creates a 2  1-element storey below the existing temple
void seq_5(){
  delay(50);
  if(elementPresent() && !isFull() ){
    if(isEmpty()){
      grabbers_first_time();
      delay(1000);
      pushers_lock_first_element();
      grabbers_reset();
    } 
    else {
      grabbers_second_time();
      delay(800);
    }
    nb_elements++;
  }

  if (isFull() && isDocked()){
    delay(100);
    closer_close();
    delay(1000);
    lifter_lift();
    delay(1000);

    pushers_push_center();
    delay(1700);
    lifter_drop();
    grabbers_before_push_second_element();
    pushers_reset();
    delay(1000);
    closer_reset();
    lifter_reset();
    grabbers_push_second_element();
    delay(800);
    pushers_lock_first_element();
    delay(200);

    closer_close();
    delay(1000);
    lifter_lift();
    delay(1000);
    pushers_push_center();
    delay(1700);
    grabbers_reset();
    pushers_reset();

    lifter_drop();
    delay(600);
    closer_reset();
    delay(600);
    lifter_reset();
    nb_elements = 0;
  }
}

// prints the value of the presence sensor
void seq_6(){
  delay(500);
  PRINTLN(filteredAnalogRead(presenceElement));
}



// prints the value of the presence sensor
void seq_7(){
  delay(100);
  closer_close();
  delay(1000);
  lifter_lift();
  delay(2000);
  lifter_drop();
  delay(1000);
  lifter_reset();
  delay(1000);
  closer_reset();
  delay(2000);
}

int user_input_pin = 1;
int user_input_servo = 5;

// sets a servo to the value given by the input on user_input_pin
void follow_user_input(){
  int position;
  static int nbiter = 0;

  position = (int)((float)filteredAnalogRead(user_input_pin) * 4.88) + 500;
  position_absolute(user_input_servo,position );
  delay(50);
  if (nbiter++ == 20){
    nbiter = 0;
    PRINTLN(position);
  }   
}


// all defined sequences
#define NB_SEQ 8
void (*sequences[NB_SEQ])()  = {
  &seq_0,   // for each servo,  go to each position alternatively
  &seq_1,   // grab one, then two, push them left and right, reset
  &seq_2,   // take what is there, lift, wait 2s, drop and reset
  &seq_3,   // grabbs two and when docked pushes the two left-right (no pliers action)
  &seq_4,   // grabbs two, and when docked creates a 2-element storey below the existing temple
  &seq_5,   // grabbs two, and when docked creates 2 1-element storey below the existing temple
  &seq_6,   // prints the value of the presence sensor
  &seq_7   // lift the column element and puts it back (just to finish the battery)
};




//-------------------------------------------
//            I2C Communication

//-------------------------------------------
void request_message( char adress, char message_id){
  Wire.beginTransmission(adress);  
  Wire.send((byte)message_id);
  Wire.endTransmission();  
  delay(5);
}

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


void send_order(char adress, char order, char val ){
  Wire.beginTransmission(adress);
  Wire.send((byte)order);
  Wire.send((byte)val);
  Wire.endTransmission();
  delay(5);
}

void send_order(char adress, char order, int val ){
  int data[1];
  data[0] = val;
  Wire.beginTransmission(adress);  
  Wire.send((byte)order);
  Wire.send((byte*)data, 1 * sizeof(int));
  Wire.endTransmission();
  delay(5);
}

void send_order(char adress, char order, char val_char, int val_int ){
  int data[1];
  data[0] = val_int;
  Wire.beginTransmission(adress);  
  Wire.send((byte)order);
  Wire.send((byte)val_char);
  Wire.send((byte*)data, 1 * sizeof(int));
  Wire.endTransmission();
  delay(5);
}

//--------------------------------------------------
//         communication DETECTION BOARD
//--------------------------------------------------

// requests 
char get_detection_status(){
  request_message(I2C_DETECTION_BOARD_ADRESS, REQUEST_DETECTION_STATUS);
  Wire.requestFrom(I2C_DETECTION_BOARD_ADRESS, 2* sizeof(char));
  detection_sensors_status = receive_char();
  detection_sensors_recap = receive_char();
  detection_sensors_status = detection_sensors_recap==0 ? STATUS_NO_OBSTACLE : STATUS_WARNING;

  return detection_sensors_status;
}

int get_detection_sensor_value(char sensor){
  send_order(I2C_DETECTION_BOARD_ADRESS, ORDER_SELECT_SENSOR, (char)sensor );
  request_message(I2C_DETECTION_BOARD_ADRESS, REQUEST_DETECTION_VALUE);
  Wire.requestFrom(I2C_DETECTION_BOARD_ADRESS, 1* sizeof(int));
  return receive_int(); // distance seen in mm
}

bool get_key_inter_value(){
  request_message(I2C_DETECTION_BOARD_ADRESS, REQUEST_KEY_INTER_STATE);
  Wire.requestFrom(I2C_DETECTION_BOARD_ADRESS, 1* sizeof(char));
  return receive_char() != 0;
}

// orders

void set_use_beacon(bool use){
  send_order(I2C_DETECTION_BOARD_ADRESS, ORDER_USE_BEACON, (char)use );
}


//--------------------------------------------------
//         communication CONTROL BOARD
//--------------------------------------------------

// requests

float get_theta(){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_THETA);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, 1 * sizeof(int));
  return ((float)receive_int())/100;
}

void get_xy(int* x, int* y){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_XY);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, 2 * sizeof(int));
  *x = receive_int();
  *y = receive_int();  
}

void get_xytheta(int* x, int* y, float* t){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_XYTHETA);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, 3 * sizeof(int));
  *x = receive_int();
  *y = receive_int();
  *t = ((float)receive_int())/100;
}

void print_xytheta(){
  int x,y;
  float t;
  get_xytheta(&x,&y,&t);

  PRINT("Position : x = ");
  PRINT(x);
  PRINT(", y = ");
  PRINT(y);
  PRINT(", theta = ");
  PRINTLN(t);
}

char get_status(){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_STATUS);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, sizeof(char));
  return receive_char();       
}

void get_line_pos(int* x, int* y){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_LINE_POS);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, 2 * sizeof(int));
  *x = receive_int();
  *y = receive_int();
}

char get_pid_usage(){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_PID_USAGE);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, sizeof(char));
  return receive_char();       
}

void get_line_sensors(int* l, int* r, int* c){
  request_message(I2C_MOTOR_BOARD_ADRESS, REQUEST_LINE_SENSORS);
  Wire.requestFrom(I2C_MOTOR_BOARD_ADRESS, 3 * sizeof(int));
  *l = receive_int();
  *r = receive_int();
  *c = receive_int();
}


// orders

void set_mode(char mode){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_SET_MODE, mode );
}

void set_find_line(bool find, bool stop_on_found = false){
  char val = find | (stop_on_found<<1);
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_FIND_LINE, val );
}

// correct the odometry-computed values, when docked for example
// absolute position in mm
void set_position_x(int x){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_SET_X, x);
}
void set_position_y(int y){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_SET_Y, y);
}

// absolute orientation, but that can be used in a color-depending code (that MUST be written as GREEN)
void set_colorwise_orientation(int theta){
  if(match.get_color() == COLOR_GREEN){
    send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_SET_THETA, theta);
  }
  else{
    send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_SET_THETA, -theta);
  }
}
// absolute orientation in *degrees*
void set_orientation(int theta){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_SET_THETA, theta);
}

void set_pid_usage(bool use_dist, bool use_angle){
  char val = use_dist<<1 | use_angle;
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_PID_USAGE, val);
}

void movement_stop(){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_STOP, 0 );
}
// default is to turn according to the color, RED beeing mirrored
void movement_turn(int deg){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_TURN, deg );  
}
// absolute orientation, but that can be used in a color-depending code (that MUST be written as GREEN)
void movement_colorwise_turn(int deg){
  if(match.get_color() == COLOR_GREEN){
    send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_TURN, deg );
  }
  else{
    send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_TURN, -deg );
  }
}

void movement_turn_abs(int deg){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_TURN_ABS, deg );
}
void movement_colorwise_turn_abs(int deg){
  if(match.get_color() == COLOR_GREEN){
    send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_TURN_ABS, deg );
  }
  else{
    send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_TURN_ABS, -deg );
  }

}

void movement_go(int mm){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_GO, mm );
}
void movement_wait(int ms){
  send_order(I2C_MOTOR_BOARD_ADRESS, ORDER_MOVEMENT_CMD, CMD_WAIT, ms );
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

volatile bool recup_active = false;
volatile bool do_recup = false;

void end_match(){
  game_phase = PHASE_THIS_IS_THE_END; // this should never be usefull but just in case...
  PRINTLN("The end !");

  // inhibate all actuators
  stop_all_servos();
  // disable the timer interrupt
  control_stop();  // useless too, but again just in case.


  // stop motors and put motor card in standby
  set_mode(MODE_STANDBY);

  // TODO set the blinkMs to a cool sequence :D

  // and maybe also stay here, don't return to the main program...
  while(true){
    delay(1000);
  }
}

//------------------------------------
// perform a go to position

void movement_go_to(int x, int y) {
  match.set_target_position(x, y);
  int x_cur, y_cur;	// current position
  float angle_cur;
  get_xytheta(&x_cur, &y_cur, &angle_cur);
  float x_diff = x - x_cur;
  float y_diff = y - y_cur;
  float dist = hypot(x_diff, y_diff);

  // compute angle
  float angle = degrees(acos( x_diff / dist ));
  if (-y_diff>0) angle = -angle;

  if (fabs(angle-angle_cur)-0.5>0) movement_turn_abs(angle);
  movement_wait(300);
  movement_go(dist);
}

void movement_colorwise_go_to(int x, int y) {
  int y_target = match.get_color() == COLOR_GREEN ? y-22 : 3000-y-55;
  movement_go_to(x, y_target); //TODO A revoir Napalm
}

void movement_go_to_elem(int i) {
  movement_go_to(
  match.get_elem_x(i),
  match.get_elem_y(i));
}


//------------------------------------
// obstacle avoidance

bool obstacle_avoidance_enabled = false;
bool avoidance_in_progress = false;

bool enemy_is_before(){
  int sharp = false;//detection_sensors_recap & ( SENSOR_MASK(sensorFrontLeft) | SENSOR_MASK(sensorFrontRight)| SENSOR_MASK(sensorFront) ) ; TODO
  int cont = contactorIsPressed(contactorFrontLeft) && contactorIsPressed(contactorFrontRight);
  return sharp || cont;
}
bool enemy_is_behind(){
  return false;
  // TODO return detection_sensors_recap & ( SENSOR_MASK(sensorBackLeft) | SENSOR_MASK(sensorBackRight) );
}
bool enemy_is_left(){
  int sharp = false;//detection_sensors_recap & ( SENSOR_MASK(sensorBackLeft) | SENSOR_MASK(sensorFrontLeft) );
  int cont = contactorIsPressed(contactorFrontLeft);
  return sharp || cont;
}
bool enemy_is_right(){
  int sharp = false;//detection_sensors_recap & ( SENSOR_MASK(sensorFrontRight) | SENSOR_MASK(sensorBackRight) );
  int cont = contactorIsPressed(contactorFrontRight);
  return sharp || cont;
}

void obstacle_avoidance_process() {
  if(false/*obstacle_avoidance_enabled*/){  // TODO shutdown avoidance
    // obstacle avoidance monitoring
    if(detection_sensors_status == STATUS_WARNING){
      if(!avoidance_in_progress){
        //if it's new, stop (and clear the trajectory planned), and react
        avoidance_in_progress = true;
        PRINTLN("Eh gros ya un ghey devant");
        //set_controller(avoidance_controller);
        // TODO react, etc, according to the relative position of the enemy

        // TODO TEMP
        // just go in the other direction a little and rotate 45 deg in the opposite direction
        movement_stop();
        movement_wait(500);
        if(enemy_is_before()  ){
          //movement_go(-150); TODO remettre cette valeur
          //movement_go(-50);
          if(enemy_is_left()){
            //sei(); PRINTLN("avant gauche"); cli();
            movement_turn(45);
          } 
          else {
            //sei(); PRINTLN("avant droit"); cli();
            movement_turn(-45);
          }
        } 
        else if (enemy_is_behind()) {

          //movement_go(150);	TODO remettre cette valeur
          //movement_go(50);
          if(enemy_is_left()){
            //sei(); PRINTLN("arriere gauche"); cli();
            movement_turn(45);
          } 
          else {
            //sei(); PRINTLN("arriere droit"); cli();
            movement_turn(-45);
          }
        }
        delay(7000);
        //sei(); PRINTLN("plop stand bye"); cli();
        set_mode(MODE_STANDBY);		  
        obstacle_avoidance_enabled = false;
      } 
      else {
        // TODO
        // maybe update the avoidance movement if it is no longer a good one (
      }
    } 
    else {
      if(avoidance_in_progress){
        // no more facing an obstacle. when the current trajectory is finished, resume normal operation
        if( get_status() != STATUS_IDLE ){
          avoidance_in_progress = false;
        }
      }
    }
  }
}

//Timer2 overflow interrupt vector handler
ISR(TIMER2_OVF_vect) {
  TCNT2 = control_timer_begining; // reload timer
  sei();

  if(game_phase != PHASE_IN_GAME){
    set_color(get_color_switch());
  }

  ///**/digitalWrite(ledGreen,HIGH);
  if(obstacle_avoidance_enabled){
    get_detection_status(); // updates detection status and the sensor recap
  }
  ///**/digitalWrite(ledGreen,LOW);

  switch(game_phase){
  case PHASE_TEST:
    if(startingCordIsIn()){
      game_phase = PHASE_WAITING_TO_START;
      PRINTLN("Starting cord inserted!");
    }

    obstacle_avoidance_process();
    break;
  case PHASE_WAITING_TO_START:
    {
    }
    break;
  case PHASE_IN_GAME:
    // end of match
    if (millis() > time_end_match){
      end_match();  
    }

    obstacle_avoidance_process();
    break;
  case PHASE_THIS_IS_THE_END:
    // wtf ? we should never get here !
    break;
  default:
    break;
  }

  // test battery voltage
  static char batteryCnt = 0;
  static char batWarnEnable = 0;
  if (--batteryCnt<0) {
    batteryCnt = 100;
    char t = phase_verif_voltage_battery(0);
    if (!batWarnEnable && t) {  // low bat
      setBeepSequence(3);
      batWarnEnable = 1;
    } 
    else if (batWarnEnable && !t) {
      setBeepSequence(0);
      batWarnEnable = 0;
    }
  }


}

//----------------------------------------
//              Match setup
//----------------------------------------

#define INIT_MATCH_DELTA  150 // the distance of the robot center to the axis, when docked
#define ANGLE_POSITIONNING_ERROR ()

void init_match(){
  set_mode(MODE_CONSIGN);

  set_pid_usage(true,false);
  movement_go(-150);
  while(!isDocked())delay(50);
  movement_stop();
  movement_go(-5);
  while(!isDocked())delay(50);
  movement_stop();
  delay(100);
  set_position_x(INIT_MATCH_DELTA);
  set_corrected_colorwise_orientation(0);
  print_xytheta();

  set_pid_usage(true,true);
  movement_go(150);
  movement_wait(200);
  movement_colorwise_turn(90);
  while(get_status() != STATUS_IDLE)delay(100);
  delay(100);

  movement_go(-150);
  while(!contactorIsPressed(contactorBackLeft) && !contactorIsPressed(contactorBackRight));
  set_pid_usage(true,false);
  while(!isDocked());
  movement_stop();
  delay(100);
  movement_go(-5);
  while(!isDocked())delay(50);
  movement_stop();
  delay(100);

  PRINTLN("Sets");
  set_pid_usage(true,true);
  //set_position_y(INIT_MATCH_DELTA);
  set_position_y(match.get_color() == COLOR_GREEN ? INIT_MATCH_DELTA : 3000-INIT_MATCH_DELTA);
  set_corrected_colorwise_orientation(90); 

  movement_go(150);
  movement_wait(200);
  movement_colorwise_turn(-45);
  while(get_status() != STATUS_IDLE)delay(100);
  print_xytheta();
}


// return true if an element was grab
bool recup_element(){
  bool res = false;
  if(!isFull() ){
    long now = millis();
    if(isEmpty()){
      grabbers_first_time();
      while( ! elementPresent() && millis() < now + 1000 ){
        delay(10);
      }
      if(elementPresent()){
        res = true;
        while( millis() < now + 1000 ){delay(10);}
        pushers_lock_first_element();
        nb_elements++;
      }
    } else {
      grabbers_second_time();
      while( ! elementPresent() && millis() < now + 800 ){
        delay(10);
      }
      if(elementPresent()){
        res = true;
        nb_elements++;
      }
      while( millis() < now + 800 ){
        delay(10);
      }
    }
    grabbers_reset();
    
  }
  return res;
}

bool has_tower = false;

void make_tower(){
  lifter_drop();
  delay(600);
  pushers_push_center();
  delay(1700);
  grabbers_before_push_second_element();
  pushers_reset();
  delay(1000);
  closer_reset();
  lifter_reset();
  grabbers_push_second_element();
  delay(800);
  pushers_lock_first_element();
  delay(200);

  closer_close();
  delay(1000);
  lifter_lift();
  delay(1000);
  pushers_push_center();
  delay(1700);
  grabbers_reset();
  pushers_reset();

  lifter_drop();
  delay(600);
  closer_reset();
  delay(600);
  lifter_reset();
  delay(500);
  closer_close();
  delay(1000);
  lifter_drop();
  delay(600);

  has_tower = true;
  nb_elements = 0;
}


#define DOCKING_ANGLE_DELTA  ( 23.0 )

void set_corrected_colorwise_orientation(int deg){
  int delta = 0;
  set_colorwise_orientation(deg); 
  if(match.get_color() == COLOR_GREEN){
    delta = DOCKING_ANGLE_DELTA;
  } 
  else {
    delta = -DOCKING_ANGLE_DELTA;
  }
  set_orientation(get_theta()+delta/10);
}

// drops the construction we have in the pliers
void drop_tower(){
  lifter_reset();
  delay(300);
  closer_reset();
  delay(600);
}

void build_tower(){
  lifter_lift();
  delay(1000);
  pushers_push_center();
  delay(1700);

  if(has_tower){
    lifter_drop();
  }

  grabbers_before_push_second_element();
  pushers_reset();
  delay(1000);
  closer_reset();
  delay(100);
  lifter_reset();
  grabbers_push_second_element();
  delay(800);
  pushers_lock_first_element();
  delay(200);

  closer_close();
  delay(1000);
  lifter_lift();
  delay(1000);
  pushers_push_center();
  delay(1700);
  grabbers_reset();
  pushers_reset();

  lifter_drop();
  delay(600);
  closer_reset();
  delay(600);
  lifter_reset();
  nb_elements = 0;
}

// upgrade an existing temple
void upgrade_tower(){

  // controle si ce bloc est necessaire (par variable d'etat)
  if (closer_is_released()) {
    closer_close();
    delay(800);
  }
  lifter_lift();
  delay(1000);

  pushers_push_center();
  delay(1500);
  lifter_drop();
  grabbers_before_push_second_element();
  pushers_reset();
  delay(900);
  // modif nounours
    lifter_drop();
    delay(400);
    closer_reset();
    delay(400);
    lifter_reset();

  if (nb_elements >1){
    grabbers_push_second_element();
    delay(600);
    pushers_lock_first_element();
    delay(100);

    closer_close();
    delay(600);
    lifter_lift();
    delay(1000);
    pushers_push_center();
    delay(1500);

    grabbers_reset();
    pushers_reset();

    lifter_drop();
    delay(400);
    closer_reset();
    delay(400);
    lifter_reset();
  } 
  else {
    grabbers_reset();
    pushers_reset();
  }

  nb_elements = 0;
}

// dock, provided that we have found a line and we are aligned
void dock_on_construction_zone(){
  movement_go(-250);
  while(!contactorIsPressed(contactorBackLeft) && !contactorIsPressed(contactorBackRight));
  set_pid_usage(true,false);
  while(!isDocked());
  movement_stop();
  delay(50);
  set_pid_usage(true,true);
  set_corrected_colorwise_orientation(180);
  int x_pos, y_pos;
  get_xy(&x_pos, &y_pos);
  match.set_used_line(y_pos);
  // TODO use these values to come back :s

    set_position_x(2100-150);
  set_position_y(match.get_used_line_y());
  print_xytheta();
}


void test_match(){
  char nb_taken = 2;
  set_mode(MODE_CONSIGN);
  // TODO obstacle_avoidance_enabled = true;
  obstacle_avoidance_enabled = false;

  //movement_colorwise_go_to(575, 600);
  movement_go_to_elem(1);
  while(get_status() != STATUS_IDLE){
    delay(50);
  }
  recup_element();

  //movement_colorwise_go_to(775, 600);
  movement_go_to_elem(2);
  while(get_status() != STATUS_IDLE){
    delay(50);
  }
  recup_element();


  if(isFull()){
    nb_taken += 2;
    //make_tower();  
    make_simple_temple();

    //movement_colorwise_go_to(975, 600);
    movement_go_to_elem(3);
    while(get_status() != STATUS_IDLE){
      delay(50);
    }
    recup_element();
  }

  //movement_colorwise_go_to(1175, 600);
  movement_go_to_elem(4);
  while(get_status() != STATUS_IDLE) {
    delay(50);
  }
  recup_element();

  rapprocherLigne();
  // if i'ts the first time and we only have 2 elements, build the first temple
  if(nb_taken==2){
    make_simple_temple();
  }
  align();
  dock_on_construction_zone();

  if( isFull() ){
    build_tower();
  } 
  else {
    drop_tower();
    upgrade_tower();
  }

  movement_go(250);
  while(get_status() != STATUS_IDLE) delay(50);
  obstacle_avoidance_enabled = true;

  //movement_colorwise_go_to(1175, 850);
  movement_go_to_elem(8);
  while(get_status() != STATUS_IDLE) delay(50);
  recup_element();

  if (nb_elements==1 && nb_taken==4) {
    //movement_colorwise_go_to(575, 850);
    movement_go_to_elem(5);
    while(get_status() != STATUS_IDLE) delay(50);
    recup_element();
  } 
  else if(nb_elements==1 && nb_taken==2) {
    //movement_colorwise_go_to(975, 850);
    movement_go_to_elem(7);
    while(get_status() != STATUS_IDLE) delay(50);
    recup_element();

    if (!isFull()) {
      //movement_colorwise_go_to(575, 850);
      movement_go_to_elem(5);
      while(get_status() != STATUS_IDLE) delay(50);
      recup_element();
      nb_taken += 5;
    }
  } 
  else {
    //movement_colorwise_go_to(975, 850);
    movement_go_to_elem(7);
    while(get_status() != STATUS_IDLE) delay(50);
    recup_element();

    if (nb_elements==1 && nb_taken==4) {
      //movement_colorwise_go_to(775, 850);
      movement_go_to_elem(6);
      while(get_status() != STATUS_IDLE) delay(50);
      recup_element();
    } 
    else if (nb_elements==1 && nb_taken==2) {
      //movement_colorwise_go_to(775, 850);
      movement_go_to_elem(6);
      while(get_status() != STATUS_IDLE) delay(50);
      recup_element();
      nb_taken = 5;

    } 
    else {
      //movement_colorwise_go_to(575, 850);
      movement_go_to_elem(5);
      while(get_status() != STATUS_IDLE) delay(50);
      //movement_colorwise_go_to(575, 1100);
      movement_go_to_elem(9);
      while(get_status() != STATUS_IDLE) delay(50);
      recup_element();

      if (nb_elements==1 && nb_taken==4) {
        //movement_colorwise_go_to(1175, 1100);
        movement_go_to_elem(12);
        while(get_status() != STATUS_IDLE) delay(50);
        recup_element();
      } 
      else if (nb_elements==1 && nb_taken==2) {
        //movement_colorwise_go_to(775, 1100);
        movement_go_to_elem(10);
        while(get_status() != STATUS_IDLE) delay(50);
        recup_element();

        //movement_colorwise_go_to(775, 850);
        movement_go_to_elem(6);
        while(get_status() != STATUS_IDLE) delay(50);
      } 
      else {
        //movement_colorwise_go_to(975, 1100);
        movement_go_to_elem(10);
        while(get_status() != STATUS_IDLE) delay(50);
        recup_element();
        
        movement_go_to_elem(11);
        while(get_status() != STATUS_IDLE) delay(50);
        recup_element();
      }
    }
  }

  //movement_colorwise_go_to(1780, 950);

  rapprocherLigne();
  align();
  dock_on_construction_zone();

  upgrade_tower();

  movement_go(250);
  while(get_status() != STATUS_IDLE) delay(50);

  if(nb_taken == 5){
    // TODO 4 last cases 
  }

  end_match();
}

void align(){
  movement_turn_abs(180);
  while(get_status() != STATUS_IDLE) delay(50);
}  

void rapprocherLigne()
{
  movement_colorwise_go_to(1800,500);
  delay(500);
  if(!isFull()){
    recup_element();
  }
  while(get_status() != STATUS_IDLE) {
    delay(50);
    int x_pos, y_pos;
    get_xy(&x_pos, &y_pos);
    if(x_pos > 1600){
      obstacle_avoidance_enabled = false;
    }
  }
  movement_colorwise_turn_abs(90);
  while(get_status() != STATUS_IDLE) delay(50);

  // find the first line but don't stop
  set_find_line(true, false);

  movement_go(600);
  static int last_x = 0, last_y = 0;
  int x,y;
  get_line_pos(&x,&y);
  while(y==0) {
    get_line_pos(&x,&y); 
    delay(50);
  }

  // find the second one and stop
  set_find_line(true, true);
  while(get_status() != STATUS_MISALIGNED) delay(50);
}


void lol_match() {	// TODO changer ce putain de nom
  set_mode(MODE_CONSIGN);
  obstacle_avoidance_enabled = false;	// TODO desactiver sharp arriere
  match.init_elem_states();

  char next_elem = 1;
  char step_elem = 1;
  bool fin = false;
  bool firstFull = true;

  while (!fin) {
    PRINT("nxt_elm "); PRINTLN((int)next_elem);
    PRINT("nb elem in "); PRINTLN((int)nb_elements);
    print_elements();
    PRINTLN("--------------");
    if (match.get_elem_state(next_elem)!=ELEM_STATE_ABSENT) {
      // bouge et recup
      mov_to_elem_and_recup(next_elem);

      if (isFull() && firstFull) {
        // make temple
        make_simple_temple();
	firstFull = false;
	if (next_elem>4) {
	  // sequence secours
	  mov_to_drop_and_drop();
	}
      } else if (isFull()) {
        // build tower
        mov_to_drop_and_drop();
      }
    } // else skip element

    if (match.get_elements_discovered()==6) {
      // if all elements found, fix others
      match.fill_unknow_to_absent();
    }
      
    // compute new position and or drop possibility
    if (next_elem==4) {
      next_elem = 8;
      step_elem = -1;
    } else if (next_elem==5) {
      next_elem = 9;
      step_elem = 1;
    } else if (next_elem==12) {
      if (nb_elements>0) {
        // si 1 restant, il n'a pas ete depose
        mov_to_drop_and_drop();
      }
      fin = true;
    } else {
        next_elem += step_elem;
    }
  }

  end_match();

}

void mov_to_elem_and_recup(int n) {
  if (n==9) {  // uniquement pour ce cas !!! WARNING autre technique vont foirer avec ce controle
    // TODO trouver une autre methode de controle anti bourrage palet
    movement_go_to_elem(5);	// tricky pour ne pas bourrer les elements
    while(get_status() != STATUS_IDLE) {
      delay(100);
    }
  }
  if (n==12) { // retricky
    int x, y;
    get_xy(&x, &y);
    if (x<match.get_elem_x(7)) {
      movement_go_to_elem(8);	// tricky pour ne pas bourrer les elements
      while(get_status() != STATUS_IDLE) {
        delay(100);
      }
    }
  }

  movement_go_to_elem(n);
  while(get_status() != STATUS_IDLE) {
    delay(100);
  }
  if (recup_element()) {
    match.set_sym_elem_state(n, ELEM_STATE_PRESENT);
  } else {
    match.set_sym_elem_state(n, ELEM_STATE_ABSENT);
  }
  if (nb_elements==2) grabbers_second_time();
}

void mov_to_drop_and_drop() {
  rapprocherLigne();
  align();
  dock_on_construction_zone();

  upgrade_tower();

  movement_go(250);
  while(get_status() != STATUS_IDLE) delay(50);
}

void test_match_2(){
  set_mode(MODE_CONSIGN);
  closer_reset();
  grabbers_reset();

  movement_go(200);

  while(get_status() != STATUS_IDLE){
    delay(100);
    recup_element();
  }

  movement_go(200);
  while(get_status() != STATUS_IDLE) delay(100);
  recup_element();

  delay(500);

  if(isFull()){
    make_simple_temple();
  }

}

void test_command_wait(){
  set_mode(MODE_CONSIGN);
  movement_go(200);
  movement_wait(2000);
  movement_turn(180);
  movement_wait(1000);
  movement_go(200);
}

void make_simple_temple(){
  long begining = millis();

  grabbers_second_time();	// TODO NouNours
  lifter_clear();
  closer_reset();                        // ouvre pince
  delay(400);  
  pushers_push_center();  // pousse le premier palet
  delay(1500);
  //lifter_reset();
  grabbers_before_push_second_element();  // libere un peu de place pour ramener les pushers
  pushers_reset();                        // laisse passer les palet
  delay(600);

  grabbers_push_second_element();

  lifter_clear();
  movement_turn(50);
  while(get_status() != STATUS_IDLE)delay(10);
  pushers_push_center();
  delay(700);
  grabbers_reset();

  movement_go(-20);
  while(get_status() != STATUS_IDLE)delay(10);

  pushers_reset();
  lifter_reset();
  delay(800);
  closer_close();
  delay(600);
  lifter_drop();
  delay(200);

  movement_go(20);
  while(get_status() != STATUS_IDLE)delay(10);
  movement_turn(-50);
  while(get_status() != STATUS_IDLE)delay(10);

  PRINT("simple temple in ms : ");
  PRINTLN( millis() - begining );

  has_tower = true;
  nb_elements = 0;
}

void test_left_square(){
  set_mode(MODE_CONSIGN);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(90);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(90);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(90);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(90);
  while(get_status() != STATUS_IDLE)delay(100);
}

void test_right_square(){
  set_mode(MODE_CONSIGN);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(-90);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(-90);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(-90);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_go(1000);
  while(get_status() != STATUS_IDLE)delay(100);
  movement_turn(-90);
  while(get_status() != STATUS_IDLE)delay(100);
}




bool last_key_inter_value;
bool key_inter_changed(){
  bool val = get_key_inter_value();
  if(val != last_key_inter_value ){
    last_key_inter_value = val;
    return true;
  }
  return false;
}


//-------------------------------------------
//                 Main
//-------------------------------------------


void setup(){
  init_servo_pos(); // init the servo values

    // talking to Pololu Serial Servo Controller
  pinMode(servoControllerRxPin, INPUT);
  digitalWrite(servoControllerTxPin, LOW); // keeps pololu board from getting spurious signal
  pinMode(servoControllerTxPin, OUTPUT);
  pinMode(servoControllerResetPin,OUTPUT);

  // Contactors
  pinMode(contactorFrontLeft, INPUT);
  digitalWrite(contactorFrontLeft, HIGH); // pullup
  pinMode(contactorFrontRight, INPUT);
  digitalWrite(contactorFrontRight, HIGH); // pullup
  pinMode(contactorBackLeft, INPUT);
  digitalWrite(contactorBackLeft, HIGH); // pullup
  pinMode(contactorBackRight, INPUT);
  digitalWrite(contactorBackRight, HIGH); // pullup


  // set the data rate for the hardware serial port (communication with the PC)
  Serial.begin(38400);
  PRINTLN("start");

  reset_servo_controller(); // reset the servo controller before speaking to it
  ServoSerial.begin(38400);
  delay(20);
  reset_all(); // set initial servo position

  // TODO get values in EEPROM (speed, initial value, and different fixed positions...

  Wire.begin();        // join i2c bus (no address for master)
  delay(10);

  // color selection
  pinMode(colorSwitch, INPUT);
  digitalWrite(colorSwitch, HIGH); // pullup
  pinMode(ledGreen, OUTPUT);
  digitalWrite(ledGreen, LOW);
  pinMode(ledRed, OUTPUT);
  digitalWrite(ledRed, LOW);
  set_color(get_color_switch());

  // starting cord
  pinMode(startingCordPin, INPUT);
  digitalWrite(startingCordPin, HIGH);

  init_control();

  //softResetMotorBoard();
  //softResetDetectionBoard();

  delay(1000);
  control_start(); // for the strating cord, color, 90s stop... every periodic treatment.

  // init sequence buzzer
  setBeepSequence(0);

  // init key inter value (since we spy on its changes)
  last_key_inter_value = get_key_inter_value();
}


// the sequence currently played, if any (for the tests)
void (*sequence)() = NULL;

void loop(){
  delay(10);
  if(key_inter_changed() || isOnBorder()){
    game_phase = PHASE_SETUP;  
  }

  switch(game_phase){
  case PHASE_TEST:
    // just fooling around, dude.
    phase_test();
    break;
  case PHASE_VERIF:
    // verify sensors and actioner
    phase_verif();
    game_phase = PHASE_TEST;
    break;
  case PHASE_SETUP:
    delay(1000);
    PRINTLN("Setup");
    init_match();
    delay(1000);
    game_phase = PHASE_TEST;
    break;
  case PHASE_WAITING_TO_START:
    // Ready? Steady ? ...
    static int starting_cord_count = 0;

    if(startingCordIsIn()){
      starting_cord_count = 0;
    } 
    else {
      starting_cord_count++;
    }

    if(starting_cord_count > 20){
      time_end_match = millis() + 89UL * 1000; // to stop after 89 seconds
      game_phase = PHASE_IN_GAME; // GO !  
      PRINTLN("Here we go!");
    }
    break;
  case PHASE_IN_GAME:
    // let's rock baby!
    PRINTLN("Go");
    //test_match(); TODO NouNours
    lol_match();
    // TODO launch the IA \o/
    delay(1000);
    game_phase = PHASE_THIS_IS_THE_END;
    break;
  case PHASE_THIS_IS_THE_END:
    // end of the match
    // BUT we will NEVER comme here, since when the time is out, we stay in the interrupt handler "forever this time"
    PRINT("END");
    delay(1000);
    break;
  default:
    break;
  }
}


void phase_test(){
  if(SERIAL_AVAILABLE()){
    char cmd = Serial.read();
    delay(10);
    switch(cmd){
    case '0':
      init_match();
      break;
    case '9':
      test_match();
      break;
    case '+':
      test_match_2();
      break;
    case '=':
      test_command_wait();
      break;
    case '8':
      test_left_square();
      break;
    case '_':
      test_right_square();
      break;
    case 's': // set sequence N
      {
        char nb = Serial.read() - '0';
        if(  nb >= 0 && nb < NB_SEQ ){
          sequence = sequences[nb];
          PRINT("Seq : ");
          PRINTLN((int)nb);
        } 
        else {
          PRINTLN("No sequence.");
          sequence = NULL;
        }
      }
      break;

    case '1':
      PRINT("Theta = ");
      PRINTLN(get_theta());
      break;
    case '2':
      {
        int x,y;
        get_xy(&x,&y);

        PRINT("x = ");
        PRINT(x);
        PRINT(", y = ");
        PRINTLN(y);
      }
      break;
    case '3':
      {
        int x,y;
        float t;
        get_xytheta(&x,&y,&t);

        PRINT("Position : x = ");
        PRINT(x);
        PRINT(", y = ");
        PRINT(y);
        PRINT(", theta = ");
        PRINTLN(t);
      }
      break;
    case '4':
      PRINT("Status: ");
      PRINTLN((int)get_status());
      break;
    case '5':
      {
        int x,y;
        get_line_pos(&x,&y);

        PRINT("Line pos : x = ");
        PRINT(x);
        PRINT(", y = ");
        PRINTLN(y);
      }
      break;
    case '6':
      PRINT("Pid usage (bit 0 = angle, bit 1 = dist)");
      PRINTLN((int)get_pid_usage());
      break;
    case '7':
      {
        int l, r, c;
        get_line_sensors(&l,&r,&c);
        PRINT("Line sensors : l = ");
        PRINT(l);
        PRINT(", r = ");
        PRINT(r);
        PRINT(", c = ");
        PRINTLN(c);
      }
      break;
    case 'm':
      {
        char mode = Serial.read();
        switch(mode){
        case 's':
          set_mode(MODE_STANDBY);
          break;
        case 'c':
          set_mode(MODE_CONSIGN);
          break;
        case 'l':
          set_mode(MODE_LINE_FOLLOWING);
          break;
        case 'a':
          set_mode(MODE_ALIGNMENT);
          break;
        default:
          break;
        }
      }
      break;
    case 'C':
      if(get_color_switch() == COLOR_GREEN){
        PRINTLN("Green");
      }
      else{
        PRINTLN("Red");      
      }
      break;
    case 'b':
      phase_verif_voltage_battery(1);
      break;
    case 'K': 
      PRINT("Key inter : ");
      PRINTLN((int)get_key_inter_value());
      break;
    case 't':
      {
        char mode = Serial.read();
        switch(mode){
        case '1':
        case 'a':
          movement_stop();
          break;
        case '2':
          movement_turn(45);
          break;
        case 'z':
          movement_turn(-45);
          break;
        case '3':
          movement_turn_abs(0);
          break;
        case 'e':
          movement_turn_abs(90);
          break;
        case '4':
          movement_go(50);
          break;
        case 'r':
          movement_go(-50);
          break;
        default:
          break;
        }
      }
      break;
    case 'y':
      {
        char mode = Serial.read();
        switch(mode){
        case '1':
          set_position_x(0);
          break;
        case 'a':
          set_position_x(-123);
          break;
        case '2':
          set_position_y(0);
          break;
        case 'z':
          set_position_y(-100);
          break;
        case '3':
          set_orientation(0);
          break;
        case 'e':
          set_orientation(20);
          break;
        case '4':
          set_find_line(true,true);
          break;
        case 'D':
          set_pid_usage(true,false);
          break;
        case 'A':
          set_pid_usage(false,true);
          break;
        case 'R':
          set_pid_usage(true,true);
          break;
        default:
          break;
        }
      }
      break;
    case 'd':
      {
        char mode = Serial.read();
        switch(mode){
        case 'd':
          {
            static bool plop = false;
            plop = !plop;
            set_use_beacon(plop);
            PRINTLN(plop);
          }
          break;
        case 's':
          {
            char nb = Serial.read()-'0';
            PRINT("Detection Sensor "); 
            PRINT((int)nb); 
            PRINT(" = ");
            PRINTLN(get_detection_sensor_value(nb));
          }
          break;
        case 't':
          {
            PRINT("Status : "); 
            PRINTLN((int)get_detection_status());
            PRINT("recap : ");
            PRINTLN((int)detection_sensors_recap);
          }
          break;
        default:
          break; 
        }
      }
      break;
    case 'i': // set user input
      sequence = &follow_user_input;
      PRINT("User input : pin = ");
      PRINT(user_input_pin);
      PRINT(", servo = ");
      PRINTLN(user_input_servo);
      break;
    case 'p': // set user input pin
      {
        char nb = Serial.read() - '0';
        if(  nb >= 0 && nb < 4 ){
          user_input_pin = nb;
          PRINT("Input pin : ");
          PRINTLN(user_input_pin);
        }
      }
      break;
    case 'o':  // set user input_servo
      {
        char nb = Serial.read() - '0';
        if(  nb >= 0 && nb < 8 ){
          user_input_servo = nb;
          PRINT("Input servo : ");
          PRINTLN(user_input_servo);
        }
      }
      break;
    case 'c':
      {
        char nb = Serial.read();
        switch(nb){
        case 'c':
          closer_close();
          break;
        case 'C':
          closer_reset();
          break;
        case 'l':
          lifter_lift();
          break;
        case 'L':
          lifter_reset();
          break;
        case 'd':
          lifter_drop();
          break;
        case '<':
          pushers_push_left();
          break;
        case '>':
          pushers_push_right();
          break;
        case 'v':
          pushers_push_center();
          break;
        case '^':
          pushers_reset();
          break;
        case 't':
          grabbers_reset();
          break;
        case 'g':
          grabbers_first_time();
          break;
        case 'G':
          grabbers_push_second_element();
          break;
        case 'T':
          grabbers_second_time();
          break;
        case 'r':
          reset_all();
          break;
        case 's':
          stop_all_servos();
          break;
        default:
          break;
        }
      }
      break;
    case 'v':
      game_phase = PHASE_VERIF;
      break;
    case 'r':
      test_motor_speed();
      break;
    case 'g':	// like ghey....
      test_alignement
        ();
      break;
    case 'G':
      test_palais();
      break;
    case 'k': // controle avoidance
      test_obstacle_avoidance();
      break;
    case 'h':
      print_status();
      break;
    case 'q':
      test_goto();
      break;
    case 'n':
      test_line_follow();
      break;
    case 'B':
      test_pid();
      break;
    case 'e':
	print_elements();
        //lol_match();
        test_lol_match();
	print_elements();
	break;
    default:
      break;  
    }
  }

  if( sequence != NULL ){
    (*sequence)();
  }

}

//******************************************
// test global des capteurs et actionneurs

char phase_next() {
  char res = 0;
  if (SERIAL_AVAILABLE()) {
    char cmd = Serial.read();
    while(SERIAL_AVAILABLE()) Serial.read();
    delay(10);
    if (cmd=='q') {
      res = -1;
    } 
    else {
      res = 1;
    }
  }
  return res;
}

//-------------- sharp's -------------------
void phase_verif(){
  char skip = 0;

  //setBeepSequence(1/*3*/);
  skip = phase_verif_sharp_sensors();
  if (skip!=-1) skip = phase_verif_line_fallow();
  if (skip!=-1) skip = phase_verif_presence_element();
  if (skip!=-1) skip = phase_verif_bumpers();
  if (skip!=-1) skip = phase_verif_servo();
  if (skip!=-1) skip = phase_verif_movements();
  phase_verif_match_color();
  phase_verif_voltage_battery(1);
  PRINT("End of verify");
  //setBeepSequence(0);
}

char phase_verif_sharp_sensors() {
  char skip = 0;
  while( skip=phase_verif_sharp_sensor(sensorFrontRight, "Avant Droit") );
  while( skip!=-1 && 0==(skip=phase_verif_sharp_sensor(sensorBackRight, "Arriere Droit")  ));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_sensor(sensorFrontLeft, "Avant Gauche")  ));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_sensor(sensorBackLeft, "Arriere Gauche")   ));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_sensor(sensorFront, "Avant")      ));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_side_sensor(sharpElementLeft, "Gauche")   ));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_side_sensor(sharpElementRight, "Droite")   ));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_side_tresh(sharpElementLeft, "Seuil Gauche")));
  while( skip!=-1 && 0==(skip=phase_verif_sharp_side_tresh(sharpElementRight, "Seuil Droit")));
  return skip;
}

char phase_verif_sharp_sensor(int sensor, char* name) {
  int value = get_detection_sensor_value(sensor);
  PRINT("Sensor ");
  PRINT(name);
  PRINT(" pin ");
  PRINT(sensor);
  PRINT(" = ");
  PRINTLN((float)value/10);
  delay(100);
  return phase_next();
}

int phase_verif_sharp_side_sensor(int sensor, char* name) {
  int value = getSharpD12(sensor);
  PRINT("Sensor ");
  PRINT(name);
  PRINT(" pin ");
  PRINT(sensor);
  PRINT(" = ");
  PRINTLN(value);
  delay(100);
  return phase_next();
}

int phase_verif_sharp_side_tresh(int sensor, char* name) {
  int value = getPosSideElement(sensor);
  PRINT("Sensor ");
  PRINT(name);
  PRINT(" pin ");
  PRINT(sensor);
  PRINT(" pos = ");
  if (value==3) {
    PRINTLN("    Rien");
  } 
  else {
    PRINTLN(value);
  }
  delay(100);
  return phase_next();
}

//-------------- line fallow -------------------

char phase_verif_line_fallow() {
  char skip = 0;
  while ((skip=phase_next())==0) {
    int l, r, c;
    get_line_sensors(&l ,&r, &c);
    PRINT("Line fallow\tl: ");
    PRINT(l);
    PRINT("\tr: ");
    PRINT(r);
    PRINT("\tc: ");
    PRINTLN(c);
    delay(100);
  }
  return skip;
}

//-------------- presence element -------------------

char phase_verif_presence_element() {
  char skip = 0;
  while (0==(skip=phase_next())) {
    int status = getPresenceElement();
    PRINT("Element presence (");
    PRINT(filteredAnalogRead(presenceElement));
    PRINT("\t) is : ");
    char* s_status;
    switch (status) {
    case presenceElementNothing:
      s_status = "Rien";
      break;
    case presenceElementGreen:
      s_status = "   Vert";
      break;
    case presenceElementRed:
      s_status = "      Rouge";
      break;
    case presenceElementHere:
      s_status = "         Present";
      break;
    default:
      s_status = "!!!!! Lol !!!!!!";
      break;
    }
    PRINTLN(s_status);
    delay(100);
  }
  return skip;
}

//-------------- bumper -------------------
char phase_verif_bumpers() {
  char skip = 0;
  while (0==(skip=phase_verif_bumper(contactorFrontLeft, "Avant Gauche")));
  while (skip!=-1 && 0==(skip=phase_verif_bumper(contactorFrontRight, "Avant Droit")));
  while (skip!=-1 && 0==(skip=phase_verif_bumper(contactorBackLeft, "Arriere gauche")));
  while (skip!=-1 && 0==(skip=phase_verif_bumper(contactorBackRight, "Arriere Droit")));
  control_stop();
  while (skip!=-1 && 0==(skip=phase_verif_startingcord()));
  control_start();
  return skip;
}

int phase_verif_bumper(int bumper, char* name) {
  PRINT("Contactor ");
  PRINT(name);
  PRINT(" is ");
  char value = digitalRead(bumper);
  char* s_state;
  if (value==0) {
    s_state = "Pressed";
  } 
  else {
    s_state = "       Released";
  }
  PRINTLN(s_state);
  delay(100);
  return phase_next();
}

char phase_verif_startingcord() {
  PRINT("Startingcor is ");
  char value = digitalRead(startingCordPin);
  char* s_state;
  if (value==0) {
    s_state = "       not inserted";
  } 
  else {
    s_state = "inserted";
  }
  PRINTLN(s_state);
  delay(100);
  return phase_next();
}

//-------------- servo verification ----------------
char phase_verif_servo() {
  char skip = 0;
  while (skip!=-1 && 0==(skip=phase_next())) {
    PRINTLN("Sequence actionneur");
    seq_0();
    delay(500);
  }
  reset_all();

  return skip;
}

//-------------- movements verification -------

char phase_verif_movements() {
  char res = 1;
  set_mode(MODE_CONSIGN);
  while (res==1) {
    PRINTLN("Avance de 3cm");
    movement_go(30);
    delay(500);
    PRINTLN("Recule de 3cm");
    movement_go(-30);
    delay(500);
    PRINTLN("Tourne de 90 deg");
    movement_turn(90);
    delay(500);
    PRINTLN("Tourne de -90 deg");
    movement_turn(-90);
    delay(500);

    PRINTLN("(r)ecommencer, (q)uit, ( )continuer test");
    while(!SERIAL_AVAILABLE()) {
      char cmd = Serial.read();
      while(SERIAL_AVAILABLE()) Serial.read();
      delay(10);
      switch(cmd) {
      case 'q':
        res = -1;
        break;
      case 'r':
        res = 1;
        break;
      default:
        res = 0;
        break;
      }
    }
  }
  set_mode(MODE_STANDBY);
  return res;
}

//------------- color match switch ------------
void phase_verif_match_color() {
  PRINT("Couleur du match selectionne : ");
  char* c;
  if (match.get_color()==COLOR_GREEN) {
    c = "Vert";
  } 
  else {
    c = "     Rouge";
  }
  PRINTLN(c);
}

//-------------- voltage battery display
char phase_verif_voltage_battery(char verbose) {
  float value = getBatteryVoltage();
  char res = 0;
  if (verbose) PRINT("Battery voltage : ");
  if (verbose) PRINT(value);
  if (value<=MIN_BATTERY_VOLTAGE) {
    if (verbose) PRINT("   !!! BATTERY CRITICAL LOW !!!");
    res = 1;
  }
  if (verbose) PRINTLN();
  return res;
}

// -------------------- test motor speed --------
void test_motor_speed() {
  set_mode(MODE_CONSIGN);

  // go foward and backward
  /*PRINTLN("Go");
   movement_go(500);
   while(get_status() != STATUS_IDLE)delay(100);
   movement_go(-500);
   while(get_status() != STATUS_IDLE)delay(100);
   */
  // slower
  PRINTLN("Slower");
  setQuadDistFirst(150);

  movement_go(500);
  delay(100);
  while(get_status() != STATUS_IDLE)delay(100);
  delay(500);
  movement_go(-500);
  delay(100);
  while(get_status() != STATUS_IDLE)delay(100);
  delay(500);

  /* // turn right and left
   PRINTLN("Turn");
   movement_turn(180);
   while(get_status() != STATUS_IDLE)delay(100);
   movement_turn(-180);
   while(get_status() != STATUS_IDLE)delay(100);
   */
  // slower
  PRINTLN("slower");
  setQuadAngleFirst(60);
  movement_turn(180);
  while(get_status() != STATUS_IDLE)delay(100);
  delay(500);
  movement_turn(-180);
  while(get_status() != STATUS_IDLE)delay(100);
  delay(500);

  // reaffect default value for motor board
  setQuadDistFirst(DIST_MAX_SPEED);
  setQuadAngleFirst(ANGLE_MAX_SPEED);
  set_mode(MODE_STANDBY);
}

void test_alignement() {
  set_mode(MODE_CONSIGN);

  set_orientation(-90);
  set_find_line(true, true);

  movement_go(400);

  while(get_status() != STATUS_MISALIGNED) {
    delay(100);
  }
  delay(500);

  // TODO controler la bonne ligne

  PRINTLN("Set mode alignement");
  set_mode(MODE_ALIGNMENT);
  while(get_status() != STATUS_ALIGNED) {
    delay(100);
  }

  set_mode(MODE_LINE_FOLLOWING);
  while(!contactorIsPressed(contactorBackLeft) && !contactorIsPressed(contactorBackRight));
  set_pid_usage(true,false);
  while(!isDocked());
  movement_stop();
  delay(500);

  set_pid_usage(true,true);
  set_position_y(2100-150);
  set_orientation(-180);
}

void test_obstacle_avoidance() {
  set_mode(MODE_CONSIGN);
  obstacle_avoidance_enabled = true;
  movement_go(200);
}

void print_status() {
  get_detection_status();
  PRINT("Status : ");
  PRINTLN(detection_sensors_status, DEC);
  PRINT("Recap : ");
  PRINTLN(detection_sensors_recap, BIN);

  PRINT("is before ");
  PRINTLN(enemy_is_before());
  PRINT("is behind ");
  PRINTLN(enemy_is_behind());

  PRINT("enemy is left ");
  PRINTLN(enemy_is_left());
  PRINT("enemy is right ");
  PRINTLN(enemy_is_right());
}

void test_goto() {
  set_mode(MODE_CONSIGN);
  set_position_x(250);
  set_position_y(match.get_color() == COLOR_GREEN ? 250 : 3000-250);
  set_orientation(match.get_color() == COLOR_GREEN ? 45 : -45);
  obstacle_avoidance_enabled = false;

  movement_go_to_elem(1);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(5);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(9);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(10);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(11);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(7);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(6);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(2);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(3);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(4);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(8);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_go_to_elem(12);
  while(get_status() != STATUS_IDLE) delay(100);

  movement_colorwise_go_to(250, 250);
  while(get_status() != STATUS_IDLE) delay(100);
  movement_turn_abs(match.get_color() == COLOR_GREEN ? 45 : -45);
  while(get_status() != STATUS_IDLE) delay(100);
}

void test_line_follow() {
  set_mode(MODE_CONSIGN);
  obstacle_avoidance_enabled = false;

  print_xytheta();
  movement_colorwise_go_to(400, 0);
  while(get_status() != STATUS_IDLE) {
    delay(100);
    print_xytheta();
  }

  movement_colorwise_go_to(400, 400);
  while(get_status() != STATUS_IDLE) delay(100);
  print_xytheta();

  movement_colorwise_go_to(0, 0);
  while(get_status() != STATUS_IDLE) delay(100);
  print_xytheta();

  movement_turn_abs(match.get_color() == COLOR_GREEN ? 45 : -45);
  while(get_status() != STATUS_IDLE) delay(100);
}

void test_pid()
{
  set_mode(MODE_CONSIGN);
  obstacle_avoidance_enabled = false;

  movement_go(-150);
  while(!isDocked())delay(50);
  movement_stop();
  movement_go(-5);
  while(!isDocked())delay(50);
  movement_stop();
  delay(5000);

  movement_go(1000);
  while(get_status() != STATUS_IDLE) delay(100);

}

void test_palais()
{
  set_mode(MODE_CONSIGN);
  obstacle_avoidance_enabled = false;

  //movement_colorwise_go_to(575, 600);
  movement_go_to_elem(1);
  while(get_status() != STATUS_IDLE){
    delay(50);
  }
  //recup_element();

  movement_colorwise_go_to(250, 250);
  while(get_status() != STATUS_IDLE){
    delay(50);
  }

  movement_colorwise_go_to(575, 600);
  while(get_status() != STATUS_IDLE){
    delay(50);
  }
  
  movement_go_to_elem(4);
  while(get_status() != STATUS_IDLE){
    delay(50);
  }


}

void print_elements() {
  int x, y;
  PRINTLN("========");
  for (x=0; x<4; x++) {
    PRINT(x+1); Serial.print(" ");
    for (y=0; y<3; y++) {
      switch (match.get_elem_state(x+y*4+1)) {
        case ELEM_STATE_UNKNOWN:
          PRINT("? ");
          break;
        case ELEM_STATE_PRESENT:
          PRINT("O ");
          break;
        case ELEM_STATE_ABSENT:
          PRINT("  ");
          break;
        default:
          break;
      }
    }
    PRINTLN("");
  }
  PRINTLN("========");
}

void test_lol_match() {
  char c = 0;
  PRINT("nb element in ");
  PRINTLN((int)nb_elements);
  while (c!=-1 && nb_elements<2) {
    while (0==(c=phase_next())) delay(100);
    recup_element();
    PRINT("nb element in ");
    PRINTLN((int)nb_elements);
  }
  //set_mode(MODE_CONSIGN);
  make_simple_temple();
}
