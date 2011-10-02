#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include "mbrship.h"
#include "fuzzy.h"
#include "controller.h"
#include "main.h"

#define TARGETING 40 // distance to target in 1/25.4 of inch
#define MAX_POS 10

#define TRUE 1
#define FALSE 0

#define OFFSET 0

#define DEAD 0
#define INFINITE -1

#define ROBOT_BDBX 35.0
#define ROBOT_BOX  20.0

#define POINTING_LIMIT  0.05

#define MAX(a,b)	((fabs(a)>fabs(b)) ? a : b)
#define MIN(a,b)	((a<b) ? a : b)

ROBOT_POSITION target;
pthread_mutex_t mtarget = PTHREAD_MUTEX_INITIALIZER;

struct position a_target[MAX_POS];

int robot_blocked = FALSE;
unsigned char pos = 0,next=0;
int pointing = FALSE, reverse=FALSE;
void (*callback)(void);
void (*blocage)(void);

void init_callback(void (*ptrfct)(void)){
	callback=ptrfct;
}

void init_blocage(void (*ptrfct)(void)){
	blocage=ptrfct;
}

float func(float x){
  return ((exp(x+0.3)-exp(0.3))/exp(1.3)) ;
}

float func2(float x){
    if (x < 0) return 1;
    else return (1 - 1/(1 - exp (-0.12*(x-60))));
}

int add_obstacle(float x, float y, float radius, float weight, int life){
  int i;
  int ok = FALSE;

  for (i=0 ; i < NB_OBSTACLE_MAX ; i++){
  	if (aObstacle[i].life == DEAD) {
		ok = TRUE;
		aObstacle[i].X = x;
		aObstacle[i].Y = y;
		aObstacle[i].R = radius;
		aObstacle[i].W = weight;
		aObstacle[i].life = life;
		break;
	}
  }
  if (ok == FALSE) return -1;
  else return 0;
}

void obstacle_life(void){
  int i;
  
  for(i=0 ; i < NB_OBSTACLE_MAX ; i++){
  	if (aObstacle[i].life > 0) aObstacle[i].life--;
  }
}

void switch_target(void) {
  pos++;
  if (pos >= MAX_POS) pos=0;
  if (pos!=next){
  	pthread_mutex_lock(&mtarget);
	target.X = a_target[pos].x ;
	target.Y = a_target[pos].y ;
	pointing = a_target[pos].ptbf;
	reverse  = a_target[pos].bkwd;
	pthread_mutex_unlock(&mtarget);
  } else {
  	callback();
  }

}

void add_target(float x, float y , int point, int rev){

  a_target[next].x=x;
  a_target[next].y=y;
  a_target[next].ptbf=point;
  a_target[next].bkwd=rev;
  if (pos==next){
  	pthread_mutex_lock(&mtarget);
	target.X=x;
	target.Y=y;
	pointing=point;
	reverse=rev;
	pthread_mutex_unlock(&mtarget);
  }
  next++;
  if (next>= MAX_POS) next=0;
  fprintf(stderr,"add point x= %f  y=%f \n",x,y);
}

void immediate_add_target (float x, float y, int point, int rev) {
  pos=0;
  next=0;
  a_target[next].x=x;
  a_target[next].y=y;
  a_target[next].ptbf=point;
  a_target[next].bkwd=rev;
  pthread_mutex_lock(&mtarget);
  target.X=x;
  target.Y=y;
  pointing=point;
  reverse=rev;
  pthread_mutex_unlock(&mtarget);
}

void  Progress (
                SENSOR_COUNT sensor_count,
                SENSOR_VALUES_ARRAY sensor_values,
                ROBOT_POSITION_PTR position,
                ROBOT_HEADING heading,
                MOTOR_SPEEDS_PTR motor_speeds
  ) {
  //static double delta_theta2 ;

  double robot_heading ;
  double heading2target ;
  double delta_theta ;
  double distance2target ;
  static double last_delta_theta;
  static double last_distance2target;
  double speed = 0;
  double speed_coeff = 0;
  double bias = 0;
  double bias_coeff = 0;

  static int perhaps_blocked = 0;

  double l ;
  double r ;

  float distance2object;
  float heading2object;
  float delta_theta_object;
  
  static double k=0;
  double speed_base;
  double fuzzy_bias;


  if (pos==next) return;
  //robot_heading=(float)( heading * M_PI ) / 180 ;
  robot_heading=heading;
  if (reverse == TRUE ){
	robot_heading += M_PI;
	if (robot_heading > M_PI) robot_heading -= 2 * M_PI;
  }
  position->X+=OFFSET*cos(robot_heading);
  position->Y+=OFFSET*sin(robot_heading);
  pthread_mutex_lock(&mtarget);
  distance2target=(float)sqrt( pow( ( position->X - target.X ),2) + pow(( position->Y - target.Y ),2) );
  if (target.X-position->X ==0) {
   if (target.Y-position->Y >0)
    heading2target=M_PI_2;
   else
    heading2target=-M_PI_2;
  }
  else
   heading2target=(float) atan( ( target.Y-position->Y ) / ( target.X-position->X ));
  if ( ( target.X - position->X ) <0 )   heading2target += M_PI ;
  if  ( heading2target < 0 ) heading2target += 2 * M_PI ;
  //speed= (float) ((motor_speeds->speed_left + motor_speeds->speed_right) /2 );
  delta_theta=(double)( heading2target - robot_heading ) ;
  if ( delta_theta > M_PI) delta_theta -= 2 * M_PI;
  pthread_mutex_unlock(&mtarget);
  
  if (distance2target<TARGETING){
     switch_target();
     fprintf (stderr,"\n SWITCH TARGET  x= %f  y=%f\n", position->X, position->Y);
  }
  else {
     // inference rules
      //  speed depending of distance and heading
       // for distance 2 target very close
     speed+=m_speed_very_slow ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_left(delta_theta)) ;

     speed+=m_speed_very_slow ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_left(delta_theta)) ;

     speed+=m_speed_slow ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_left(delta_theta)) ;

     speed+=m_speed_normal ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_center(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_center(delta_theta)) ;

     speed+=m_speed_slow ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_right(delta_theta)) ;

     speed+=m_speed_very_slow ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_right(delta_theta)) ;

     speed+=m_speed_very_slow ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_right(delta_theta)) ;

        // for distance 2 target close

     //speed+=m_speed_very_slow ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     speed+=m_speed_normal ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_left(delta_theta)) ;

     //speed+=m_speed_very_slow ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_left(delta_theta))) ;
     speed+=m_speed_normal ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_left(delta_theta)) ;

     speed+=m_speed_normal ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_left(delta_theta)) ;

     speed+=m_speed_fast ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_center(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_center(delta_theta)) ;

     speed+=m_speed_normal ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_right(delta_theta)) ;

     //speed+=m_speed_very_slow ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_right(delta_theta))) ;
     speed+=m_speed_normal ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_right(delta_theta)) ;

     //speed+=m_speed_very_slow ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     speed+=m_speed_normal( fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_right(delta_theta)) ;


        // for distance 2 target far

     //speed+=m_speed_very_slow ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     speed+=m_speed_normal( fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_left(delta_theta)) ;

    // speed+=m_speed_slow ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_left(delta_theta))) ;
     speed+=m_speed_normal( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_left(delta_theta)) ;

     speed+=m_speed_fast ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_left(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_left(delta_theta)) ;

     speed+=m_speed_fast ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_center(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_center(delta_theta)) ;

     speed+=m_speed_fast ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_right(delta_theta)) ;

     //speed+=m_speed_slow ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_right(delta_theta))) ;
     speed+=m_speed_normal ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_right(delta_theta)) ;

     //speed+=m_speed_very_slow ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     speed+=m_speed_normal ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     speed_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_right(delta_theta)) ;

     //  bias depending of distance and heading
     bias+=m_bias_center ( m_dtheta_center(delta_theta)) ;
     bias_coeff +=  m_dtheta_center(delta_theta) ;

       // for distance 2 target very close
     bias+=m_bias_full_left ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_left(delta_theta)) ;

     bias+=m_bias_full_left ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_left(delta_theta)) ;

     bias+=m_bias_strong_left ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_left(delta_theta)) ;

     bias+=m_bias_strong_right ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_right(delta_theta)) ;

     bias+=m_bias_full_right ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_far_right(delta_theta)) ;

     bias+=m_bias_full_right ( fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_very_near(distance2target),m_dtheta_extreme_right(delta_theta)) ;

        // for distance 2 target close

     bias+=m_bias_full_left ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_left(delta_theta)) ;

     bias+=m_bias_strong_left ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_left(delta_theta)) ;

     bias+=m_bias_left ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_left(delta_theta)) ;

     bias+=m_bias_right ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_right(delta_theta)) ;

     bias+=m_bias_strong_right ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_far_right(delta_theta)) ;

     bias+=m_bias_full_right ( fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_near(distance2target),m_dtheta_extreme_right(delta_theta)) ;


        // for distance 2 target far

     bias+=m_bias_full_left ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_left(delta_theta)) ;

     //bias+=m_bias_strong_left ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_left(delta_theta))) ;
     bias+=m_bias_strong_left ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_left(delta_theta)) ;

     bias+=m_bias_left ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_left(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_left(delta_theta)) ;

     bias+=m_bias_right ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_right(delta_theta)) ;

     //bias+=m_bias_strong_right ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_right(delta_theta))) ;
     bias+=m_bias_strong_right ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_far_right(delta_theta)) ;

     bias+=m_bias_full_right ( fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_right(delta_theta))) ;
     bias_coeff +=  fuzzyAnd (m_dist_far(distance2target),m_dtheta_extreme_right(delta_theta)) ;

     speed=speed/speed_coeff;
     bias=bias/bias_coeff;
     fuzzy_bias=bias;
#define BIAS_ALIGN 0.2
#define BIAS_ROTATING 0.5
#define MIN_SPEED 0.25
#define MIN_STEP 2.0
#define MIN_DELTA2_THETA .01
#define BLOCKED_LEVEL 100
     ////________________________________________________________________________
     if (bias < BIAS_ALIGN && speed > MIN_SPEED) {
	     if (fabs (last_distance2target-distance2target) < MIN_STEP)
		     perhaps_blocked++;
             else
		     perhaps_blocked--;
     }
     
     if (speed < MIN_SPEED && bias > BIAS_ROTATING) {
	     if (fabs (last_delta_theta - delta_theta) < MIN_DELTA2_THETA)
		     perhaps_blocked++;
	     else
		     perhaps_blocked--;
     }
     if (perhaps_blocked < 0) perhaps_blocked = 0;
     if (perhaps_blocked > BLOCKED_LEVEL) {
	     blocage();
	     perhaps_blocked = 0;
     }
     last_delta_theta=delta_theta;
     last_distance2target=distance2target;
     ////_________________________________________________________________________

     //// ________________________________________________________________________
     {
      int i;
       for (i=0;i<NB_OBSTACLE_MAX;i++){
           if (aObstacle[i].life == DEAD) continue;
           distance2object=(float)sqrt( pow( ( position->X - aObstacle[i].X ),2) + pow(( position->Y - aObstacle[i].Y ),2) );
           if (distance2object < (aObstacle[i].R+ROBOT_BDBX)) {
              if (aObstacle[i].X-position->X ==0) {
               if (aObstacle[i].Y-position->Y >0)
                heading2object=M_PI_2;
               else
                heading2object=-M_PI_2;
              }
              else
               heading2object=(float) atan( ( aObstacle[i].Y-position->Y ) / ( aObstacle[i].X-position->X ));

              if ( ( aObstacle[i].X - position->X ) <0 )   heading2object += M_PI ;
              if  ( heading2object < 0 ) heading2object += 2 * M_PI ;

              delta_theta_object=(float)( heading2object - robot_heading ) ;

              if ( delta_theta_object > M_PI) delta_theta_object -= 2 *	M_PI;


                  if (delta_theta_object > -M_PI/6 && delta_theta_object < M_PI/6) {
                          sensor_values[4] += 200*func2(distance2object-20-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
                  if (delta_theta_object > -M_PI/3 && delta_theta_object < 0) {
                          sensor_values[2] += 255*func2(distance2object-8-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
                  if (delta_theta_object > -M_PI/2 && delta_theta_object < -M_PI/6) {
                          sensor_values[3] += 20*func2(distance2object-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
                  if (delta_theta_object > -4*M_PI/6 && delta_theta_object < -M_PI/4) {
                          sensor_values[9] += 160*func2(distance2object+10-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
                  if (delta_theta_object < M_PI/3 && delta_theta_object > 0) {
                          sensor_values[1] += 255*func2(distance2object-8-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
                  if (delta_theta_object < M_PI/2 && delta_theta_object > M_PI/6) {
                          sensor_values[0] += 200*func2(distance2object-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
                 if (delta_theta_object < 4*M_PI/6 && delta_theta_object > M_PI/4) {
                          sensor_values[8] += 160*func2(distance2object+10-aObstacle[i].R-ROBOT_BOX)*aObstacle[i].W;
                  }
              //printf ("theta2object = %f  distance = %f \n",delta_theta_object,distance2object);

           }
        }
     }

     ////_________________________________________________________________________
/*
     #define seuil 80
     #define seuil2 160
    pthread_mutex_lock(&mcapteur);
    speed_base=1;
    l =  sensor_values[0] / 255 + sensor_values[1] / 255 + 2*sensor_values[8] / 255
       - (sensor_values[2] / 255 +sensor_values[3] / 255 + sensor_values[9] / 255) ;
    r =  sensor_values[2] / 255  + sensor_values[3] / 255  +2*sensor_values[9] / 255
         - (sensor_values[0] / 255 + sensor_values[1] / 255 +sensor_values[8] / 255) ;
    //l = l + ( 2*sensor_values[5] / 255  - sensor_values[7] / 255) ;
    //r = r + ( 2*sensor_values[7] / 255  - sensor_values[5] / 255) ;

    //k = func(k+(fabs(r-l))/9);
      k = func(k+(sensor_values[0]+sensor_values[1]+sensor_values[2]
                  +sensor_values[3]+1.5*sensor_values[8]+1.5*sensor_values[9])/800);

    if (k>1) k=1;

    bias=k*(r-l)/2+(1-k)*bias;
    speed=k*((l+r)/2+speed_base)+(1-k)*speed;
    if (sensor_values[4]>120) speed=0;
    if (sensor_values[0]+sensor_values[1]+sensor_values[2]+sensor_values[3]>600) {
       if (robot_blocked == FALSE) {
       		blocage();
		robot_blocked = TRUE;
	}	
       speed=0;
       bias=fuzzy_bias;
    }
    else {
    	robot_blocked = FALSE;
    }
    pthread_mutex_unlock(&mcapteur);
    //printf("%f\n",k);
    //if (k>0) k-=0.01;
    //if (k<0.1) k=0;
*/
     ////_________________________________________________________________________
     if (pointing == TRUE) {
	 if (fabs(bias) > POINTING_LIMIT) speed=0;
	 else {
		 switch_target();
		 fprintf(stderr,"POINTING OK, SWITCH TARGET\n");
	 }
     }
     ////_________________________________________________________________________	
     l=speed-bias;
     r=speed+bias;

     if (reverse == TRUE) {l=-speed-bias;r=bias-speed;}
     if (l<-1) {
       l=-1 ;
     } else if (l>1) {
       l=1;
     }
     if (r<-1) {
       r=-1 ;
     } else if (r>1) {
       r=1;
     }

     motor_speeds->speed_left = l ;
     motor_speeds->speed_right = r ;
  }
}
  
