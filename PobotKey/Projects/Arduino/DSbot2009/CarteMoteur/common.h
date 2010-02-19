#ifndef COMMON_H
#define COMMON_H


#include "WProgram.h"

#include "params.h"

#include "TrajectoryManager.h"
#include "PositionSystem.h"
#include "Motor.h"
#include "StatusManager.h"

extern CoderSystem coder_system ;
extern PositionSystem position_system;
extern Motor motor1;
extern Motor motor2;
extern StatusManager status_manager;

#endif
