#ifndef WORLD_H
#define WORLD_H

#define COLOR_GREEN  0
#define COLOR_RED    1

#define SWITCH_TO_COLOR HIGH // if switch  equal this value, the result is the color

//  ____________
// | l__l__l__l |   ^
// |            |   | 
// |_    ()    _|   | x
// | |        | |   |
// |-+--------+-|   +
// RED      GREEN
//
// <------------+
//       y

// values (for green)  (in mm)
#define ZONE_X_MIN 300
#define ZONE_X_MAX 1820
#define ZONE_Y_MIN 300
#define ZONE_Y_MAX 1150

#define LINE_INTERVAL 130 //distance between two line, in mm

#define LINE_EXT        0  // the "exterior" line, whatever the color
#define LINE_MIDDLE_EXT 1
#define LINE_MIDDLE_IN  2
#define LINE_IN         3  // the "interior" line, whatever the color

#define ELEMENT_NUMBER 12

#define ELEM_STATE_UNKNOWN 0
#define ELEM_STATE_PRESENT 1
#define ELEM_STATE_ABSENT 2

// absolute line y position in mm
int lines_y[][4] = 
    {{    // green
	705,
	835,
	965,
	1095
      },{  // red
	2295,
	2165,
	2035,
	1905
    }};
// absolute boundaries, in mm
int zone_x_min[] = {ZONE_X_MIN , ZONE_X_MIN};
int zone_x_max[] = {ZONE_X_MAX , ZONE_X_MAX};
int zone_y_min[] = {ZONE_Y_MIN , 3000-ZONE_Y_MAX};
int zone_y_max[] = {ZONE_Y_MAX , 3000-ZONE_Y_MIN};


// the current match
class Match{
public:
  Match(){
    set_color(COLOR_GREEN); // green by default... but we should maybe remove this :/
    used_line = LINE_MIDDLE_IN; // default is middle in but we should never use it without setting the line before
    elements_count = 0;	// nb element position find
  }
  
  void set_color(char col){
    color = col;
  }
  char get_color(){
    return color; 
  }

  //position is ok to go to (x, y absolute, in mm)
  bool valid_position(int x, int y){ 
    bool x_valid = x >= get_x_min() && x <= get_x_max();
    bool y_valid = y >= get_y_min() && y <= get_y_max();
    return x_valid && y_valid;
  }

  // determines which line we are on
  // returns false if we are nor on a known line, true otherwise
  bool set_used_line(int y){
    bool found  = false;
    for(char i = 0; i < 4; i++){
      int delta = LINE_INTERVAL / 2; // half of the distance between two lines in mm
      int line_pos = lines_y[color][i];
      if( y > line_pos - delta && y < line_pos + delta ){
        used_line = i;
        found = true;
      }
    }
    return found;
  }

  int get_used_line_y(){
    return lines_y[color][used_line];
  }


// TODO manage the element positions

// Elements:
//
//  Robot      
//    |       lateral scanning, update of the element presence as we go
//    v
//
//    o    o    o
//
//    o    o    o
//
//    o    o    o
//
//    o    o    o
//
// 
// States :   present(should be), absent, don't know

	int get_elem_x(int i){
		int x = 0;
		switch(i){
			case 1:
			case 5:
			case 9:
				x = 575;
				break;
			case 2:
			case 6:
			case 10:
				x = 775;
				break;
			case 3:
			case 7:
			case 11:
				x = 975;
				break;
			case 4:
			case 8:
			case 12:
				x = 1175;
				break;	
		}
		return x;
	}
	 
	int get_elem_y(int i){
		int y = 0;
		switch(i){
			case 1:
			case 2:
			case 3:
			case 4:
				y = 600;
				break;
			case 5:
			case 6:
			case 7:
			case 8:
				y = 850;
				break;
			case 9:
			case 10:
			case 11:
			case 12:
				y = 1100;
				break;
		}
		
		if(this->color == COLOR_RED) {
			y = 3000 - y;
		}
		
		return y;
	}
	
	int get_elem_sym(int i){
		int sym = 0;
		switch(i){
			case 1:
				sym = 4;
				break;
			case 2:
				sym = 3;
				break;
			case 3:
				sym = 2;
				break;
			case 4:
				sym = 1;
				break;
			case 5:
				sym = 8;
				break;
			case 6:
				sym = 7;
				break;
			case 7:
				sym = 6;
				break;
			case 8:
				sym = 5;
				break;
			case 9:
				sym = 12;
				break;
			case 10:
				sym = 11;
				break;
			case 11:
				sym = 10;
				break;
			case 12:
				sym = 9;
				break;		
		}
		return sym;
	}
		
	char get_elem_state(int i){
		return this->elem_states[i-1];
	}
	
	void init_elem_states(void){
		int i = 0;
		
		for(i = 0; i < ELEMENT_NUMBER; i++) {
			this->elem_states[i] = ELEM_STATE_UNKNOWN;
		}
		
		this->elem_states[0] = ELEM_STATE_PRESENT;
		this->elem_states[3] = ELEM_STATE_PRESENT;
	}
	
	void set_elem_state(int i, char state){
		char old = this->elem_states[i-1];
		this->elem_states[i-1] = state;
		if ( state==ELEM_STATE_PRESENT && (old==ELEM_STATE_ABSENT || old==ELEM_STATE_UNKNOWN) ) elements_count++;
		if ( old==ELEM_STATE_PRESENT && (state==ELEM_STATE_ABSENT || state==ELEM_STATE_UNKNOWN) ) elements_count++;
	}

	void set_sym_elem_state(int i, char state) {
		set_elem_state(i,state);
		int sym = get_elem_sym(i);
		set_elem_state(sym,state);
	}

	// i (line 1 to 4), dist (1, 2, 3nothing)
	void set_element_state_from_first_line(int i, int dist) {
		switch (dist) {
			case 1:
				set_sym_elem_state(i+4, ELEM_STATE_PRESENT);
				break;
			case 2:
				set_sym_elem_state(i+4, ELEM_STATE_ABSENT);
				set_sym_elem_state(i+8, ELEM_STATE_PRESENT);
				break;
			case 3:
				set_sym_elem_state(i+4, ELEM_STATE_ABSENT);
				set_sym_elem_state(i+8, ELEM_STATE_ABSENT);
				break;
			default:
				break;
		}
	}

	char get_elements_discovered() {
		return elements_count;
	}

	void fill_unknow_to_absent() {
		int i;
		for (i=0; i<ELEMENT_NUMBER; i++) {
			if (elem_states[i]==ELEM_STATE_UNKNOWN) {
				elem_states[i] = ELEM_STATE_ABSENT;
			}
		}
	}

	
  int get_x_min(){return zone_x_min[color];}
  int get_x_max(){return zone_x_max[color];}
  int get_y_min(){return zone_y_min[color];}
  int get_y_max(){return zone_y_max[color];}

  void set_target_position(int x, int y) {
    target_x = x;
    target_y = y;
  }
  void get_target_position(int* x, int*y) {
    *x = target_x;
    *y = target_y;
  }
	
private:
  char color;
  char used_line; // the ID of the line used for construction
  char elem_states[ELEMENT_NUMBER];
  char elements_count;
  int target_x, target_y;
};



#endif


