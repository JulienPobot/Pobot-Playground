#ifndef SENSORS_H
#define SENSORS_H



#define LINE_FINDER_THRESHOLD 480
#define LINE_FINDER_NB_TIMES_TO_SEE 3



// Analog sensor with mapping to 0:1023 but no filtering
class AnalogSensor{
public:
  AnalogSensor(int pin);
  /*virtual*/ int get_value();
private:
  int pin_;
//  int min_;
//  int max_;
};


class AnalogComparator{
public:
  AnalogComparator(AnalogSensor* s, int detection_threshold, char nb_times_to_see):_sensor(s){
	_detection_threshold = detection_threshold;
	_nb_times_to_see = nb_times_to_see;
	reset();
  }
  
  bool update(){
    if( _sensor->get_value() > _detection_threshold ){
      if(!_found){
        if( ++_nb_times_seen > _nb_times_to_see  )
          _found = true;
      }
    } else {
      reset();
    }
    return _found;
  }
  
  bool found(){
    return _found;
  }
  
  void reset(){
    _found = false;
    _nb_times_seen = 0;  
  }
  
private:
  AnalogSensor* _sensor;
  int _detection_threshold;
  char _nb_times_seen;
  char _nb_times_to_see;
  bool _found;
};



class LineDetector {
public:
	LineDetector(AnalogComparator* comp);
	
	bool detect();
	void reset();
    int get_line_x_mm(){return line_pos_x_mm;}
    int get_line_y_mm(){return line_pos_y_mm;}

private:
	AnalogComparator* _comp;
	volatile int line_pos_x_mm;  // position of the last line found.
	volatile int line_pos_y_mm;
};










#endif
