#ifndef __robot_controller_h__
#define __robot_controller_h__

#define NB_OBSTACLE_MAX  30

typedef struct {
        double speed_left ;
        double speed_right ;
} MOTOR_SPEEDS, *MOTOR_SPEEDS_PTR ;

typedef double SENSOR_VALUE, SENSOR_VALUES_ARRAY[] ;

typedef unsigned int SENSOR_COUNT ;

typedef struct {
        double X ;
        double Y ;
} ROBOT_POSITION, *ROBOT_POSITION_PTR ;

typedef double ROBOT_HEADING ;

typedef int INIT_RETURN_CODE ;

struct position {
	float x;
	float y;
	int ptbf;
	int bkwd;
};

struct sObject {
    	float X;
        float Y;
        float R;
        float W;
	int life;
};

struct sObject aObstacle[NB_OBSTACLE_MAX];

void init_callback(void (*ptrfct)(void));

void init_blocage(void (*ptrfct)(void));

void switch_target(void);

int add_obstacle(float x, float y, float radius, float weight, int life);

void add_target(float,float,int,int);

void immediate_add_target(float,float,int,int);

void  Progress (
	SENSOR_COUNT sensor_count,
                 SENSOR_VALUES_ARRAY sensor_values,
                 ROBOT_POSITION_PTR position,
                 ROBOT_HEADING heading,
                 MOTOR_SPEEDS_PTR motor_speeds
  ) ; 

#endif
