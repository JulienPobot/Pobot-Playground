#include <stdio.h>
#include <math.h>

#define PIx2  6.283185307
#define PI   3.141592654
#define PI_2 1.570796327
#define PI_4 0.785398163

#define STEP 0.26 		// 50/240
#define dist 265.0		// distance odometres D-G
#define dtheta 0.000981132     	// 2ArcSin(STEP/2xdist)

#define INIT_SIZE 6
unsigned char init[INIT_SIZE]={0xf5,0xf3,200,0xe8,0x03,0xf4};


int main (int arg,char** argv){
   FILE *fp;
   unsigned char a;
   signed char x_signe,y_signe;
   int i;
   double x=0.0,y=0.0,theta=PI_2,s=0;
   int d=0,g=0;
  fprintf(stderr,"start\n");
  fp=fopen("/dev/psaux","r+w");
  
  for (i=0 ; i<INIT_SIZE ; i++){
	  putc(init[i],fp);
	  a=getc(fp);
	  if (a != 0xfa){
		  fprintf(stderr,"init mouse error !EXITING!\n");
		  return -1;
	  }
  }	  
  
  if (fp==NULL) {
    fprintf(stderr,"ouverture impossible psaux !EXITING! \n");
    return -1;
  }
  fprintf(stderr,"ouverture ok \n");
  while (1){
    a=getc(fp);
    if ((a&0x10)==0x10) g=(unsigned char)getc(fp)-256;
    else g=(unsigned char)getc(fp);
    if ((a&0x20)==0x20) d=(unsigned  char)getc(fp)-256;
    else d=(unsigned char)getc(fp);
    s=(g+d)*STEP/2;
    theta+=(d-g)*dtheta;
    if(theta<0) theta+=PIx2;
    if(theta>PIx2) theta-=PIx2;
    x+=s*cos(theta);
    y+=s*sin(theta);
    fprintf(stderr,"x:%f y:%f theta:%f \n",x,y,theta);
  }
}
