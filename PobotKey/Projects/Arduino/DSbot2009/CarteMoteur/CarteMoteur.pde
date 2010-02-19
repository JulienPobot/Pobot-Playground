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
// Written by Nicolas Saugnier aka Xevel <nicolas.saugnier@esial.net>
// Based on or heavilly inspired by :
//  - RotaryEncoder by Paul Badger  ( http://www.arduino.cc/playground/Main/RotaryEncoders )
//  - Aversive  by the aversive Team (Zer0, Tof,...)  ( http://wiki.droids-corp.org/mediawiki/index.php/Aversive )
//  - A Tutorial and Elementary Trajectory Model for the Differential Steering System of Robot Wheel Actuators   by G.W. Lucas ( http://rossum.sourceforge.net/papers/DiffSteer/ )
//  - Pololu 3pi line following example PID3piLineFollower ( http://www.pololu.com )
//  - MotionControl by Julien Rouviere <gmail address : julien.rouviere@...>,  ClubElek <http://clubelek.insa-lyon.fr>


/* TODO update
// Ticks per seconds around 10000 should be ok... maybe it can work with more.
// Tweaking :
// 0) verify pins aliases... each coder should have A or B on an interruptable pin (2 or 3) and the other one should in the D port (4 to 7 prefered).
// 1) be sure that encoders are in the right way (forward increments both counts). If not, reverse with C1_REVERSE or C2_REVERSE
// 2) determine, by pysically testing, the values of C1_TicksPerM, C2_TicksPerM and track_in_meter. they might be tweakable later with some standard adjustement procedures...
// 3) verify TOPS_PER_TICKS is adapted to the usage (bigger means less error but quicker overflow I think)
// 4) 
*/


// OK , just FYI : 1 = right, 2 = left... usually :/
// I hope this is consistent along the whole project :/ 

#include <avr/wdt.h>
#include "params.h"   // all defined parameters
#include "utils.h"    // some misc utils
#include "ControlManager.h"
#include "Sensors.h"
#include "PositionSystem.h"
#include "Motor.h"
#include "StatusManager.h"
#include "StatusList.h"
#include "TrajectoryManager.h"
//#include "Tests.h"


#include "Wire.h" // I2C , I is teh slave /o\


StatusManager status_manager;

// aliases for the coders in the coder system
Coder* C1;
Coder* C2;

CoderSystem coder_system(C1_A, C1_B, C2_A, C2_B) ;
PositionSystem position_system(&coder_system);

void init_coders(){
  C1 = &(coder_system.C1);
  C2 = &(coder_system.C2);
  attachInterrupt(C1_INT, do_encoder1, CHANGE);  // encoder pin on interrupt 
  attachInterrupt(C2_INT, do_encoder2, CHANGE);  // encoder pin on interrupt 
}



// coders can be reversed by defining the CX_REVERSE symbols
#ifdef C1_REVERSE
#define C1_CW (C1->inc())
#define C1_CCW (C1->dec())
#else
#define C1_CW (C1->dec())
#define C1_CCW (C1->inc())
#endif

#ifdef C2_REVERSE
#define C2_CW (C2->inc())
#define C2_CCW (C2->dec())
#else
#define C2_CW (C2->dec())
#define C2_CCW (C2->inc())
#endif


void do_encoder1(){
  //digitalWrite(9,LOW);
  if ( (C_VAL(C1_A)==false) ^ (C_VAL(C1_B)==false) ) {
    C1_CCW;
  } else {
    C1_CW;
  }
  //digitalWrite(9,HIGH);
}

void do_encoder2(){
  //digitalWrite(9,LOW);
  if ( (C_VAL(C2_A)==false) ^ (C_VAL(C2_B)==false) ) {
    C2_CCW;
  } else {
    C2_CW;
  }
  //digitalWrite(9,HIGH);
}

#define soft_reset()        \
do                          \
{                           \
    wdt_enable(WDTO_15MS);  \
    for(;;)                 \
    {                       \
    }                       \
} while(0)


//--------------------------------------------------------
//                       Motion
//--------------------------------------------------------

TrajectoryManager trajectory_manager;

//--------------------------------------------------------
//                          Motor
//--------------------------------------------------------

// Our two motors.
Motor motor1; // maybe right motor
Motor motor2; // maybe left  motor

//--------------------------------------------------------
//                   Blocking detection
//--------------------------------------------------------
// TODO 


//--------------------------------------------------------
//                     Sensors
//--------------------------------------------------------

// Reflectance sensors for line following
AnalogSensor rs_left(RS_LEFT);
AnalogSensor rs_right(RS_RIGHT);
AnalogSensor rs_center(RS_CENTER);

//--------------------------------------------------------
//                     Control System
//--------------------------------------------------------
void init_control();
void control_start();
void control_stop();
void init_timer();
void set_next_controller(void* controller); // sets what controller will be used, starting at the next iteration

unsigned char control_timer_begining; // value at which the timer begins in order to achieve the desired frequency with its overflow interruption

// controllers ptrs  needed to manage smoothly the controller change
Controller* current_controller = NULL;
Controller* next_controller = NULL;


void init_control(){
  init_timer(CONTROL_FREQ); //control frequency 
#ifdef OUTPUT_CONTROL_TICK
  pinMode(CONTROL_PIN,OUTPUT);
#endif
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

//==================================

// the current controller will be replaced by the next if is different.
void set_next_controller( void *control ){  // void* to overcome one of the limitations of Arduino IDE : auto prototyping...
  next_controller = (Controller*)control;
}

//Timer2 overflow interrupt vector handler
ISR(TIMER2_OVF_vect) {
  TCNT2 = control_timer_begining; // reload timer
  sei(); // activate interrupts so the coders can tick freely

#ifdef OUTPUT_CONTROL_TICK
  digitalWrite(CONTROL_PIN,LOW); // the CONTROL_PIN is held low for the time it takes to perform all the control and odometry tasks
#endif
  
  // compute differences since last time (dAngle, dTheta, dDist)
  coder_system.compute_encoder_delta();
  
  // compute new position
  position_system.integration();
  
  // update the controller if it's not the same as previously
  if(next_controller != current_controller){
    if(current_controller != NULL)
      current_controller->desinit();
    if(next_controller != NULL)
      next_controller->init();
    current_controller = next_controller;
  }
  
  
  if(current_controller != NULL){
    current_controller->perform();
  }

  trajectory_manager.update();
#ifdef OUTPUT_CONTROL_TICK
  digitalWrite(CONTROL_PIN,HIGH); 
#endif
}

//--------------------------------------------------------
//                  Line Detection
//--------------------------------------------------------
AnalogComparator ac_left(&rs_left, LINE_FINDER_THRESHOLD, LINE_FINDER_NB_TIMES_TO_SEE);
AnalogComparator ac_right(&rs_right, LINE_FINDER_THRESHOLD, LINE_FINDER_NB_TIMES_TO_SEE);
AnalogComparator ac_center(&rs_center, LINE_FINDER_THRESHOLD, LINE_FINDER_NB_TIMES_TO_SEE);

bool on_line(){
  return ac_left.found() || ac_right.found();
}


LineDetector ld_center(&ac_center);


//--------------------------------------------------------
//                      Line Following
//--------------------------------------------------------

class LineFollowingController : public Controller{
public:
  LineFollowingController(){
    reset();
  }

  void init(){
    reset();
    trajectory_manager.reset();
    trajectory_manager.enable();
  }
  void desinit(){}

  void reset(){
    line_last_proportional = 0;
    line_integral = 0;
  }
    
  void perform(){
    // re-activate trajectory manager
    trajectory_manager.enable();
    // get sensor values
    int left = rs_left.get_value();
    int right = rs_right.get_value();
    int center = rs_center.get_value();
  
    int sum = left + right;
    int diff = left - right; 
  
    // determine if we are on a line or not
    if( sum > LF_SUM_THRESHOLD ){ // ok we see the line
      //follow line
      int position = diff;
    
      // The "proportional" term should be 0 when we are on the line.
      int proportional = position;

      // Compute the derivative (change) and integral (sum) of the
      // position.
      int derivative = proportional - line_last_proportional;
      line_integral += proportional;

      // Remember the last position.
      line_last_proportional = proportional;

      // Compute the difference between the two motor power settings,
      // m1 - m2.  If this is a positive number the robot will turn
      // to the right.  If it is a negative number, the robot will
      // turn to the left, and the magnitude of the number determines
      // the sharpness of the turn.  You can adjust the constants by which
      // the proportional, integral, and derivative terms are multiplied to
      // improve performance.
	  // [update] this may or may not be phrased in the right direction : we want to go backwards here...
      int power_difference = proportional * LF_KP + line_integral * LF_KI + derivative * LF_KD;

    
      // Compute the actual motor settings.  We never set either motor
      // to a negative value.
      const int maximum = MAX_LINE_FOLLOWING_SPEED;
      if (power_difference > maximum)
        power_difference = maximum;
      if (power_difference < -maximum)
        power_difference = -maximum;

      // all speeds are negated because we're going forwards
      if (power_difference < 0){
        motor2.set_speed(-(maximum + power_difference));
        motor1.set_speed(-maximum);
      }else{
        motor2.set_speed(-maximum);
        motor1.set_speed(-(maximum - power_difference));
      }
	  status_manager.set(STATUS_LINE_FOLLOWING_RUNNING);
	  
    } else if ( center < LF_CENTER_THRESHOLD ) {
      // we are no more on the line !
      // stop motors
      status_manager.set(STATUS_LINE_LOST);
      motor1.set_speed(0);
      motor2.set_speed(0);
    } else {
      // only the center sensor sees the line. request an alignement
      status_manager.set(STATUS_MISALIGNED);
      motor1.set_speed(0);
      motor2.set_speed(0);
	}
  }

private:
  int line_last_proportional;
  long line_integral;
};


LineFollowingController lfc;
ConsignController cc;

class AlignementController : Controller{
  // align in the correct direction... and avoids destruction of the temple !
  
  void init(){
  }
  void reset(){
  }
  void desinit(){
  }
  
  void perform(){
	// try to align by rotating towards a null angle, and monitor the left and right line sensors to spot the line.
	switch(status_manager.get()){
		case STATUS_MISALIGNED:
			{
				long consign;
				cc.set_angle_first_order_derivative(100);
				if( position_system.get_theta_rad() > 0 ){
					consign = - position_system.deg_to_top(180);
				}else{
					consign = position_system.deg_to_top(180);
				}
				cc.reset();
				cc.add_angle_consign(consign);
				status_manager.set(STATUS_ALIGNEMENT_RUNNING);
			}
			break;
		case STATUS_ALIGNEMENT_RUNNING:
			{
				int left = rs_left.get_value();
				int right = rs_right.get_value();
				
				if(left + right > LF_SUM_THRESHOLD ){
					//set_mode(MODE_LINE_FOLLOWING); // switch directly to line following, no time to lose !
					cc.reset_angle(); // stop here
					status_manager.set(STATUS_ALIGNED);
				}
			}
			break;
		case STATUS_ALIGNED:
		default:
			break;
	}
	
	// use the Consign manager to control the movement.
	cc.perform();
  }
  
};

AlignementController ac;


//--------------------------------------------------------
//                    Communication
//--------------------------------------------------------
void init_serial(); // to speak to the computer
void init_i2c();    // to speak to other cards
void i2c_receive(int nbReceived);
void i2c_request();


#define USE_ANGLE_PID 0x01
#define USE_DIST_PID  0x02

#define SEARCH_LINE   0x01
#define STOP_ON_LINE_FOUND 0x02


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

#define ORDER_SOFT_RESET 8  // perform a soft reset

#define ORDER_QUAD_DIST_FIRST_ORDER 9
#define ORDER_QUAD_ANGLE_FIRST_ORDER 10


volatile char requested_message = REQUEST_NOTHING;


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

unsigned long receive_ulong() {
  while(Wire.available() < 4);
  unsigned long i = Wire.receive();
  unsigned long j = Wire.receive() << 8;
  unsigned long k = Wire.receive() << 16;
  unsigned long l = Wire.receive() << 24;
  return i | j | k | l;
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
      case ORDER_SET_MODE:
        if(nb_received == 2){
          set_mode(receive_char());
        }else{cleanup=true;}
        break;
      case ORDER_FIND_LINE:
        if(nb_received == 2){
          char val = receive_char();
          cc.set_search_line(val & SEARCH_LINE);
          cc.set_stop_on_line_found(val & STOP_ON_LINE_FOUND);
        }else{cleanup=true;}
        break;
      //  void set_theta_rad(float theta); // set orientation in radian
      case ORDER_SET_X:
        if(nb_received == 3){
          int val = receive_int();
          position_system.set_x_top( position_system.mm_to_top(val) );
        }else{cleanup=true;}
        break;
      case ORDER_SET_Y:
        if(nb_received == 3){
          int val = receive_int(); // val arrives in mm
          position_system.set_y_top( position_system.mm_to_top(val) );
        }else{cleanup=true;}
        break;
      case ORDER_SET_THETA:
        if(nb_received == 3){
          int val = receive_int(); // value arrives in degrees
          position_system.set_theta_rad( radians(val) );
        }else{cleanup=true;}
        break;
      case ORDER_PID_USAGE:
        if(nb_received == 2){
          char val = receive_char(); // bit 1 = use_dist, bit 0 = use_angle
          cc.use_dist( val & USE_DIST_PID );
          cc.use_angle( val & USE_ANGLE_PID );
        }else{cleanup=true;}
        break;
      case ORDER_MOVEMENT_CMD:
        if(nb_received == 4){
          int movement = receive_char(); //the type of movement
          int val = receive_int(); // the associated value
          trajectory_manager.add_command(movement, val);
        }else{cleanup=true;}
        break;
      case ORDER_SOFT_RESET:
        soft_reset();
        cleanup=true;
        break;
      case ORDER_QUAD_DIST_FIRST_ORDER:
        if(nb_received == 3){
          int pos_mm_p_s = receive_int();
          Serial.print("Set dist first order : ");
          Serial.print(pos_mm_p_s);
          Serial.print(" mm/s , ");
          Serial.print(position_system.mm_to_top(pos_mm_p_s)/CONTROL_FREQ);
          Serial.println(" top/iter");
          cc.set_dist_first_order_derivative(position_system.mm_to_top(pos_mm_p_s)/CONTROL_FREQ);
        }else{cleanup=true;}
        break;
      case ORDER_QUAD_ANGLE_FIRST_ORDER:
        if(nb_received == 3){
          int pos_mm_p_s = receive_int();
          cc.set_angle_first_order_derivative(position_system.mm_to_top(pos_mm_p_s)/CONTROL_FREQ);
        }else{cleanup=true;}
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
void multisend(int val1, int val2){
  int data[2];
  data[0] = val1;
  data[1] = val2;
  Wire.send((byte*)data,2 * sizeof(int));
}
void multisend(int val1, int val2, int val3){
  int data[3];
  data[0] = val1;
  data[1] = val2;
  data[2] = val3;
  Wire.send((byte*)data, 3 * sizeof(int));
}


void i2c_request(){
  // to know WHAT is requested, look at the requested_message var...
  switch(requested_message){
  case REQUEST_THETA : // angle in 1/100 of degrees, 2 bytes
    multisend((int)(position_system.get_theta_deg()*100));
    break;
  case REQUEST_XY : // x, y in mm, 4 bytes
    multisend(
      (int)position_system.get_x_mm(),
      (int)position_system.get_y_mm()
      );
    break;
  case REQUEST_XYTHETA : // x, y in mm, angle in 1/100 of degrees, 6 bytes
    multisend(
    (int)position_system.get_x_mm(),
    (int)position_system.get_y_mm(),
    (int)(position_system.get_theta_deg()*100)
      );
    break;
  case REQUEST_STATUS : // current status of the controler, 1 byte
    Wire.send(status_manager.get());
    break;
  case REQUEST_LINE_POS : // position of the last seen line, (x,y) in mm, 4 bytes
    multisend( cc.get_line_detector()->get_line_x_mm(), cc.get_line_detector()->get_line_y_mm() );
    break;
  case REQUEST_PID_USAGE : // uses dist (bit 1)? uses angle (bit 0)? , 1 char
    {
      char d = cc.use_dist();
      char a = cc.use_angle();
      Wire.send( (d<<1) | a );
    }
    break;
  case REQUEST_LINE_SENSORS : // values of the line sensors, 3 char
    multisend( rs_left.get_value(), rs_right.get_value(), rs_center.get_value() );
    break;
  case REQUEST_NOTHING :
  default:
    break; 
  }
  requested_message = REQUEST_NOTHING;
}



void init_serial(){
  Serial.begin (SERIAL_BAUDRATE);
  Serial.println("start");
}

void init_i2c(){
  Wire.begin(MY_ADDR); // join the I2C bus as a slave
  // register the handlers
  Wire.onReceive(i2c_receive);
  Wire.onRequest(i2c_request);
}




//--------------------------------------------------------
//          High level Commands (used by the master)
//--------------------------------------------------------

//selects the current mode
void set_mode(char mode){
  switch(mode){
    case MODE_CONSIGN:
      set_next_controller(&cc);
      break;
    case MODE_LINE_FOLLOWING:
      set_next_controller(&lfc);
      break;
    case MODE_ALIGNMENT:
      set_next_controller(&ac);
      break;
    case MODE_STANDBY:
    default:
      current_controller->reset();
      set_next_controller(NULL);
      // force motor stop
      motor1.set_speed(0);
      motor2.set_speed(0);
    break;
  }  
}

long consign_from_meter(float val){
  return position_system.m_to_top(val);
}
long consign_from_centimeter(float val){
  return position_system.cm_to_top(val);
}
long consign_from_milimeter(float val){
  return position_system.mm_to_top(val);
}

long consign_from_degree(float val){
  return position_system.deg_to_top(val);
}
long consign_from_radian(float val){
  return position_system.rad_to_top(val);
}


//--------------------------------------------------------
//                      Main
//--------------------------------------------------------


void setup(){
  motor1.init(M1_PWM, M1_IN1, M1_IN2);
  motor2.init(M2_PWM, M2_IN1, M2_IN2);
  
  init_coders();
  init_serial();
  init_i2c();
  //init_tests();
  init_control();


  trajectory_manager.set_controller(&cc);
  cc.set_line_detector(&ld_center);

  control_start();
  
  pinMode(9,OUTPUT);
}




void loop(){
  // all control code is ran by interrupts
  
  if(Serial.available()){
  char cmd = Serial.read();
  delay(10);
  char param = Serial.read();
  delay(10);
  char dir = Serial.read();
  
    Pid* pid = NULL;
      float* p = NULL;
      char* pid_name = NULL;
      
      switch(cmd){
        case 'l':
          set_mode(MODE_LINE_FOLLOWING);
          break;
        case 'c':
          set_mode(MODE_CONSIGN);
          break;
        case 't':
          test_motor_back_and_forth();
          break;
        case 'd':
          pid = cc.get_dist_pid();
          pid_name = "dist";
          break;
        case 'a':
          pid = cc.get_angle_pid();
          pid_name = "angle";
          break;
        
        case 'F':
          cc.add_dist_consign(consign_from_meter(1));
          break;
        case 'f':
          cc.add_dist_consign(-consign_from_meter(1));
          break;
        case 'G':
          cc.add_dist_consign(consign_from_centimeter(1));
          break;
        case 'g':
          cc.add_dist_consign(-consign_from_centimeter(1));
          break;
        case '1':
          cc.add_dist_consign(consign_from_centimeter(40));
          cc.set_search_line(true);
          cc.set_stop_on_line_found(true);
          delay(1000);
          Serial.print(ld_center.get_line_x_mm());
          Serial.print("\t");
          Serial.println(ld_center.get_line_y_mm());
             
          
          break;
        case 'H':
          cc.add_angle_consign(consign_from_degree(45));
          break;
        case 'h':
          cc.add_angle_consign(-consign_from_degree(45));
          break;
        case 'J':
          cc.add_angle_consign(consign_from_degree(180));
          break;
        case 'j':
          cc.add_angle_consign(-consign_from_degree(180));
          break;
        case 'K':
          cc.add_angle_consign(consign_from_degree(360));
          delay(3000);
          cc.add_angle_consign(consign_from_degree(360));
          delay(3000);
          cc.add_angle_consign(consign_from_degree(360));
          break;
        case 'k':
          cc.add_angle_consign(-consign_from_degree(360));
          delay(3000);
          cc.add_angle_consign(-consign_from_degree(360));
          delay(3000);
          cc.add_angle_consign(-consign_from_degree(360));
          break;
        case 'm':  // test for the difference of diameter of the encoder wheels. Needs only coarse approximation of every other coefficient ^^
          set_mode(MODE_CONSIGN);
          trajectory_manager.add_command(CMD_GO, 1000);
          trajectory_manager.add_command(CMD_TURN, 180);
          trajectory_manager.add_command(CMD_GO, 1000);
          trajectory_manager.add_command(CMD_TURN, -180); 
         
          break;
        case 'D':
          cc.use_dist(!cc.use_dist());
          Serial.print("Use dist : ");
          Serial.println(cc.use_dist());
          break;
        case 'A':
          cc.use_angle(!cc.use_angle());
          Serial.print("Use angle : ");
          Serial.println(cc.use_angle());
          break;
        case 'p':
          print_xyt();
          break;
        case 'P':
          Serial.print("line sensors (right, left, center): ");
          Serial.print(rs_right.get_value());
          Serial.print("\t");
          Serial.print(rs_left.get_value());
          Serial.print("\t");
          Serial.println(rs_center.get_value());
          break;
        case ';':
          Serial.println(position_system.top_to_mm(8330230));
          Serial.println(position_system.top_to_cm(8330230));
          Serial.println(position_system.top_to_m(8330230));
          Serial.println(degrees(position_system.top_to_rad(3245107)));
          Serial.println(position_system.top_to_deg(3245107));
	
	  Serial.println(position_system.mm_to_top(1000.0));
          Serial.println(position_system.cm_to_top(100.0));
          Serial.println(position_system.m_to_top(1.0));
          Serial.println(position_system.rad_to_top(PI));  
          Serial.println(position_system.deg_to_top(180));
	break;
        case '2':
          set_mode(MODE_CONSIGN);
          trajectory_manager.add_command(CMD_GO, 100);
          trajectory_manager.add_command(CMD_GO, -100);
          
          break;

        default:
          break;  
      }
      
      if(pid != NULL){
        switch(param){
          case 'p':
            p = &(pid->Kp) ;
            break;
          case 'i':
            p = &(pid->Ki);
            break;
          case 'd':
            p = &(pid->Kd);
            break;
          default:
            break;  
        }
      }
    
      if (p!= NULL){
        switch(dir){
          case '<':
            *p *= 0.9;
            break;
          case '>':
            *p *= 1.1;
            break;
          case '+':
            *p *= 2;
            break;
          case '-':
            *p *= 0.5;
            break;
          case '*':
            *p *= 10;
            break;
          case '/':
            *p *= 0.1;
            break;
          default:
            break;
        }
      
      Serial.print(pid_name);
      Serial.print(" PID : ");
      Serial.print(pid->Kp );
      Serial.print(", ");
      Serial.print(pid->Ki );
      Serial.print(", ");
      Serial.println(pid->Kd );
      } 
  }
}

void print_xyt(){
    Serial.print("x,y,t (mm et deg) : ");
    Serial.print(position_system.get_x_mm() );
    Serial.print("\t");
    Serial.print(position_system.get_y_mm()); 
    Serial.print("\t");
    Serial.println( position_system.get_theta_deg() );  
}


void test_motor_back_and_forth(){
  int x = 0;
  
  while(x<255){
    x+=5;
    motor1.set_speed(x);
    motor2.set_speed(x);
    delay(50);
  }
  delay(1000);
  while(x>-255){
    x-=5;
    motor1.set_speed(x);
    motor2.set_speed(x);
    delay(50);
  }
  delay(1000);
}

