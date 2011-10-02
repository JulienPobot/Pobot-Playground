#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

#define PIx2  6.283185307
#define PI   3.141592654
#define PI_2 1.570796327
#define PI_4 0.785398163

#define STEP 0.077		// (PI*27,35)/240
#define dist 141.0		// distance odometres D-G
#define dtheta  0.0005485     	// 2ArcSin(STEP/2xdist)

#define BAUDRATE B9600
#define PORT "/dev/ttyS0"


int main (int arg,char** argv){
   int fd;
   double x=0.0,y=0.0,theta=PI_2,s=0;
   int d=0,g=0, tot_g=0,tot_d=0;
   unsigned char buf[3],nb_trame=0;
   struct termios newtio;
   fprintf(stderr,"start\n");
   fd = open (PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {perror(PORT);exit(-1);}

	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd,&newtio);

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

	tcsetattr(fd , TCSAFLUSH , &newtio);
  buf[0]=0x47;
  write (fd,buf,1); // pour demarrer

  fprintf(stderr,"ouverture ok \n");
  while (1){
    read(fd,buf,1);
    read(fd,buf+1,1);
    read(fd,buf+2,1);
    
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
    tot_g=tot_g+g;
    tot_d=tot_d+d;
    
    s=(g+d)*STEP/2;
    theta+=(d-g)*dtheta;
    if(theta<0) theta+=PIx2;
    if(theta>PIx2) theta-=PIx2;
    x+=s*cos(theta);
    y+=s*sin(theta);
    //fprintf(stdout," %x-%x  %x  %x  \t%d %d     \t%d %d \n",buf[0],nb_trame,buf[1],buf[2],d,g,tot_d,tot_g);
    fprintf(stderr,"x:%f y:%f theta:%f  g=%d d=%d \n",x,y,theta,g,d);
    fflush(stdout);
    nb_trame++;
  }
}
