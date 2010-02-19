#include "Sensors.h"
#include "common.h"

  AnalogSensor::AnalogSensor(int pin):pin_(pin){
  //  min_ = 0;
  //  max_ = 1023;
  }
  //void AnalogSensor::set_min_max(int min_value, int max_value){
  //  min_ = min_value;
  //  max_ = max_value;
  //}
  int AnalogSensor::get_value(){
    return analogRead(pin_);
    //return ( analogRead(pin_) - min_ ) * 1023 / (max_ - min_) ;
  }






	LineDetector::LineDetector(AnalogComparator* comp):_comp(comp){
		reset();
	}
	
	bool LineDetector::detect(){
		bool found = _comp->update();
		if( found ){
			line_pos_x_mm = position_system.get_x_mm();
			line_pos_y_mm = position_system.get_y_mm();
			_comp->reset();
		}
		return found;
	}

	void LineDetector::reset(){
		_comp->reset();
		line_pos_x_mm = 0;
		line_pos_y_mm = 0;
	}
