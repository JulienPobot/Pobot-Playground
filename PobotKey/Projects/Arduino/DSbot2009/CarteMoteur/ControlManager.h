#ifndef CONTROLMANGER_H
#define CONTROLMANGER_H
#include "WProgram.h"


// The perform() method of the current_controller is called at each control iteration (if control is running)
// When the controller changes, the new one is initialized, and the other one is finalized.
class Controller {
public:
  virtual void init(){
  }     // first time
  virtual void perform(){
  }  // at each iteration
  virtual void reset(){
  }    // recycle without changing controller
  virtual void desinit(){
  }  // last time before changing controller
};

class ControlManager;

class Filter{
public:
  void set_manager(ControlManager* cm){
    mgr = cm;
  }
  virtual long filter(long value) = 0;
  virtual void reset(){
  }
protected:
  ControlManager* mgr;  // the manager that uses this filter, in case a filter need to lookup information
};


// Accumulator for the feedback, in order to control position instead of speed
class Accumulator : 
public Filter{
public:
  Accumulator():
  total (0){
  }
  long filter(long value){
    total += value;
    return total;
  }
  void reset(){ 
    total = 0;
  }

private:
  long total;
};


#define PID_DERIVATE_FILTER_SIZE 4  // number of last errors we compute the derivative on
class Pid : 
public Filter{
public:
  Pid();

  void set_coeffs(float kp, float ki, float kd);

  void set_max_input(long max_in);
  void set_max_output(long max_out);
  void set_max_integral(long max_i);
  void set_output_ratio(float r);

  long filter(long error);

  // reset all integral and derivative data
  void reset();

public:
  float Kp, Ki, Kd; // PID coeffs
  float out_ratio; // coefficient applied to the output BEFORE saturation

  unsigned char index; // index in the  circular buffer of previous errors
  unsigned long prev_samples[PID_DERIVATE_FILTER_SIZE]; // previous errors, stored in a circular buffer

  long max_input;  // input saturation level
  long max_output; // output saturation level
  long max_I;      // integral saturation level

  long integral; // previous value of the integral
  long prev_D;   // previous value of the derivative
};




#define NEXT(n, i)  (((n) + (i)/(n)) >> 1)

static unsigned long u32_sqrt(unsigned long number) {
  unsigned long n  = 1;
  unsigned long n1 = NEXT(n, number);

  while(abs(n1 - n) > 1) {
    n  = n1;
    n1 = NEXT(n, number);
  }
  while((n1*n1) > number) {
    n1 -= 1;
  }
  return n1;
}


class QuadRamp : 
public Filter{
public:
  QuadRamp();

  void set_first_order_derivative(unsigned long positive, unsigned long negative);

  void set_second_order_derivative(unsigned long positive, unsigned long negative);

  void compute_deceleration_thresholds();

  long filter(long input);

  void reset();

  // finished when we are at the required point, and we don't change the output anymore
  bool finished();

private:
  long var_2nd_ord_acc; //  for acceleration
  long var_2nd_ord_dec; //  for deceleration(is negative)
  long var_1st_ord_acc; // for acceleration
  long var_1st_ord_dec; // for deceleration (is negative)

  long threshold_pos; // threshold for deceleration 
  long threshold_neg; // (idem, negative)


  long previous_var;
  long previous_out;
  long previous_in;
};


// creates a control loop
class ControlManager{
public:
  ControlManager(){
    cf = NULL;
    ff = NULL;
    ef = NULL;
    reset();
  }

  void init(Filter* consign_filter, Filter* feedback_filter, Filter* error_filter){
    set_consign_filter(consign_filter);
    set_feedback_filter(feedback_filter);
    set_error_filter(error_filter);
  }

  void set_consign_filter(Filter* consign_filter){
    cf = consign_filter;
    if(cf) cf->set_manager(this);
  }
  void set_feedback_filter(Filter* feedback_filter){
    ff = feedback_filter;
    if(ff) ff->set_manager(this);
  }
  void set_error_filter(Filter* error_filter){
    ef = error_filter;
    if(ef) ef->set_manager(this);
  }

  long manage(long consign, long feedback){  
    filtered_feedback = ff ? ff->filter(feedback) : feedback;
    filtered_consign = cf ? cf->filter(consign) : consign;
    error = filtered_consign - filtered_feedback;
    output = ef ? ef->filter(error) : error;
    return output;
  }

  void reset(){
    if(cf) cf->reset();
    if(ff) ff->reset();
    if(ef) ef->reset();
    filtered_consign = 0;
    filtered_feedback = 0;
    error = 0;
    output = 0;
  }

public:
  long filtered_consign;
  long filtered_feedback;
  long error;
  long output;
private:
  Filter* cf; // consign filter
  Filter* ff; // feedback filter
  Filter* ef; // error filter
};


//---------------------------------------------------
//            ConsignController
//---------------------------------------------------

class LineDetector;
class TrajectoryManager;

class ConsignController: 
public Controller{
public:
  ConsignController();

  void init();
  LineDetector* get_line_detector(){
    return line_detector;
  }
  Pid* get_dist_pid(){
    return &dist_pid;
  }
  Pid* get_angle_pid(){
    return &angle_pid;
  }

  void add_angle_consign(long delta);
  void add_dist_consign(long delta);

  bool angle_quad_finished();
  bool dist_quad_finished();

  void perform();

  void reset_dist();
  void reset_angle();
  void reset();  

  void desinit(){
    reset();
  }

  void set_line_detector(LineDetector* ld){
    line_detector = ld;
  }
  void set_search_line(bool search);
  void set_stop_on_line_found(bool stop){
    stop_on_line_found = stop;
  }

  bool use_dist(){
    return use_dist_pid;
  }
  bool use_angle(){
    return use_angle_pid;
  }
  bool use_dist(bool uses){
    use_dist_pid = uses; 
    return uses;
  }
  bool use_angle(bool uses){
    use_angle_pid = uses; 
    return uses;
  }

  void set_dist_first_order_derivative(unsigned long positive);
  void set_angle_first_order_derivative(unsigned long positive);

  void set_trajectory_manager(TrajectoryManager *trajectoryManager);

private:
  long dist_consign;
  long angle_consign;

  ControlManager dist_mgr;
  ControlManager angle_mgr;
  Accumulator dist_feedback_acc;
  Accumulator angle_feedback_acc;
  Pid dist_pid;
  Pid angle_pid;
  QuadRamp dist_quad;
  QuadRamp angle_quad;

  LineDetector* line_detector;

  TrajectoryManager* trajectoryManager;

  bool search_line;
  bool stop_on_line_found;
  bool use_angle_pid;
  bool use_dist_pid;
};




#endif

