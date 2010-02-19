#include "ControlManager.h"
#include "params.h"
#include "utils.h"
#include "Sensors.h"
#include "common.h"
#include "StatusList.h"


  Pid::Pid() : Kp(1), Ki(0), Kd(0), out_ratio(0.0){
    memset(prev_samples, 0, sizeof(prev_samples));
  }
  
  void Pid::set_coeffs(float kp, float ki, float kd){
  Kp = kp;
  Ki = ki;
  Kd = kd;
  }
  
  void Pid::set_max_input(long max_in){max_input = max_in;}
  void Pid::set_max_output(long max_out){max_output = max_out;}
  void Pid::set_max_integral(long max_i){max_I = max_i;}
  void Pid::set_output_ratio(float r){out_ratio = r;}
  
  long Pid::filter(long error){
    long derivate;
    long output;
    unsigned char prev_index;
   
  /* derivate value                                             
  *             f(t+h) - f(t)        with f(t+h) = current value
  *  derivate = -------------             f(t)   = previous value
  *                    h
  * so derivate = current error - previous error
  *
  * We can apply a filter to reduce noise on the derivate term,
  * by using a bigger period.
  */
  prev_index = ( index + 1 ) % PID_DERIVATE_FILTER_SIZE;
  derivate = error - prev_samples[prev_index];

  if (max_input)
    S_MAX(error, max_input) ; //saturate input before calculating integral, as it influences it
  
  /* 
   * Integral value : the integral become bigger with time .. (think
   * to area of graph, we add one area to the previous) so, 
   * integral = previous integral + current value
   */
  integral += error ;

  if (max_I)
    S_MAX(integral, max_I); // saturate integrale term

  /* output = P.coef_P + I.coef_I + D.coef_D */
  //output = (long) (Kp * error + Ki * integral + (Kd * derivate) / PID_DERIVATE_FILTER_SIZE );
  
  // coef_P * (P + I.coef_I + D.coef_D)
  output =  (long)(Kp * ( error + Ki * integral + (Kd * derivate) / PID_DERIVATE_FILTER_SIZE ));

    output = out_ratio * output;
  
  if(max_output)
    S_MAX(output,max_output);  // saturate output
    
  // backup values for the next calcul of derivate */
  prev_samples[index] = error;
  index = (++index) % PID_DERIVATE_FILTER_SIZE;
  prev_D = derivate ;
  
  return output;
  }
  
  // reset all integral and derivative data
  void Pid::reset(){
    integral = 0;
    prev_D = 0;
    memset(prev_samples, 0, sizeof(prev_samples)); // reset all errors
  }




// QUADRAMP



  QuadRamp::QuadRamp(){
    var_1st_ord_acc = 0;
    var_1st_ord_dec = 0;
    var_2nd_ord_acc = 0;
    var_2nd_ord_dec = 0;
    reset();
  }
  
  void QuadRamp::set_first_order_derivative(unsigned long positive, unsigned long negative){
    var_1st_ord_acc = positive;
    var_1st_ord_dec = -negative;
    compute_deceleration_thresholds();
  }
  
  void QuadRamp::set_second_order_derivative(unsigned long positive, unsigned long negative){
    var_2nd_ord_acc = positive;
    var_2nd_ord_dec = -negative;
    compute_deceleration_thresholds();
  }
  
  void QuadRamp::compute_deceleration_thresholds(){
    // for d>0
    if(var_2nd_ord_dec)
      threshold_pos = var_1st_ord_acc * ( (float)(var_2nd_ord_dec - var_1st_ord_acc) / ((float) (var_2nd_ord_dec * 2 ) ));
    // for d<0
    if(var_2nd_ord_acc)
      threshold_neg = var_1st_ord_dec * ( (float)(var_2nd_ord_acc - var_1st_ord_dec) / ((float) (var_2nd_ord_acc * 2 ) ));
  }
  
  long QuadRamp::filter(long input){
    long pos_target;
    long var_1st_ord_pos = var_1st_ord_acc;
    long var_1st_ord_neg = var_1st_ord_dec;
    
    long d = input - previous_out ;
		
	
    /* Deceleration ramp */
    if ( d > 0 && var_2nd_ord_dec && d < threshold_pos) {
      long ramp_pos;
      /* var_2nd_ord_dec < 0 */
      /* real EQ : sqrt( var_2nd_ord_dec^2/4 - 2.d.var_2nd_ord_dec ) + var_2nd_ord_dec/2 */
      ramp_pos = sqrt( (var_2nd_ord_dec*var_2nd_ord_dec)/4 - d*var_2nd_ord_dec*2 ) + var_2nd_ord_dec/2;
      var_1st_ord_pos = ramp_pos ;
    } else if (d < 0 && var_2nd_ord_acc && d > threshold_neg) {
      long ramp_neg;
      /* var_2nd_ord_acc > 0 */
      /* real EQ : sqrt( var_2nd_ord_acc^2/4 - 2.d.var_2nd_ord_acc ) - var_2nd_ord_acc/2 */
      ramp_neg = -sqrt( (var_2nd_ord_acc*var_2nd_ord_acc)/4 - d*var_2nd_ord_acc*2 ) - var_2nd_ord_acc/2;
      var_1st_ord_neg = ramp_neg ;
    }
    
    /* try to set the speed : can we reach the speed with our acceleration ? */
    /* si on va moins vite que la Vmax */
    if ( previous_var < var_1st_ord_pos )  {
      /* acceleration would be too high, we reduce the speed */
      /* si rampe acceleration active ET qu'on ne peut pas atteindre Vmax,
       * on sature Vmax a Vcourante + acceleration */
      if (var_2nd_ord_acc && ( var_1st_ord_pos - previous_var > var_2nd_ord_acc) )
        var_1st_ord_pos = previous_var + var_2nd_ord_acc ;
    }
    /* si on va plus vite que Vmax */
    else if ( previous_var > var_1st_ord_pos )  { 
      /* deceleration would be too high, we increase the speed */
      /* si rampe deceleration active ET qu'on ne peut pas atteindre Vmax,
       * on sature Vmax a Vcourante + deceleration */
      if (var_2nd_ord_dec && ( var_1st_ord_pos - previous_var < var_2nd_ord_dec) )
        var_1st_ord_pos = previous_var + var_2nd_ord_dec;
    }
    
    /* same for the neg */
    /* si on va plus vite que la Vmin (en negatif : en vrai la vitesse absolue est inferieure) */
    if ( previous_var > var_1st_ord_neg )  {
      /* acceleration would be to high, we reduce the speed */
      /* si rampe deceleration active ET qu'on ne peut pas atteindre Vmin,
       * on sature Vmax a Vcourante + deceleration */
      if (var_2nd_ord_dec && ( var_1st_ord_neg - previous_var < var_2nd_ord_dec) )
        var_1st_ord_neg = previous_var + var_2nd_ord_dec ;
    }
    /* si on va moins vite que Vmin (mais vitesse absolue superieure) */
    else if ( previous_var < var_1st_ord_neg )  {
      /* deceleration would be to high, we increase the speed */
      /* si rampe acceleration active ET qu'on ne peut pas atteindre Vmin,
       * on sature Vmax a Vcourante + deceleration */
      if (var_2nd_ord_acc && (var_1st_ord_neg - previous_var > var_2nd_ord_acc) )
        var_1st_ord_neg = previous_var + var_2nd_ord_acc;
    }

    /*
     * Position consign : can we reach the position with our speed ?
     */
    if ( /* var_1st_ord_pos &&  */d > var_1st_ord_pos ) {
      pos_target = previous_out + var_1st_ord_pos ;
      previous_var = var_1st_ord_pos ;
    }
    else if ( /* var_1st_ord_neg &&  */d < var_1st_ord_neg ) {
      pos_target = previous_out + var_1st_ord_neg ;
      previous_var = var_1st_ord_neg ;
    }
    else {
      pos_target = previous_out + d ;
      previous_var = d ;
    }

    previous_in = input;
    previous_out = pos_target;
    return pos_target ;
  }
  
  void QuadRamp::reset(){
    previous_var = 0;
    previous_out = 0;
    previous_in = 0;
  }

  // finished when we are at the required point, and we don't change the output anymore
  bool QuadRamp::finished(){
    return previous_in == previous_out && previous_var == 0;
  }

  
//  CONSIGN CONTROLLER


  ConsignController::ConsignController(){
	
	dist_consign = 0;
	angle_consign = 0;

	dist_mgr.init (&dist_quad,  &dist_feedback_acc,  &dist_pid );
    angle_mgr.init(&angle_quad, &angle_feedback_acc, &angle_pid);
   
    use_angle_pid = true;
    use_dist_pid = true;
   
    dist_pid.set_coeffs(DIST_KP, DIST_KI, DIST_KD);
    dist_pid.set_max_input(DIST_MAX_INPUT);
    dist_pid.set_max_output(DIST_MAX_OUTPUT);
    dist_pid.set_max_integral(DIST_MAX_INTEGRAL);
    dist_pid.set_output_ratio(DIST_OUT_RATIO);
  
    angle_pid.set_coeffs(ANGLE_KP, ANGLE_KI, ANGLE_KD);
    angle_pid.set_max_input(ANGLE_MAX_INPUT);
    angle_pid.set_max_output(ANGLE_MAX_OUTPUT);
    angle_pid.set_max_integral(ANGLE_MAX_INTEGRAL);
    angle_pid.set_output_ratio(ANGLE_OUT_RATIO);
    
    dist_quad.set_first_order_derivative(DIST_QUAD_1ST_ACC,DIST_QUAD_1ST_DEC);
    dist_quad.set_second_order_derivative(DIST_QUAD_2ND_ACC,DIST_QUAD_2ND_DEC);
    
    angle_quad.set_first_order_derivative(ANGLE_QUAD_1ST_ACC,ANGLE_QUAD_1ST_DEC);
    angle_quad.set_second_order_derivative(ANGLE_QUAD_2ND_ACC,ANGLE_QUAD_2ND_DEC);
  }
  
  void ConsignController::init(){
	reset();
  }
  
  void ConsignController::add_angle_consign(long delta){
	// TODO check for overflow... or just systematically offset the consign, error, quadramp...
        angle_consign += delta;
  }
  
  void ConsignController::add_dist_consign(long delta){
	// TODO check for overflow... or just systematically offset the consign, error, quadramp...
        dist_consign += delta;
  }
  
  bool ConsignController::angle_quad_finished(){
	return angle_quad.finished();
  }
  bool ConsignController::dist_quad_finished(){
	return dist_quad.finished();
  }
  
  
  void ConsignController::set_search_line(bool search){
    search_line = search;
	if(line_detector){
		line_detector->reset();
	}
  }
  
  
  void ConsignController::perform(){
    long dist_output = 0;
    long angle_output = 0;
    int m1_speed = 0, m2_speed = 0 ;
    
    if(line_detector && search_line){
      if( line_detector->detect() && stop_on_line_found ){
        reset_dist();
        search_line = false;
        stop_on_line_found = false;
	status_manager.set(STATUS_MISALIGNED);
	trajectoryManager->disable();
      }
    }
    
    
    if(use_angle_pid) 
      angle_output = angle_mgr.manage(angle_consign, coder_system.get_dAngle());
    if(use_dist_pid)
      dist_output = dist_mgr.manage(dist_consign, coder_system.get_dDist());
    
    
    m2_speed = (int)(dist_output - angle_output);
    m1_speed = (int)(dist_output + angle_output);
    if(abs(m1_speed) > MAX_MOTOR_SPEED || abs(m2_speed) > MAX_MOTOR_SPEED){
      // TODO scale down both
    }
    
#ifdef REMOVE_SLOW_MOTOR_SPEED
    // speeds too slow don't move the robot and make an unpleasant noise...
    if (abs(m1_speed) < MIN_MOTOR_SPEED){m1_speed = 0;}
    if (abs(m2_speed) < MIN_MOTOR_SPEED){m2_speed = 0;}
#endif
  
    motor1.set_speed(m1_speed);
    motor2.set_speed(m2_speed);
  }
  
  void ConsignController::reset_dist(){
    dist_consign = 0;
    dist_mgr.reset();
  }
  void ConsignController::reset_angle(){
    angle_consign = 0;
    angle_mgr.reset();
  }
  
  void ConsignController::reset(){
    reset_dist();
    reset_angle();
  }
  
  void ConsignController::set_dist_first_order_derivative(unsigned long positive) {
    dist_quad.set_first_order_derivative(positive, positive);
  }
  
  void ConsignController::set_angle_first_order_derivative(unsigned long positive) {
    angle_quad.set_first_order_derivative(positive, positive);
  }
  
  
  void ConsignController::set_trajectory_manager(TrajectoryManager *TrajectoryManager) {
    this->trajectoryManager = trajectoryManager;
  }
  
  





