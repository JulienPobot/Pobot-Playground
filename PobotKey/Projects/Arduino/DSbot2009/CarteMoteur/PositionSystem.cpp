#include "PositionSystem.h"


Coder::Coder(char c_A, char c_B){
	count = 0;
	pinMode(c_A, INPUT); 
	pinMode(c_B, INPUT); 
	digitalWrite(c_A, HIGH);       // turn on pullup resistor
	digitalWrite(c_B, HIGH);       // turn on pullup resistor
}

void Coder::reset(){
	count = 0;
}

char Coder::get_count(){
	return count;
}


//==================================================



CoderSystem::CoderSystem(char c1_A, char c1_B, char c2_A, char c2_B): C1(c1_A, c1_B), C2(c2_A, c2_B){	
	dAngle = 0.0;
    dDist = 0.0;
    delta_angle = 0; 
    delta_dist = 0;
	C1_EncoderRatio = 0;
	C2_EncoderRatio = 0;
}

void CoderSystem::set_encoder_ratios(float ratio1, float ratio2){
	cli();
	C1_EncoderRatio = ratio1;
	C2_EncoderRatio = ratio2;
	sei();
}

void CoderSystem::reset_coders(){
	cli();
	C1.reset();
	C2.reset();
	sei();
}


void CoderSystem::compute_encoder_delta(){
	long dC1, dC2; // encoder count deltas in tops
  
	//compute delta, convert units and save position
	cli();
	char count1 = C1.get_count();
	char count2 = C2.get_count();
	C1.reset();
	C2.reset();
	sei();
  
	// apply ratio to correct coder wheel diameter difference
	dC1 = (long)(C1_EncoderRatio * count1);
	dC2 = (long)(C2_EncoderRatio * count2);
  
	//compute alpha and delta displacement
	delta_angle = dC1 - dC2;
	delta_dist =  dC1 + dC2;
  
	dAngle = ((float)delta_angle) / 2;                  // angle (in tops) 
	dDist  = ((float)delta_dist) / 2;                  // distance (in tops)
}





//--------------------------------------------------------
//                       Odometry
//--------------------------------------------------------


PositionSystem::PositionSystem(CoderSystem* cs) : coders(cs) {
	float C1_EncoderRatio;
	float C2_EncoderRatio;

	// compute systematic variables
    if(C1_TicksPerM > C2_TicksPerM){
      C1_EncoderRatio = TOPS_PER_TICKS;
      C2_EncoderRatio = (float)C1_TicksPerM / (float)C2_TicksPerM * TOPS_PER_TICKS;
      ticks_per_meter = C1_TicksPerM;
      tops_per_meter = C1_TicksPerM * TOPS_PER_TICKS;
    } else {
      C1_EncoderRatio = (float)C2_TicksPerM / (float)C1_TicksPerM * TOPS_PER_TICKS;
      C2_EncoderRatio = TOPS_PER_TICKS;
      ticks_per_meter = C2_TicksPerM;
      tops_per_meter = C2_TicksPerM * TOPS_PER_TICKS;
    }
	
	coders->set_encoder_ratios(C1_EncoderRatio, C2_EncoderRatio);
    
	meter_per_top = 1.0 / (float)tops_per_meter;

    dAngle_ratio_to_rad = 1.0 / (track_in_meter * tops_per_meter);

	Theta = 0.0;          // orientation of the robot [rad]
	x_top = 0;
	y_top = 0;  // position [tops] 

	// temp values
	cosTheta = 1.0;
	sinTheta = 0.0;

}

void PositionSystem::set_position(float x, float y, float theta){
  set_x_top(m_to_top(x));
  set_y_top(m_to_top(y));
  set_theta_rad(theta);
}

void PositionSystem::set_theta_rad(float theta){
  Theta = theta;
  if ( Theta > PI ) {
    Theta -= TWO_PI ;
  } else if ( Theta <= -PI ) {
    Theta += TWO_PI ;
  }
  cosTheta = cos(theta);
  sinTheta = sin(theta);
}

void PositionSystem::set_x_top(long x){
  x_top = x;
}

void PositionSystem::set_y_top(long y){
  y_top = y;
}


void PositionSystem::integration(){
  float delta_angle = coders->get_delta_angle();
  float dTheta =  delta_angle * dAngle_ratio_to_rad; // angle (in rad)
  float dDist = coders->get_dDist();
  
  if (abs(delta_angle) < 2) { 
    // we approximate by a straight line
    x_top += (long)(dDist*cosTheta);
    y_top += (long)(dDist*sinTheta);
  } else {
    // we approximate with a circular trajectory at constant speed. 
    /* r the radius of the circle arc, in tops */
    float r = dDist / dTheta ;
    float newTheta = Theta+dTheta;
    float sinNewTheta = sin(newTheta);
    float cosNewTheta = cos(newTheta);
  
    x_top += r * (-sinTheta + sinNewTheta);
    y_top += r * (cosTheta - cosNewTheta);

    // update orientation
    Theta = newTheta;
    sinTheta = sinNewTheta; 
    cosTheta = cosNewTheta;

    // limit orientation to +/- PI
    if ( Theta > PI ) {
      Theta -= TWO_PI ;
    } else if ( Theta <= -PI ) {
      Theta += TWO_PI ;
    }
  }
}
 


