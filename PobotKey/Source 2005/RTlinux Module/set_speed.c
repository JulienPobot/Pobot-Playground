
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "set_speed.h"
#include <rtlinux/rtl_fifo.h>

int fd0,fd1;
int speedd=1,speedg;

int init_rtf(void){
	if ((fd0 = open("/dev/rtf0", O_WRONLY)) < 0) {
		fprintf(stderr, "Error opening /dev/rtf0\n");
		return -1;
	}
 	if ((fd1 = open("/dev/rtf1", O_WRONLY)) < 0) {
		fprintf(stderr, "Error opening /dev/rtf0\n");
  		close (fd0);
		return -1;
	}
        return 0;
}

void set_speed(int speedd , int speedg ){
	write(fd0, &speedd, sizeof(speedd));
 	write(fd1, &speedg, sizeof(speedg));
}


int main(void){
  init_rtf();
  while (speedd!=0){
	printf("Spped Right[0-2000]:");
	scanf("%d",&speedd);
	
	printf("Spped Left [0-2000]:");
	scanf("%d",&speedg);
	
	set_speed(speedd,speedg);
	fflush(stdout);
  }
  return 0;
}
