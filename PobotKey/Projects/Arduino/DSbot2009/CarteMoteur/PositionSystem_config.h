#ifndef POSITION_SYSTEM_CONFIG_H
#define POSITION_SYSTEM_CONFIG_H

//--------------------------------------------------------
//                      Encoders
//--------------------------------------------------------

// allows to reverse coders incrementation
//#define C1_REVERSE
#define C2_REVERSE


// ===========  systematic constants   ================

// max ratio between tops [unit used in the code] and ticks[increments of coders]
#define TOPS_PER_TICKS 1024

//(experimentally determined)
#define C1_TicksPerM 8135UL // number of ticks par meter of the first encoder
#define C2_TicksPerM 8125UL // number of ticks per meter of the second encoder
#define track_in_meter 0.2480f //distance between coders in meters


#define tops_to_m(x) ((float) x * meter_per_top)
#define tops_to_cm(x) ((float) x * meter_per_top * 100)
#define tops_to_mm(x) ((float) x * meter_per_top * 1000)





#endif




