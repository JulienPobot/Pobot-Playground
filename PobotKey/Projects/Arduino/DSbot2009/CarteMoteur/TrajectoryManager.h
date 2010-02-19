#ifndef TRAJECTORY_MANAGER_H
#define TRAJECTORY_MANAGER_H


#include "ControlManager.h"

#define TRAJECTORY_MANAGER_NB_COMMANDS 6
#define COUNT_TO_WAIT_BETWEEN_COMMANDS 120


class Command{
public:
	char cmd;
	char trigger_status;
	long param;
};



class TrajectoryManager{
public:
	TrajectoryManager();
	
    void set_controller(ConsignController* c);

	// add high level command
	void add_command(char cmd, int param = 0);

	void reset();
	
	// add low level command
	bool add_cmd(char cmd, char trigger_status, long parameter);

	void update();

	void enable();
	void disable();
	
private:
	bool enabled;
	char next_cmd;
	char next_free_cmd;
	char current_cmd;
	int wait_between_commands_count;
	ConsignController* controller;
	Command cmds[TRAJECTORY_MANAGER_NB_COMMANDS];
	bool last_command_finished;
};




#endif
