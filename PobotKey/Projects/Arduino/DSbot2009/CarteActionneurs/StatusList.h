#ifndef STATUS_LIST_H  
#define STATUS_LIST_H

enum StatusList{
	STATUS_IDLE = 0,  // we are not doing anything (end of a trajectory)
	
	STATUS_COMMAND_ERROR,
	
	// consign controller
	STATUS_CONSIGN_FINISHED,  // end of a movement,  but the trajectory is not yet finished
	STATUS_CONSIGN_RUNNING,   // during a movement
	
	// line following and alignement controllers
	STATUS_LINE_FOLLOWING_RUNNING,
	STATUS_ALIGNEMENT_RUNNING,
	STATUS_ALIGNED,    // at least one of the left and right sensors sees the line
	STATUS_MISALIGNED, // only the center line sensor sees the line
	STATUS_LINE_LOST,  // no line sensor sees the line
	STATUS_BLOCKED     // something blocks us, or collision...
};


#endif
