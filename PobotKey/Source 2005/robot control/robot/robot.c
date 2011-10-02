#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>

#include "controller.h"
#include "set_speed.h"
#include "image_processing.h"
#include "video_device.h"
#include "detection_quilles.h"
#include "detection_ponts.h"
#include "detection_socles.h"
#include "detection_distance.h"


#define DEBUG	1

//#define PETIT_ROBOT 1
#define GROS_ROBOT 1
//#define GROS_ROBOT_TIR1 1
#define GROS_ROBOT_TIR2 1

#define TRUE  1
#define FALSE 0

#define PIx2  			6.283185307
#define PI   			3.141592654
#define PI_2 			1.570796327
#define PI_4 			0.785398163

#ifdef GROS_ROBOT
#define STEP 			0.1988948	// (PI*27,35)/240
#define dist 			208.25		// distance odometres D-G
#define dtheta 			0.000955077     // 2ArcSin(STEP/2xdist)
#endif

#ifdef PETIT_ROBOT
#define STEP 0.077		// (PI*27,35)/240
#define dist 141.0		// distance odometres D-G
#define dtheta  0.0005485     	// 2ArcSin(STEP/2xdist)
#endif

#define INIT_MOUSE_SIZE 	6
#define MATCH_DURATION 		90

#ifdef GROS_ROBOT
#define SPEED_GAIN 		800
#endif

#ifdef PETIT_ROBOT
#define SPEED_GAIN		200
#endif

#define INIT			0
#define PROGRESS		1
#define AIMING			2
#define SHOOTING1		3
#define CAPTURING		4
#define	RELEASING		5
#define LASER_RECKONING		6
#define COLLISION_RECKONING	7
#define SHOOTING2		8
#define	IDLE			9
#define STOP			10
#define SHOOTING3		11
#define HUNTING			12
#define CRUISING		13

#define MAX_SHOOT		8

#define PHASE_START		0
#define PHASE_SHOOT		1
#define PHASE_MOVE		2
#define PHASE_CAPTURING		3
#define PHASE_RELEASING		4
#define PHASE_HUNTING		5

#define AL 0
#define AR 1

#ifdef GROS_ROBOT
#define SHOOT_PORT "/dev/ttyS0"
#define CAPT_PORT  "/dev/ttyS1"
#define RELEVEUR_PORT  "/dev/ttyS2"
#endif

#ifdef PETIT_ROBOT
#define MOUSE_PORT "/dev/ttyS0"
#endif

#define IN_DEFAULT		0
#define NORM_DEFAULT		0

#define WIDTH			320
#define HEIGHT			240
#define DEPTH			3

#define ROUGE	1
#define VERT	2
#define BLEU	3
#define BLANC	4

pthread_mutex_t mcapteur = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t xy = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pgr_speed = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mimage_info = PTHREAD_MUTEX_INITIALIZER;

SENSOR_VALUE sensor[10];

unsigned char init[INIT_MOUSE_SIZE]={0xf5,0xf3,200,0xe8,0x03,0xf4};

int state,phase;

int match_time;

int jack_inside = FALSE;
int image_info = FALSE;
int color = 0;

int nb_object;
object a_object[NB_OBJECT_MAX];

FILE *fp;
#ifdef GROS_ROBOT
int SHOOT,CAPT,RELEVEUR;
#endif

#ifdef PETIT_ROBOT
int MOUSE;
#endif
double  x=210.0,y=125.0,theta=PI_2;
int progress_speed_right, progress_speed_left;

int run=0,progress_ok=0;

video_device_t	*vd;

video_device_t * init_video( char *device, int width, int height, int depth){
	int dev=0, max_try=5;
        int input = IN_DEFAULT;
	int norm  = NORM_DEFAULT;
	video_device_t *vd;

	if (device == (char *) 0) {
		fprintf(stderr, "No device set \n");
		exit(8);
	}

	/* open the video4linux device */
	while (max_try) {
		dev = open (device, O_RDWR);
		if (dev == -1) {
			if (!--max_try) {
				fprintf (stderr, "Can't open device %s\n", device);
				exit (0);
			}
			sleep (1);
		} else {
			break;
		}
	}

	vd = vd_setup(width, height, depth, dev);


	if (!vd_setup_capture_mode(vd)) {
		fprintf(stderr, "Unable to setup capture mode.\n");
		exit(8);
	}

	if (!vd_setup_video_source(vd, input, norm)) {
		// unavailable channel selected
		fprintf(stderr, "Unable to setup video source!\n");
		exit(8);
	}

	return vd;
}

void image_process (char * image, int width, int height){
	int i,j,state;
	int nb_object;
	object a_object[NB_OBJECT_MAX];
	char *color[3]={"Rouge","Vert","Bleu"};
	char *etat[8]={"Couche","Debout","Pile","Not a Quille","Doute_Gauche","Doute_Droite","Doute_Bas","Doute_Haut"};
	pthread_mutex_lock(&mimage_info);
	state=!image_info;
	pthread_mutex_unlock(&mimage_info);
	if (state){
		mediane_filtering(image,width,height);
		nb_object=freeman(image,width,height,a_object,0);
		fprintf(stderr,"nb object %u\n",nb_object);
		for(i=0;i<nb_object;i++) {
			if ( (a_object[i].color != ROUGE) && (a_object[i].color != VERT) ) {

				if (a_object[i].color == BLANC) {
 					a_object[i].etat_object = detectSocle(a_object[i].x1,
		                        				       a_object[i].x2,
				        				       a_object[i].y1,
				                                               a_object[i].y2,
				                                               a_object[i].surface);
					// ajouter ici un add_obstacle

				}
				else {
					a_object[i].etat_object = NOT_QUILLE;
				}
			}
			else {
				a_object[i].etat_object = detectDebout(a_object[i].x1,
								       a_object[i].x2,
								       a_object[i].y1,
							 	       a_object[i].y2,
								       a_object[i].surface);
			}

			a_object[i].distance = detectDistance(a_object[i].y2, a_object[i].x_bary, a_object[i].etat_object);
			a_object[i].ecart_axe = getDistanceAxeRobot(a_object[i].y2, a_object[i].x_bary,a_object[i].distance);
			fprintf(stderr,"%s ",color[a_object[i].color-1]);
			fprintf(stderr,"%s ",etat[a_object[i].etat_object]);
		}
		fprintf(stderr,"\n");
		pthread_mutex_lock(&mimage_info);
		image_info=TRUE;
		pthread_mutex_unlock(&mimage_info);
	}
}

void *Video_thread(void * param){

	if ((vd=init_video("/dev/video0",WIDTH,HEIGHT,DEPTH)))
		printf("init video ok\n");
	else
		printf("can't init video \n");

	vd_get_and_process_image((video_device_t *)vd,image_process);
}
#ifdef GROS_ROBOT
int init_releveur(void) {
   struct termios newtio;
   fprintf(stderr,"start releveur com\n");
   RELEVEUR = open (RELEVEUR_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (RELEVEUR < 0) {perror(RELEVEUR_PORT);exit(-1);}

	fcntl(RELEVEUR, F_SETFL, 0);
	tcgetattr(RELEVEUR,&newtio);

	cfsetispeed(&newtio,B57600);
	cfsetospeed(&newtio,B57600);
	newtio.c_cflag|=(CLOCAL | CREAD);
	newtio.c_cflag &=~PARENB;
	newtio.c_cflag &=~CSTOPB;
	newtio.c_cflag &=~CSIZE;
	newtio.c_cflag |=CS8;
	newtio.c_cflag &=~CRTSCTS;

	newtio.c_lflag &=~(ICANON | ECHO | ECHOE | ISIG);

	newtio.c_iflag |= IGNPAR  ;
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY | IGNCR | INLCR | ICRNL |IUCLC);

	newtio.c_oflag &= ~OPOST;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;

	tcsetattr(RELEVEUR , TCSAFLUSH , &newtio);
     return 0;
}

int close_releveur(void){
  return close(RELEVEUR);
}

void  *Releveur_thread(void *param){
unsigned char buf[7];
unsigned char capteur[10];
int i;

    while (!run) usleep(50000);
    tcflush(CAPT,TCIOFLUSH);
    while (run) {
	while (read(CAPT,buf,1)==0);
	usleep(5000);
    }
}

int init_shoot(void) {
   struct termios newtio;
   fprintf(stderr,"start shoot com\n");
   SHOOT = open (SHOOT_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (SHOOT < 0) {perror(SHOOT_PORT);exit(-1);}

	fcntl(SHOOT, F_SETFL, 0);
	tcgetattr(SHOOT,&newtio);

	cfsetispeed(&newtio,B57600);
	cfsetospeed(&newtio,B57600);
	newtio.c_cflag|=(CLOCAL | CREAD);
	newtio.c_cflag &=~PARENB;
	newtio.c_cflag &=~CSTOPB;
	newtio.c_cflag &=~CSIZE;
	newtio.c_cflag |=CS8;
	newtio.c_cflag &=~CRTSCTS;

	newtio.c_lflag &=~(ICANON | ECHO | ECHOE | ISIG);

	newtio.c_iflag |= IGNPAR  ;
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY | IGNCR | INLCR | ICRNL |IUCLC);

	newtio.c_oflag &= ~OPOST;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;

	tcsetattr(SHOOT , TCSAFLUSH , &newtio);
     return 0;
}

int close_shoot(void){
  return close(SHOOT);
}

int init_capteur(void) {
   struct termios newtio;
   fprintf(stderr,"start capt com\n");
   CAPT = open (CAPT_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (CAPT < 0) {perror(SHOOT_PORT);exit(-1);}

	fcntl(CAPT, F_SETFL, 0);
	tcgetattr(CAPT,&newtio);

	cfsetispeed(&newtio,B57600);
	cfsetospeed(&newtio,B57600);
	newtio.c_cflag|=(CLOCAL | CREAD);
	newtio.c_cflag &=~PARENB;
	newtio.c_cflag &=~CSTOPB;
	newtio.c_cflag &=~CSIZE;
	newtio.c_cflag |=CS8;
	newtio.c_cflag &=~CRTSCTS;

	newtio.c_lflag &=~(ICANON | ECHO | ECHOE | ISIG);

	newtio.c_iflag |= IGNPAR  ;
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY | IGNCR | INLCR | ICRNL |IUCLC);

	newtio.c_oflag &= ~OPOST;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;

	tcsetattr(CAPT , TCSAFLUSH , &newtio);
     return 0;
}

int init_mouse (void){
  int i;
  unsigned char a;

  fprintf(stderr,"start\n");
  fp=fopen("/dev/psaux","r+w");

 if (fp==NULL) {
    fprintf(stderr,"ouverture impossible psaux !EXITING! \n");
    return -1;
  }

  for (i=0 ; i<INIT_MOUSE_SIZE ; i++){
	  putc(init[i],fp);
	  a=getc(fp);
	  if (a != 0xfa){
		  fprintf(stderr,"init mouse error !EXITING!\n");
		  return -1;
	  }
  }

  fprintf(stderr,"ouverture ok \n");
  return 0;
}

int close_mouse(void){
	return fclose(fp);
}


void *Mouse_thread(void *param){
   unsigned char a;
   double s=0;
   int d=0,g=0;
     while (!run) usleep(50000);
     while (run){
        a=getc(fp);
	if ((a&0x80)==0x80) printf("Y Mouse Overflow \n");
	if ((a&0x40)==0x40) printf("X Mouse Overflow \n");
	if ((a&0x01)==0x01) jack_inside = TRUE;
	else jack_inside = FALSE;
	if ((a&0x02)==0x02) color = ROUGE;
	else color=VERT;
        if ((a&0x10)==0x10) g=(unsigned char)getc(fp)-256;
        else g=(unsigned char)getc(fp);
        if ((a&0x20)==0x20) d=(unsigned  char)getc(fp)-256;
        else d=(unsigned char)getc(fp);
        s=(g+d)*STEP/2;
        pthread_mutex_lock(&xy);
        theta+=(d-g)*dtheta;
        if(theta<0) theta+=PIx2;
        if(theta>PIx2) theta-=PIx2;
        x+=s*cos(theta);
        y+=s*sin(theta);
        pthread_mutex_unlock(&xy);
        //fprintf(stderr,"x:%f y:%f theta:%f \n",x,y,theta);
  }
}


unsigned char conditionneur(int a, int type){

	if (type == AL){
		if (a<0x10) a=0;
		else a=(a-0x10)*4;
	}
	else {
		if (a<0x18) a=0;
		else a=(a-0x18)*2;
	}
	if (a>255) a=255;
 return (unsigned char)a;
}


void  *Capteur_thread(void *param){
unsigned char buf[7];
unsigned char capteur[10];
int i;

    while (!progress_ok) usleep(50000);
    tcflush(CAPT,TCIOFLUSH);
    while (progress_ok) {
    	buf[0]=0xAA;
	write(CAPT,buf,1);
	while (read(CAPT,buf,7)==0);
	capteur[0]=conditionneur(buf[4],AL);
	capteur[1]=conditionneur(buf[3],AL);
	capteur[2]=conditionneur(buf[0],AL);
	capteur[3]=conditionneur(buf[1],AL);
	capteur[4]=conditionneur((buf[0]+buf[3])/4,AL);
	capteur[5]=0;
	capteur[6]=0;
	capteur[7]=0;
	capteur[8]=conditionneur(buf[5],AR);
	capteur[9]=conditionneur(buf[2],AR);
        pthread_mutex_lock(&mcapteur);
	for (i=0;i<10;i++){
		sensor[i]=capteur[i];
		//printf("%f ",sensor[i]);
	}
	//printf("\n");
	pthread_mutex_unlock(&mcapteur);
        usleep(5000);
    }
    close(CAPT);
}
#endif
#ifdef PETIT_ROBOT
int init_mouse(void){
  unsigned char buf[3],nb_trame=0;
   struct termios newtio;
   fprintf(stderr,"start mouse\n");
   MOUSE = open (MOUSE_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (MOUSE < 0) {perror(MOUSE_PORT);exit(-1);}

	fcntl(MOUSE, F_SETFL, 0);
	tcgetattr(MOUSE,&newtio);

	cfsetispeed(&newtio,B19200);
	cfsetospeed(&newtio,B19200);
	newtio.c_cflag|=(CLOCAL | CREAD);
	newtio.c_cflag &=~PARENB;
	newtio.c_cflag &=~CSTOPB;
	newtio.c_cflag &=~CSIZE;
	newtio.c_cflag |=CS8;
	newtio.c_cflag &=~CRTSCTS;

	newtio.c_lflag &=~(ICANON | ECHO | ECHOE | ISIG);

	newtio.c_iflag |= IGNPAR  ;
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY | IGNCR | INLCR | ICRNL |IUCLC);

	newtio.c_oflag &= ~OPOST;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;

	tcsetattr(MOUSE , TCSAFLUSH , &newtio);
  buf[0]=0x47;
  write (MOUSE,buf,1); // pour demarrer

  fprintf(stderr,"ouverture  mouse ok \n");
  return 0;
}

int close_mouse(void){
	return close(MOUSE);
}

void *Mouse_thread(void *param){
 double s=0;
 int d=0,g=0;
 unsigned char buf[3],nb_trame=0;
  while (!run) usleep(50000);
  while (run){
    read(MOUSE,buf,1);
    read(MOUSE,buf+1,1);
    read(MOUSE,buf+2,1);

    //if ((buf[0]&0x80)==0x80) printf("Y Mouse Overflow \n");
    //if ((buf[0]&0x40)==0x40) printf("X Mouse Overflow \n");
    if (nb_trame !=buf[0]) printf("ERROR : Nombre de trames NOK\n");

    if ((buf[2]&0x80)==0x80){
	    g=buf[2];
	    g=g-256;
    }
    else
	    g=buf[2];

    if ((buf[1]&0x80)==0x80) {
	    d=buf[1];
    	    d=d-256;
    }
    else
	    d=buf[1];
    //tot_g=tot_g+g;
    //tot_d=tot_d+d;

    s=(g+d)*STEP/2;
    pthread_mutex_lock(&xy);
    theta+=(d-g)*dtheta;
    if(theta<0) theta+=PIx2;
    if(theta>PIx2) theta-=PIx2;
    x+=s*cos(theta);
    y+=s*sin(theta);
    pthread_mutex_unlock(&xy);
    //fprintf(stdout," %x-%x  %x  %x  \t%d %d     \t%d %d \n",buf[0],nb_trame,buf[1],buf[2],d,g,tot_d,tot_g);
    //fprintf(stderr,"x:%f y:%f theta:%f  g=%d d=%d \n",x,y,theta,g,d);
    //fflush(stdout);
    nb_trame++;
  }
}
#endif
void  *Navigate_thread(void *param){
    ROBOT_POSITION position;
    ROBOT_HEADING heading;
    MOTOR_SPEEDS motor_speeds;

    while (!progress_ok) usleep(50000);
    while (progress_ok) {
        pthread_mutex_lock(&xy);
        position.X=x;
        position.Y=y;
        heading=theta;
        pthread_mutex_unlock(&xy);
        Progress(10,sensor,&position,heading,&motor_speeds);
	pthread_mutex_lock(&pgr_speed);
        progress_speed_right=floor(motor_speeds.speed_right*SPEED_GAIN);
        progress_speed_left=floor(motor_speeds.speed_left*SPEED_GAIN);
	pthread_mutex_unlock(&pgr_speed);
        usleep(5000);
    }
}

void *Strategy_thread(void *param){
     unsigned char buf[3];
     int res;
     int i;
     int fin;
     double costheta, sintheta,xc,yc;
     static int shooting_state;
     int shoot=0;
#ifdef GROS_ROBOT_TIR2
     int attente_viseur,attente_chargeur;
#endif
     while (!run) usleep(50000);
     progress_ok=1;
     shooting_state=SHOOTING1;
     state=INIT;
     phase=PHASE_START;
     while (run) {
	switch (state){
		case INIT	:
			// ajouter ici le code pour l'attente du jack
#ifdef GROS_ROBOT
			fprintf(stderr,"Select Color & Insert Jack PLEASE \07\n");
			while (jack_inside == FALSE){
				printf("\07");
				usleep(100000);
			}
			fprintf(stderr,"--------->   Jack Inserted\n");
			if (color==ROUGE)
				fprintf(stderr,"COLOR ROUGE\n");
			else
				fprintf(stderr,"COLOR VERT");
			while (jack_inside == TRUE){
				printf("\07");
				usleep(250000);
			}
			fprintf(stderr,"--------->   Start Match \n");
			add_obstacle(1350,1800,300,0.8,-1);
			add_obstacle(1800,1800,300,0.8,-1);
			add_obstacle(1565,1600,100,0.5,-1);
			add_target(210.0,215.0,FALSE,FALSE);
			state=PROGRESS;
#endif
#ifdef PETIT_ROBOT
			set_speed(1200,1200);
			sleep(2);
			set_speed(0,0);
			/*add_target(0.0,500.0,FALSE,FALSE);
			add_target(600.0,600.0,FALSE,TRUE);
			add_target(600.0,0.0,FALSE,FALSE);
			add_target(0.0,0.0,FALSE,FALSE);
			state=PROGRESS;*/
			state=STOP;
#endif
			break;
		case PROGRESS 	:
			pthread_mutex_lock(&pgr_speed);
			set_speed(progress_speed_right,progress_speed_left);
			//fprintf(stderr,"speed right = %d  speed left = %d\n",progress_speed_right,progress_speed_left);
			pthread_mutex_unlock(&pgr_speed);
			break;
#ifdef GROS_ROBOT_TIR1
		case AIMING	:
			tcflush(SHOOT,TCIOFLUSH);
			if (shooting_state == SHOOTING1 || shooting_state == SHOOTING2)
				set_speed(-50,50);
			else
				set_speed(50,-50);
			buf[0]=0x41;
			buf[1]=0x10;
			write(SHOOT,buf,2);
			usleep(500000);
			buf[0]=0x41;
			buf[1]=0x20;
			write(SHOOT,buf,2);
			state=shooting_state;
			break;
		case SHOOTING1	:
			if ((res=read(SHOOT,buf,1))!=0){
				printf("%d  %x\n",res,buf[0]);
				if (buf[0]==0xCC) {
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x11;
					write(SHOOT,buf,2);
					buf[0]=0x41;
					buf[1]=0x01;
					write(SHOOT,buf,2);
					buf[0]=0x41;
					buf[1]=0x30;
					write(SHOOT,buf,2);
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					shooting_state=SHOOTING1;
					state=AIMING;
					usleep(100000);
				}
			}
			pthread_mutex_lock(&xy);
			if (theta < 1.1) {
				shooting_state=SHOOTING2;
				add_target(450.0,680.0,FALSE,FALSE);
				add_target(300.0,2100.0,TRUE,FALSE);
				state=PROGRESS;
			}
			pthread_mutex_unlock(&xy);
			break;
		case SHOOTING2	:
			if ((res=read(SHOOT,buf,1))!=0){
		 	printf("%d  %x\n",res,buf[0]);
				if (buf[0]==0xCC) {
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x11;
					write(SHOOT,buf,2);
					buf[0]=0x41;
					buf[1]=0x01;
					write(SHOOT,buf,2);
					buf[0]=0x41;
					buf[1]=0x30;
					write(SHOOT,buf,2);
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					shooting_state=SHOOTING2;
					state=AIMING;
					usleep(100000);
				}
			}
			pthread_mutex_lock(&xy);
			if (theta < 0.8) {
			        if (shoot<MAX_SHOOT) {
					shooting_state=SHOOTING3;
					add_target(850.0,850.0,FALSE,FALSE);
					add_target(1800.0,3000.0,TRUE,FALSE);
					state=PROGRESS;
				}
			}
			pthread_mutex_unlock(&xy);
			break;
		case SHOOTING3	:
			if ((res=read(SHOOT,buf,1))!=0){
		 	printf("%d  %x\n",res,buf[0]);
				if (buf[0]==0xCC) {
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x11;
					write(SHOOT,buf,2);
					buf[0]=0x41;
					buf[1]=0x01;
					write(SHOOT,buf,2);
					buf[0]=0x41;
					buf[1]=0x30;
					write(SHOOT,buf,2);
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					shooting_state=SHOOTING3;
					state=AIMING;
					usleep(100000);
				}
			}
			pthread_mutex_lock(&xy);
			if (theta > 2.1) {
				buf[0]=0x41;
				buf[1]=0x11;
				write(SHOOT,buf,2);
				add_target(450.0,600.0,FALSE,FALSE);
				add_target(2100.0,300.0,TRUE,FALSE);
				phase=PHASE_HUNTING;
			        state=PROGRESS;
			}
			pthread_mutex_unlock(&xy);
			break;
		case CRUISING 	:
			// yeah, baby yeaahhh !!!
			break;
		case HUNTING	:
			add_target(900.0,1100.0,FALSE,FALSE);
			add_target(1800.0,1270.0,FALSE,FALSE);
			add_target(1800.0,200.0,FALSE,FALSE);
			add_target(300.0,300.0,FALSE,FALSE);
			state=PROGRESS;
			phase=PHASE_RELEASING;
			/*pthread_mutex_lock(&mimage_info);
			image_info = FALSE;
			pthread_mutex_unlock(&mimage_info);
			fin=FALSE;
			while (image_info == FALSE)
				usleep(50000);
			// la une image est dispo et les infos sont a jour
			for (i=0;i<(nb_object) ;i++){
				if (a_object[i].color == color && a_object[i].etat_object == QUILLE_COUCHEE){
					fin=TRUE;
					break;
					}
			}
			if (fin == TRUE) {
				pthread_mutex_lock(&xy);
				sintheta=sin(theta);
				costheta=cos(theta);
				xc=x+a_object[i].distance*10*costheta-a_object[i].ecart_axe*sintheta;
				yc=y+a_object[i].distance*10*sintheta+a_object[i].ecart_axe*costheta;
				pthread_mutex_unlock(&xy);
				add_target(xc,yc,FALSE,FALSE);
				phase=PHASE_CAPTURING;
				state=PROGRESS;
			}
			else {
				add_target(900.0,1320.0,FALSE,FALSE);
				add_target(1950.0,1320.0,FALSE,FALSE);
				add_target(1950.0,200.0,FALSE,FALSE);
				add_target(200.0,200.0,FALSE,FALSE);
				state=PROGRESS;
				phase=PHASE_HUNTING;
			}*/

			break;
		case CAPTURING	:
			add_target(250.0,250.0,FALSE,FALSE);
			phase=PHASE_RELEASING;
			state=PROGRESS;
			break;
		case RELEASING	:
			break;
		case LASER_RECKONING	:
			break;
		case COLLISION_RECKONING:
			break;
		case IDLE	:
			break;
		case STOP :
			set_speed(0,0);
			break;
#endif
#ifdef GROS_ROBOT_TIR2
		case AIMING	:
			tcflush(SHOOT,TCIOFLUSH);
			buf[0]=0x41;
			buf[1]=0x01;
			write(SHOOT,buf,2);
			buf[0]=0x41;
			buf[1]=0x30;
			if (shooting_state == SHOOTING1 || shooting_state == SHOOTING2)
				set_speed(-50,50);
			else
				set_speed(50,-50);
			buf[0]=0x41;
			buf[1]=0x10;
			write(SHOOT,buf,2);
			usleep(500000);
			buf[0]=0x41;
			buf[1]=0x20;
			write(SHOOT,buf,2);
			state=shooting_state;
			attente_viseur=TRUE;attente_chargeur=TRUE;
			break;
		case SHOOTING1	:
			if ((res=read(SHOOT,buf,1))!=0){
				printf("%d  %x\n",res,buf[0]);
				if (buf[0]==0xCC && attente_viseur==TRUE) {
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x11;
					write(SHOOT,buf,2);
					attente_viseur=FALSE;
				}
				if (buf[0]==0xAA && attente_chargeur==TRUE){
					attente_chargeur=FALSE;
				}
				if (attente_viseur==FALSE && attente_chargeur==FALSE){
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					shooting_state=SHOOTING1;
					state=AIMING;
					usleep(100000);
				}
			}
			pthread_mutex_lock(&xy);
			if (theta < 1.1) {
				shooting_state=SHOOTING2;
				add_target(450.0,680.0,FALSE,FALSE);
				add_target(300.0,2100.0,TRUE,FALSE);
				state=PROGRESS;
			}
			pthread_mutex_unlock(&xy);
			break;
		case SHOOTING2	:
			if ((res=read(SHOOT,buf,1))!=0){
				printf("%d  %x\n",res,buf[0]);
				if (buf[0]==0xCC && attente_viseur==TRUE) {
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x11;
					write(SHOOT,buf,2);
					attente_viseur=FALSE;
				}
				if (buf[0]==0xAA && attente_chargeur==TRUE){
					attente_chargeur=FALSE;
				}
				if (attente_viseur==FALSE && attente_chargeur==FALSE){
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					shooting_state=SHOOTING2;
					state=AIMING;
					usleep(100000);
				}
			}
			pthread_mutex_lock(&xy);
			if (theta < 0.8) {
			        if (shoot<MAX_SHOOT) {
					shooting_state=SHOOTING3;
					add_target(850.0,850.0,FALSE,FALSE);
					add_target(1800.0,3000.0,TRUE,FALSE);
					state=PROGRESS;
				}
			}
			pthread_mutex_unlock(&xy);
			break;
		case SHOOTING3	:
			if ((res=read(SHOOT,buf,1))!=0){
				printf("%d  %x\n",res,buf[0]);
				if (buf[0]==0xCC && attente_viseur==TRUE) {
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x11;
					write(SHOOT,buf,2);
					attente_viseur=FALSE;
				}
				if (buf[0]==0xAA && attente_chargeur==TRUE){
					attente_chargeur=FALSE;
				}
				if (attente_viseur==FALSE && attente_chargeur==FALSE){
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
					shooting_state=SHOOTING3;
					state=AIMING;
					usleep(100000);
				}
			}
			if (theta > 2.1) {
				buf[0]=0x41;
				buf[1]=0x11;
				write(SHOOT,buf,2);
				if (shoot==0){
					set_speed(-50,50);
					usleep(1500000);
					set_speed(0,0);
					buf[0]=0x41;
					buf[1]=0x02;
					write(SHOOT,buf,2);
					shoot++;
					while (read(SHOOT,buf,1)==0){
						usleep(50000);
					}
				}
				add_target(450.0,600.0,FALSE,FALSE);
				add_target(2100.0,300.0,TRUE,FALSE);
				phase=PHASE_HUNTING;
			        state=PROGRESS;
			}
			pthread_mutex_unlock(&xy);
			break;
			case CRUISING 	:
			// yeah, baby yeaahhh !!!
			break;
		case HUNTING	:
			add_target(900.0,1100.0,FALSE,FALSE);
			add_target(1800.0,1270.0,FALSE,FALSE);
			add_target(1800.0,200.0,FALSE,FALSE);
			add_target(300.0,300.0,FALSE,FALSE);
			state=PROGRESS;
			phase=PHASE_RELEASING;
			/*pthread_mutex_lock(&mimage_info);
			image_info = FALSE;
			pthread_mutex_unlock(&mimage_info);
			fin=FALSE;
			while (image_info == FALSE)
				usleep(50000);
			// la une image est dispo et les infos sont a jour
			for (i=0;i<(nb_object) ;i++){
				if (a_object[i].color == color && a_object[i].etat_object == QUILLE_COUCHEE){
					fin=TRUE;
					break;
					}
			}
			if (fin == TRUE) {
				pthread_mutex_lock(&xy);
				sintheta=sin(theta);
				costheta=cos(theta);
				xc=x+a_object[i].distance*10*costheta-a_object[i].ecart_axe*sintheta;
				yc=y+a_object[i].distance*10*sintheta+a_object[i].ecart_axe*costheta;
				pthread_mutex_unlock(&xy);
				add_target(xc,yc,FALSE,FALSE);
				phase=PHASE_CAPTURING;
				state=PROGRESS;
			}
			else {
				add_target(900.0,1320.0,FALSE,FALSE);
				add_target(1950.0,1320.0,FALSE,FALSE);
				add_target(1950.0,200.0,FALSE,FALSE);
				add_target(200.0,200.0,FALSE,FALSE);
				state=PROGRESS;
				phase=PHASE_HUNTING;
			}*/

			break;
		case CAPTURING	:
			add_target(250.0,250.0,FALSE,FALSE);
			phase=PHASE_RELEASING;
			state=PROGRESS;
			break;
		case RELEASING	:
			break;
		case LASER_RECKONING	:
			break;
		case COLLISION_RECKONING:
			break;
		case IDLE	:
			break;
		case STOP :
			set_speed(0,0);
			break;
#endif
	}
        usleep(5000);
     }
     progress_ok=0;
}

void no_more_target(void){
	fprintf(stderr,"callback  \n");
	switch (phase){
		case PHASE_SHOOT :
			set_speed(0,0);
			state=AIMING;
			break;
		case PHASE_START :
			set_speed(0,0);
			state=AIMING;
			phase=PHASE_SHOOT;
			break;
		case PHASE_HUNTING :
			set_speed(0,0);
			state=HUNTING;
			break;
		case PHASE_CAPTURING:
			state=CAPTURING;
			break;
		case PHASE_RELEASING :
			state=STOP;
			break;
		default :
			set_speed(0,0);
			state=IDLE;
			break;
	}
}

void robot_is_blocked(void){
	fprintf(stderr,"                  !!!   ROBOT BLOCKED   !!! \n");
}

int main (int arg,char** argv){
   pthread_t th_mouse,th_navigate,th_strategy,th_capteur,th_video,th_releveur;

   if (pthread_create(&th_mouse,(pthread_attr_t *)NULL,(void *)Mouse_thread,(void *)NULL)!=0) {
        fprintf(stderr," EXITING  pthread Mouse create error\n");
        return -1;
   }

   if (pthread_create(&th_navigate,(pthread_attr_t *)NULL,(void *)Navigate_thread,(void *)NULL)!=0) {
        fprintf(stderr," EXITING  pthread Navigate create error\n");
        return -1;
   }
   if (pthread_create(&th_strategy,(pthread_attr_t *)NULL,(void *)Strategy_thread,(void *)NULL)!=0) {
        fprintf(stderr," EXITING  pthread Strategy create error\n");
        return -1;
   }
#ifdef GROS_ROBOT
   if (pthread_create(&th_capteur,(pthread_attr_t *)NULL,(void *)Capteur_thread,(void *)NULL)!=0) {
        fprintf(stderr," EXITING  pthread Sensor create error\n");
        return -1;
   }
   if (pthread_create(&th_releveur,(pthread_attr_t *)NULL,(void *)Releveur_thread,(void *)NULL)!=0) {
        fprintf(stderr," EXITING  pthread Releveur create error\n");
        return -1;
   }
    /*if (pthread_create(&th_video,(pthread_attr_t *)NULL,(void *)Video_thread,(void *)NULL)!=0) {
        fprintf(stderr," EXITING  pthread Video create error\n");
        return -1;
   }*/
#endif
   if (init_mouse()!=0){
        fprintf(stderr," EXITING  mouse Init error \n");
        return -1;
   }

   if (init_rtf()!=0){
        fprintf(stderr," EXITING rtf init error \n ");
        return -1;
   }
#ifdef GROS_ROBOT
   if (init_shoot()!=0){
   	fprintf(stderr, " EXITING Shoot Init error \n");
	return -1;
   }
   if (init_capteur()!=0){
   	fprintf(stderr, " EXITING Capteur Init error \n");
	return -1;
   }
   if (init_releveur()!=0){
   	fprintf(stderr, " EXITING Releveur Init error \n");
	return -1;
   }
#endif
   init_callback(no_more_target);
   init_blocage(robot_is_blocked);
   run=1;
  // wait MATCH_DURATION
   for (match_time=0 ; match_time < 90 ; match_time ++){
   	usleep(1000000);
   	obstacle_life();
   }
   run=0;
   set_speed(0,0);
   fprintf(stderr,"End of Match, closing Mouse & Ending Threads \n");
   close_mouse();
   pthread_join(th_mouse,0);
   pthread_join(th_navigate,0);
   pthread_join(th_strategy,0);
   pthread_join(th_video,0);
#ifdef GROS_ROBOT
   close_shoot();
   close_releveur();
   pthread_join(th_capteur,0);
   pthread_mutex_destroy(&mcapteur);
#endif
   pthread_mutex_destroy(&xy);
   pthread_mutex_destroy(&pgr_speed);
   return 0;
}
