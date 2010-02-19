#include "TrajectoryManager.h"
#include "common.h"
#include "StatusList.h"


TrajectoryManager::TrajectoryManager(){
	reset();
	controller = NULL;
	enable();
}

void TrajectoryManager::set_controller(ConsignController* c){
  controller = c;  
  c->set_trajectory_manager(this);
}


// add high level command
void TrajectoryManager::add_command(char cmd, int param){
  switch(cmd){
	case CMD_STOP:
		// clear all commands in line and stop
		reset();
		controller->reset();
		break;
	case CMD_TURN:
		// param  is the angle in degrees
		add_cmd(CMD_TURN, STATUS_CONSIGN_FINISHED, position_system.deg_to_top(param) );
		break;
	case CMD_TURN_ABS:
		{
		  // param  is the angle in degrees
		  // add_cmd(CMD_TURN, STATUS_CONSIGN_FINISHED, position_system.rad_to_top(radians(param)-position_system.get_theta_rad()) );
                  float theta = (float)param - position_system.get_theta_deg();
                  if (theta>0) {
		    theta = fmod((theta+180) , 360) - 180;
                  } else {
		    theta = -(fmod((-theta+180) , 360) - 180);
                  }
		  add_cmd(CMD_TURN, STATUS_CONSIGN_FINISHED, position_system.rad_to_top(radians(theta)) );
		}
		break;
	case CMD_GO:
		// param is the distance to run in mm,
		add_cmd(CMD_GO, STATUS_CONSIGN_FINISHED, position_system.mm_to_top(param) );
		break;
	case CMD_WAIT:
		// param is the time to wait in ms
		add_cmd(CMD_WAIT, STATUS_CONSIGN_FINISHED, param );
		break;
	default:
		break;
  }
}

void TrajectoryManager::reset(){
	next_cmd = 0;
	next_free_cmd = 0;
	last_command_finished = true;
	wait_between_commands_count = 0;
}

// add low level command
bool TrajectoryManager::add_cmd(char cmd, char trigger_status, long parameter){
   char tmp = next_free_cmd + 1;
   if(tmp >= TRAJECTORY_MANAGER_NB_COMMANDS) tmp = 0;
	
   if (controller && tmp == next_cmd) return false; // no room for this command, sorry
	
	cmds[next_free_cmd].cmd = cmd;
	cmds[next_free_cmd].trigger_status = trigger_status;
	cmds[next_free_cmd].param = parameter;
	next_free_cmd = tmp;
	return true;
	   
} 

long wake_up_time = 0;
bool is_waiting = false;

void TrajectoryManager::update(){
	if(!controller || !enabled) return; // we must have a controller
	//Serial.print((int)status_manager.get());
	
	if(last_command_finished){
		if(next_cmd != next_free_cmd){
			// go to next command
			current_cmd = cmds[next_cmd].cmd;
			switch(current_cmd){
				case CMD_TURN :
					controller->add_angle_consign(cmds[next_cmd].param);
					break;
				case CMD_GO:
					controller->add_dist_consign(cmds[next_cmd].param);
				case CMD_WAIT:
					wake_up_time = millis() + cmds[next_cmd].param;
					is_waiting = true;
					break;
				default:break;
			}
			if (++next_cmd >= TRAJECTORY_MANAGER_NB_COMMANDS) next_cmd = 0;
			last_command_finished = false;
			wait_between_commands_count = 0;
			status_manager.set(STATUS_CONSIGN_RUNNING);	
		} else {
			status_manager.set(STATUS_IDLE);
		}
	} else {
		// see if it has finished since last time we checked
		switch(current_cmd){
			case CMD_TURN :
				// look at the angle quad
				if(controller->angle_quad_finished()){
				//if(status_manager.get() == cmds[next_cmd].trigger_status){
					wait_between_commands_count++;
				} else {
					wait_between_commands_count = 0;
				}
				break;
			case CMD_GO:
				// look at the dist quad
				if(controller->dist_quad_finished()){
				//if(status_manager.get() == cmds[next_cmd].trigger_status){
					wait_between_commands_count++;
				} else {
					wait_between_commands_count = 0;
				}
				break;
			case CMD_WAIT:
				if(millis() >= wake_up_time){
					is_waiting = false;
					wait_between_commands_count = COUNT_TO_WAIT_BETWEEN_COMMANDS + 1;
				} else {
					wait_between_commands_count = 0;
				}
				break;
			default:break;
		}
		
		if(wait_between_commands_count > COUNT_TO_WAIT_BETWEEN_COMMANDS){
			last_command_finished = true;
			status_manager.set(STATUS_CONSIGN_FINISHED);
		}
	}
	
}

void TrajectoryManager::enable() {
  enabled = true;
}

void TrajectoryManager::disable() {
  enabled = false;
}



