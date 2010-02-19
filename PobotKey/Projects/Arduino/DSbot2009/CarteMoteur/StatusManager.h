#ifndef STATUS_MGR_H
#define STATUS_MGR_H

class StatusManager{
public:
	StatusManager(){status = 0;}
        void set(char s){
          if(status != s){
            //Serial.println((int)s);
            status = s;
          }
        }
	char get(){return status;}
private:
	volatile char status;
};


#endif

