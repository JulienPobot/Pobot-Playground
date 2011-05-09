#define AIL 0 // chan1 D2
#define ELE 1
#define THR 2 // D4
#define RUD 3
#define AUX 4 // chan5 D5

#define FRONTL 0  // D9  16bit 50-500hz
#define FRONTR 1  // D10 16bit 50-500hz
#define REAR 2    // D3  8bit  488hz
#define AUX1 3    // D11 8bit  488hz
#define YAW 4     // D8  8bit  50hz
#define AUX2 5    // D12 8bit  50hz

#define MAXCHANIN 7 // max input channel
#define MAXCHANOUT 6 // max output channel

#define CENTER 1520 // in microseconds
#define DT 2020 // in microseconds



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Global value

extern unsigned long cycleTime;
extern int __bss_end;
extern void *__brkval;

// every channel is zero centered with uS as unit so (-500,500)
int channelIn[MAXCHANIN];
int channelOut[MAXCHANOUT];

