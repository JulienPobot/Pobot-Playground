#ifndef POSITION_SYSTEM_H
#define POSITION_SYSTEM_H

#include "PositionSystem_config.h"
#include "WProgram.h"

class Coder{
public:
	Coder(char c_A, char c_B );
	
	inline void inc(){count++;}
	inline void dec(){count--;}
	
	inline char get_count();
	inline void reset();
	
private:
	volatile char count;	
};



class CoderSystem {
public:
	// sets up the coders (pin contiguration + systematic values calculation)
	CoderSystem(char c1_A, char c1_B, char c2_A, char c2_B);

	void compute_encoder_delta();
	
	// resets the coders tick counts
	void reset_coders();

	inline long get_delta_angle(){return delta_angle;}
	inline long get_delta_dist(){return delta_dist;}
	inline float get_dAngle(){return dAngle;} 
	inline float get_dDist(){return dDist;} 

	void set_encoder_ratios(float ratio1, float ratio2);
	
public:
	// coders contain the difference of ticks since last compute_encoder_delta()
    Coder C1;
	Coder C2;
	
private:
	
	// Ratio for each encoder (to correct differences in the number of tick per meter caused by the difference of the wheels diameter)
	// The smaller wheel gets TOPS_PER_TICKS as ratio (because she will tick more often than the other), the other one has a slightly smaller ratio
	// The goal is to have count * ratio equal for both coders when the robot is folloxing perfectly straight line trajectory.
	float C1_EncoderRatio, C2_EncoderRatio;

	
    // delta values for angle and distance
    float dAngle; // difference of angle, in tops (for motion control)
    float dDist;  // difference of distance, in tops (for motion control)
    
    long delta_angle; // raw 
    long delta_dist;
};





//--------------------------------------------------------
//                       Odometry
//--------------------------------------------------------


class PositionSystem{
public:
	PositionSystem(CoderSystem* cs);
	
	void set_theta_rad(float theta); // set orientation in radian
	void set_x_top(long x); // set x in top
	void set_y_top(long y); // set y in top
	void set_position(float x, float y, float theta);// set position in meters and radian
	
	
	long get_x_top(){return x_top;}
	long get_y_top(){return y_top;}
	int get_x_mm(){return (int)top_to_mm(x_top);}
	int get_y_mm(){return (int)top_to_mm(y_top);}
	float get_theta_rad(){return Theta;}
	float get_theta_deg(){return degrees(Theta);}
	
	
	
	float top_to_mm(long t){return meter_per_top * t * 1000;}
	float top_to_cm(long t){return meter_per_top * t * 100;}
	float top_to_m(long t){return meter_per_top * t;}
	float top_to_rad(long t){return dAngle_ratio_to_rad * t * 2;}
	float top_to_deg(long t){return degrees(dAngle_ratio_to_rad * t * 2);}
	
	long mm_to_top(float val){return (long)(val * 0.001 * tops_per_meter);}
	long cm_to_top(float val){return (long)(val * 0.01 * tops_per_meter);}
	long m_to_top(float val){return (long)(val * tops_per_meter);}
	long rad_to_top(float val){return val/(2*dAngle_ratio_to_rad);}
	long deg_to_top(float val){return radians(val)/(2*dAngle_ratio_to_rad);}
	
	void integration(); // computes new x, y and theta from coder differences

private:
	CoderSystem* coders;
	
	// ===========  systematic fixed calculated values  ================
	// (calculated from the systematic constants)

	float dAngle_ratio_to_rad; // coeff to pass from the difference of coders counts (balanced by their ratios), in tops, to radians
	//value of a top in meter (ratio for 'major wheel')
	float meter_per_top;     // [meters/top] length of the top unit in meter, to get back readable values from the odometry
	unsigned long ticks_per_meter; // [ticks/meter] number of ticks in one meter, to convert the commands to value usable in the control system
	unsigned long tops_per_meter; // [tops/meter] number of tops in one meter, to convert the odometry values to something usable


	
	// main values of odometry : absolute position and orientaion of the robot
	float Theta;          // orientation of the robot [rad]
	long x_top, y_top;  // position [tops] 

	// temp values
	float cosTheta, sinTheta;
};



#endif
