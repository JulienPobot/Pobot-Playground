#ifndef TESTS_H
#define TESTS_H

//--------------------------------------------------------
//                      Tests
//--------------------------------------------------------
void init_tests();
void launch_timings();

void print_res(const char* name);

void test_motor_back_and_forth();


unsigned long timing_time = 0;
unsigned long timing_delta = 0;
int timing_offset = 0;
#define TIMING_NB_ITER 1000

int timing_i;

#define BEGIN_TIMING  timing_i = TIMING_NB_ITER;         \
  timing_time = micros();                                \
  do{                                                    \

#define END_TIMING(x)    --timing_i;                     \
  }while(timing_i>0);                                    \
  timing_delta = micros() - timing_time - timing_offset ;\
  print_res(x);


void print_res(const char* name){
  Serial.print(TIMING_NB_ITER);
  Serial.print(" calculs de ");
  Serial.print(name);
  Serial.print(" : \t");
  Serial.print(timing_delta);
  Serial.println("us");
  delay(50);
}


void init_tests(){
  BEGIN_TIMING
  END_TIMING("offset")
  timing_offset = timing_delta;   
}

void launch_timings(){
  BEGIN_TIMING
  do_encoder1();
  END_TIMING("encoder1")
  
  BEGIN_TIMING
  compute_encoder_delta();
  END_TIMING("calcul delta")

  BEGIN_TIMING
  odometry_integration();
  END_TIMING("odometry")
}


#endif