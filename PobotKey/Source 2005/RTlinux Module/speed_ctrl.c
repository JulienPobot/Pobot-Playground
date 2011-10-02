/*
 * Copywrite 2004 pollet pqtrick
 * License GPL V2
 * Author pollet@pobot.org
 */
/*
 * speed ctrl . c
 */
#include <rtl.h>
#include <rtl_fifo.h>
#include <time.h>
#include <rtl_sched.h>
#include <rtl_sync.h>
#include <pthread.h>
#include <posix/unistd.h>
#include <asm/io.h> /* outb */
//#include <stdlib.h>
#include "speed.h"
#include "accel.h"

#define LPT 0x378
#define LPT_CNTRL LPT+2

pthread_t speed_d_thread,speed_g_thread;
void *speed_d_code(void *);
void *speed_g_code(void *);
int fd0,fd1;
int stop = 0;
long speed_d=0,speed_g=0;
long target_speed_d=0, target_speed_g=0;






void *speed_d_code(void *arg){
	struct timespec t;
	unsigned char pins;
	clock_gettime(CLOCK_REALTIME,&t);
	while(!stop){
 		pins=inb(LPT);
		if (speed_d <0) outb(pins|0x04,LPT);
  		else outb(pins&0xfb,LPT);
		pins=inb(LPT);
		if (abs(speed_d)>0) outb(pins|0x01,LPT);
		timespec_add_ns(&t,period[abs(speed_d)]);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL);
  		pins=inb(LPT);
		outb(pins&0xFE,LPT);
		timespec_add_ns(&t,period[abs(speed_d)]);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL);
  		if(abs(target_speed_d-speed_d)<accel[abs(speed_d)])
  			speed_d=target_speed_d;
     		else {
       			if (target_speed_d-speed_d>0)
          			speed_d+=4*accel[abs(speed_d)];
             		else
               			speed_d-=4*accel[abs(speed_d)];
       		}

	}
	return (void *)stop;
}

void *speed_g_code(void *arg){
	struct timespec t;
	unsigned char pins;
	clock_gettime(CLOCK_REALTIME,&t);
	while(!stop){
 		pins=inb(LPT);
   		if (speed_g<0)  outb(pins|0x08,LPT);
     		else outb(pins&0xf7,LPT);
		pins=inb(LPT);
		if (abs(speed_g)>0) outb(pins|0x02,LPT);
		timespec_add_ns(&t,period[abs(speed_g)]);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL);
  		pins=inb(LPT);
		outb(pins&0xFD,LPT);
		timespec_add_ns(&t,period[abs(speed_g)]);
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL);
  		if(abs(target_speed_g-speed_g)<accel[abs(speed_g)])
  			speed_g=target_speed_g;
     		else {
       			if (target_speed_g-speed_g>0)
          			speed_g+=4*accel[abs(speed_g)];
             		else
               			speed_g-=4*accel[abs(speed_g)];
       		}
	}
	return (void *)stop;
}

int param_handler0(unsigned int fifo)
{
	int err=1;

	while(err){
 		err = (rtf_get(0, &target_speed_d, sizeof(target_speed_d)) == sizeof(target_speed_d)) ;
		rtl_printf("speed right set to %d\n",(int)target_speed_d);
	}
	if (err != 0) {
		return -EINVAL;
	}
	return 0;
}

int param_handler1(unsigned int fifo)
{
	int err=1;

	while(err){
  		err = (rtf_get(1, &target_speed_g, sizeof(target_speed_g)) == sizeof(target_speed_g)) ;
		rtl_printf("speed left set to %d\n",(int)target_speed_g);
	}
	if (err != 0) {
		return -EINVAL;
	}
	return 0;
}


int init_module(void){
        rtf_destroy(0);
	rtf_create(0,4000);
	if ( (fd0 = open("/dev/rtf0",O_RDONLY | O_NONBLOCK )) < 0)
	{
		printk("Open fifo0 failed %d\n",errno);
		return -1;
	}

	rtf_destroy(1);
	rtf_create(1,4000);
	if ( (fd1 = open("/dev/rtf1",O_RDONLY | O_NONBLOCK )) < 0)
	{
		printk("Open fifo1 failed %d\n",errno);
		return -1;
	}


	if(pthread_create(&speed_d_thread,NULL,speed_d_code,NULL))
	{
		close(fd0);
		close(fd1);
		printk("Pthread_create failed\n");
		return -1;
	}
 	if(pthread_create(&speed_g_thread,NULL,speed_g_code,NULL))
	{
		close(fd0);
		close(fd1);
		printk("Pthread_create failed\n");
		return -1;
	}
	rtf_create_handler(0, &param_handler0);
	rtf_create_handler(1, &param_handler1);
	return 0;
}

void cleanup_module(void){
        rtf_destroy(0);
	rtf_destroy(1);
	stop = 1;
	pthread_join(speed_d_thread,NULL);
 	pthread_join(speed_g_thread,NULL);
	close(fd0);
	close(fd1);
}

