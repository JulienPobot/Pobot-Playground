// excellent for 60 and 90 cms
//#define CST_DIST_A -0.687 
//#define CST_DIST_B -0.018 
//#define CST_DIST_A -687
//#define CST_DIST_B -18
//
//good for 120 and 150
//#define CST_DIST_A -2.307
//#define CST_DIST_B -0.034 
//
//excellent for 60 and 150 !!!
//#define CST_DIST_A -1.23
//#define CST_DIST_B -0.0273
//#define CST_DIST_A -1380
//#define CST_DIST_B -29.4

// < 10CM de marge
//#define CST_DIST_A -1.44
//#define CST_DIST_B -0.0278

// ratio for 60 => 90 cm objects
#define CST_DIST_A_TAB1 -707
#define CST_DIST_B_TAB1 -18.7

// ratio for 120 => 150 cm objects
#define CST_DIST_A_TAB2 -2307
#define CST_DIST_B_TAB2 -35.3

// Y position in pixels where switch from TAB1 to TAB2
#define Y2_SWITCH_TAB1_TAB2 85 

// to adjust position when quille is not centered on screen
#define X_CORRECTION 0.00065

/* returns distance between the object and the robot */
int detectDistance(int y2, int xBary);
