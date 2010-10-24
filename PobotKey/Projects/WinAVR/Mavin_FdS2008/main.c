 #include <stdio.h>

#include "mavin.h"
#include "ModuleFunction.h"
#include "timers_2.h"

#define MAX_SPEED 17
#define MIN_SPEED 4

#define MAX_SPEED_SUMO 14

#define BUTTON_STARTSTOP 1
#define BUTTON_MODESEL 2
#define BUTTON_VAL 4

#define BUTTON_SLOT 1
#define BUZZER_SLOT 5

#define LEVELMIC 0x40

#define AVANCE(x) {                                  \
    DC_Control(0, MOTOR_LEFT, x, CW);                \
    DC_Control(0, MOTOR_RIGHT, x, CW);               \
  }
#define RECULE(x) {                                   \
    DC_Control(0, MOTOR_LEFT, x, CCW);                \
    DC_Control(0, MOTOR_RIGHT, x, CCW);               \
  }

#define AVANCE_RIGHT(x, y) {                            \
    DC_Control(0, MOTOR_LEFT, x, CW);                   \
    DC_Control(0, MOTOR_RIGHT, abs(y), y>0? CW:CCW);    \
  }
#define AVANCE_LEFT(x, y) {                         \
    DC_Control(0, MOTOR_LEFT, abs(y), y>0? CW:CCW); \
    DC_Control(0, MOTOR_RIGHT, x, CW);              \
  }

#define RECULE_RIGHT(x, y) {                             \
    DC_Control(0, MOTOR_LEFT, x, CCW);                   \
    DC_Control(0, MOTOR_RIGHT, abs(y), y>0? CCW:CW);     \
  }
#define RECULE_LEFT(x, y) {                         \
    DC_Control(0, MOTOR_LEFT, abs(y), y>0? CCW:CW); \
    DC_Control(0, MOTOR_RIGHT, x, CCW);             \
  }

//White --> 0
//LeftNoir --> 4
//Center --> 2
//Right --> 1

char str_l1[] = "        ";
char str_l2[] = "        ";

#define MODE_LINE 0
#define MODE_SUMO 1
#define MODE_CARRE 2
int mode = MODE_LINE;
int enable = 1;

int val_buttons = 0;

#define STATE_AVANCE 0
#define STATE_TURN 1
void DoCarre(void)
{
  static int state_carre = STATE_AVANCE;

  switch (state_carre) {
  case STATE_AVANCE:
    if (timer_complete(TIMER_RECULE)) {
      timer_start(TIMER_TURN90, TIMER_TURN90_DELAY);
      AVANCE_RIGHT(0, -val_buttons);
      state_carre = STATE_TURN;
    }
    break;
  case STATE_TURN:
    if (timer_complete(TIMER_TURN90)) {
      timer_start(TIMER_RECULE, TIMER_RECULE_DELAY);
      AVANCE(MAX_SPEED_SUMO);
      state_carre = STATE_AVANCE;
    }
    break;
  }
}

void StayInside(void)
{
  static int nb_recule = 0;
  // Motor Control
  // -------------

  GetPhotoSensor(DOWNWARD, 0);
  str_l2[0] = DownwardPhotoSensor & 1? 'x' : ' ';
  str_l2[3] = DownwardPhotoSensor & 2? 'x' : ' ';
  str_l2[7] = DownwardPhotoSensor & 4? 'x' : ' ';

  switch (DownwardPhotoSensor) {
  case PHOTO_CENTER_LEFT:
    if (timer_complete(TIMER_RECULE)) {
      AVANCE_RIGHT(0, -val_buttons);
      timer_start(TIMER_TURN90, TIMER_TURN90_DELAY);
      nb_recule = 0;
    }
    break;

  case PHOTO_CENTER_RIGHT:
    if (timer_complete(TIMER_RECULE)) {
      AVANCE_LEFT(0, -val_buttons);
      timer_start(TIMER_TURN90, TIMER_TURN90_DELAY);
      nb_recule = 0;
    }
    break;

  case PHOTO_LEFT_CENTER_RIGHT:
    if (timer_complete(TIMER_TURN90)) {
      if (nb_recule >= 5) {
        //Anti deadlock
        AVANCE_LEFT(0, -val_buttons);
        timer_start(TIMER_TURN90, TIMER_TURN90_DELAY);
        nb_recule = 0;
      } else {
        RECULE(MAX_SPEED_SUMO);
        timer_start(TIMER_RECULE, TIMER_RECULE_DELAY);
        nb_recule++;
      }
    }
    break;
  } //end switch
  
  if (timer_complete(TIMER_TURN90) &&
      timer_complete(TIMER_RECULE)) {
    AVANCE(MAX_SPEED_SUMO);
  }
}

void FollowLine(void)
{
  // Motor Control
  // -------------

  GetPhotoSensor(DOWNWARD, 0);
  str_l2[0] = DownwardPhotoSensor & 1? 'x' : ' ';
  str_l2[3] = DownwardPhotoSensor & 2? 'x' : ' ';
  str_l2[7] = DownwardPhotoSensor & 4? 'x' : ' ';

  switch (DownwardPhotoSensor) {
  case PHOTO_CENTER:
    AVANCE(MAX_SPEED);
    break;

  case PHOTO_CENTER_RIGHT:
  case PHOTO_RIGHT:
    AVANCE_RIGHT(MAX_SPEED, -val_buttons);
    break;

  case PHOTO_CENTER_LEFT:
  case PHOTO_LEFT:
    AVANCE_LEFT(MAX_SPEED, -val_buttons);
    break;

  case PHOTO_LEFT_CENTER_RIGHT:
    //Not reachable
    AVANCE(MIN_SPEED);
    break;
    
  case PHOTO_ALL_OFF:
    break;
  }
}

int main(void)
{
  Initialize();		
  timer_start(TIMER_TEMP, TIMER_TEMP_DELAY);
  timer_start(TIMER_LCD, TIMER_LCD_DELAY);
  timer_start(TIMER_TURN90, TIMER_TURN90_DELAY);
  timer_start(TIMER_RECULE, TIMER_RECULE_DELAY);
  
  /*
  u08 tempmic;
  while (!GetMicValLuca(LEVELMIC, &tempmic)) {
    snprintf(str_l1, LCD_SIZE, "%02x - %02x", tempmic, LEVELMIC);
  }*/

  int nb_s = 0;

  while (1) {
    timer_handle();

    //Check switches
    if (TactSWModuleCtrl(BUTTON_SLOT, BUTTON_STARTSTOP)) {
      while (TactSWModuleCtrl(BUTTON_SLOT, BUTTON_STARTSTOP));
      if (!enable) {
        enable = 1;
      } else {
        enable = 0;
      }
    }
    if (enable) {
      if (TactSWModuleCtrl(BUTTON_SLOT, BUTTON_MODESEL)) {
        while (TactSWModuleCtrl(BUTTON_SLOT, BUTTON_MODESEL));
        mode++;
        mode %= 3;
      }
    }
    if (TactSWModuleCtrl(BUTTON_SLOT, BUTTON_VAL)) {
      while (TactSWModuleCtrl(BUTTON_SLOT, BUTTON_VAL));
      val_buttons++;
      val_buttons %= MAX_SPEED;
    }

    if (enable) {
      switch (mode) {
      case MODE_LINE:
        FollowLine();
        break;
      case MODE_SUMO:
        StayInside();
        break;
      case MODE_CARRE:
        DoCarre();
        break;
      }
    } else {
      //Stop
      AVANCE(0);
    }

    //Test the timer
    if (timer_complete(TIMER_TEMP)) {
      timer_start(TIMER_TEMP, 500);
      nb_s ++;
      //snprintf(str_l1, LCD_SIZE, "%d", nb_s);
    }

    if (timer_complete(TIMER_LCD)) {
      if (!enable) {
        snprintf(str_l1, LCD_SIZE, "off");
      } else {
        char mode_str[20];
        switch (mode) {
        case MODE_LINE: snprintf(mode_str, LCD_SIZE, "line"); break;
        case MODE_SUMO: snprintf(mode_str, LCD_SIZE, "sumo"); break;
        case MODE_CARRE: snprintf(mode_str, LCD_SIZE, "carr"); break;
        default: snprintf(mode_str, LCD_SIZE, "????"); break;
        }

        snprintf(str_l1, LCD_SIZE, "%s %02d ", mode_str, val_buttons);
       
      }

      timer_start(TIMER_LCD, TIMER_LCD_DELAY);
      lcd_str_out(1,1, str_l1);
      lcd_str_out(2,1, str_l2);
    }
  }

  return 0;
}

