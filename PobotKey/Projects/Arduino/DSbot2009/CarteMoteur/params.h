#ifndef PARAMS_H
#define PARAMS_H

// parameters

//if defined, outputs a tick on the CONTROL_PIN on each control iteration. 
// The pin is up the whole time the control computation occures
#define OUTPUT_CONTROL_TICK 

// Communication
#define MY_ADDR 3
#define SERIAL_BAUDRATE 38400


#define MODE_STANDBY         's'
#define MODE_CONSIGN         'c'
#define MODE_LINE_FOLLOWING  'l'
#define MODE_ALIGNMENT       'a'


#define CMD_STOP 1
#define CMD_TURN 2
#define CMD_TURN_ABS 3
#define CMD_GO   4
#define CMD_WAIT 5


//--------------------------------------------------------
//                       Pin aliases
//--------------------------------------------------------
// Unused pins : digital 0(RX), 1(TX), 9

// Pins for motor control ( TB6612FNG Dual Motor Driver Carrier )
#define M1_PWM 6
#define M1_IN1 8
#define M1_IN2 7

#define M2_PWM 10
#define M2_IN1 13
#define M2_IN2 12

// pin for optional control timing 
#define CONTROL_PIN  11

// pins for coders
// all coder pins must be in port D (digital pins 0 to 7)
#define C1_A 2
#define C1_B 4
#define C1_INT 0 // interruption 0 is on pin 2

#define C2_A 3
#define C2_B 5
#define C2_INT 1 // interruption 1 is on pin 3


// analog pins for reflectance sensors
#define RS_0 0
#define RS_LEFT 0   // line sensor at the left of the robot's axis, *behind* the drive
#define RS_1 1
#define RS_RIGHT 1  // line sensor at the right of the robot's axis, *behind* the drive
#define RS_2 2
#define RS_CENTER 2 // line sensor between the two wheels
#define RS_3 3

// analog pins 4(SDA) and 5(SCL) are dedicated to I2C


//----------------------------------

// Macro to get the value of the corresponding coder pins.
// This is not portable nor secure but much quicker than digitalRead...
#define C_VAL(x) (PIND & (1 << x))



// ============ pid controlers ======================
#define DIST_KP 12
#define DIST_KI 0.01
#define DIST_KD 1
#define DIST_OUT_RATIO 0.0001
#define DIST_MAX_INPUT 0
#define DIST_MAX_OUTPUT 250
#define DIST_MAX_INTEGRAL 10

#define ANGLE_KP 11.5
#define ANGLE_KI 0.01
#define ANGLE_KD 1
#define ANGLE_OUT_RATIO 0.0001
#define ANGLE_MAX_INPUT  0
#define ANGLE_MAX_OUTPUT 250
#define ANGLE_MAX_INTEGRAL 10

#define DIST_QUAD_1ST_ACC 13000
#define DIST_QUAD_1ST_DEC 13000
#define DIST_QUAD_2ND_ACC 120
#define DIST_QUAD_2ND_DEC 120

#define ANGLE_QUAD_1ST_ACC 10000
#define ANGLE_QUAD_1ST_DEC 10000
#define ANGLE_QUAD_2ND_ACC 100
#define ANGLE_QUAD_2ND_DEC 100

// ==============  Control system  =================================

#define CONTROL_FREQ 200.0     // theoretical frequency of control
//float actual_control_freq;     // the real control frequency (the time has a small resolution, so the real frequency is a little different from the theoretical one)
#define TIMER2_CLOCK_FREQ 15625.0 // 15625Hz for /1024 prescale from 16MHz

//---------------------------------------------------
//            ConsignController
//---------------------------------------------------


#define MAX_MOTOR_SPEED 70     // not really used... yet :/
#define REMOVE_SLOW_MOTOR_SPEED // if defined,  we truncate slow motor speeds.
#define MIN_MOTOR_SPEED 12   // below, we set to zero to avoid annoying noise, and as it does not make the robot move, we don't care. (only when REMOVE_SLOW_MOTOR_SPEED is defined) 


//      Line Follower Controller
#define LF_SUM_THRESHOLD 120      // below this (for the sum of left and right), we are no more on the line...
#define LF_CENTER_THRESHOLD 200  // below this, we are no longer on the line...

#define MAX_LINE_FOLLOWING_SPEED 90
#define LF_KP 0.18f
#define LF_KI 0.0001f
#define LF_KD 1.2f


#define ABSOLUTE_ROTATION 0
#define RELATIVE_ROTATION 1

#endif
