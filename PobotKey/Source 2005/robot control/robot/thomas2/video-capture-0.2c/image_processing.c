

#include "video_device.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

//#include "v4l.h"
#include "image_processing.h"


#define VERSION		"0.2 (looped video capture added by <nick@nick-andrew.net>)"

#define DEF_WIDTH	320
#define DEF_HEIGHT	240
// default colour depth (changing this will break everything)
#define DEF_DEPTH	3

#define MAX_WIDTH	640
#define MAX_HEIGHT	480
#define MAX_DEPTH	3
#define FMT_UNKNOWN	0
#define FMT_PPM		1
#define FMT_PNG		2
#define FMT_JPEG	3
#define FMT_DEFAULT	2

#define IN_TV			0
#define IN_COMPOSITE1	1
#define IN_COMPOSITE2	2
#define IN_SVIDEO		3
#define IN_DEFAULT		0

#define NORM_PAL		0
#define NORM_NTSC		1
#define NORM_SECAM		2
#define NORM_DEFAULT	0

#define QUAL_DEFAULT	80

#define ROUGE 1
#define VERT 2
#define BLEU 3
/* TC */
#define BLANC 4

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

unsigned char sort_and_select (unsigned char *tab){
  int i,j;
  unsigned char min,pos_min;


  for (i = 0;i < 5;i++){
        min=255;
  	for (j = i;j < 9;j++){
        	if (tab[j]<min){
			min=tab[j];
			pos_min=j;
		}
	}
	tab[pos_min]=tab[i];
	tab[i]=min;
  }
  return min;
}

void mediane_filtering(char *image, int width, int height){
  int x,y;
  int dx[9]={0,0,-1,-1,-1,0,1,1,1};
  int dy[9]={0,1,1,0,-1,-1,-1,0,1};
  unsigned char dir,*p,*ptr;
  unsigned char tabr[9],tabg[9],tabb[9];

  p=image+width*3+3;



	for (y = 1;y < height-1;y++){
	   for (x = 1;x < width-1;x++){
        	for (dir = 0;dir < 9;dir++){
			ptr= (p+dx[dir]*3+dy[dir]*width*3);
			tabr[dir]= *(ptr);
			tabg[dir]= *(ptr+1);
			tabb[dir]= *(ptr+2);
	   	}
		*p=sort_and_select(tabr);
		*(p+1)=sort_and_select(tabg);
		*(p+2)=sort_and_select(tabb);
		p+=3;
	   }
	   p+=6;
      }

}

void contour_enhance(char *image, int width, int height){
  int x,y;
  int dx[9]={0,0,-1,-1,-1,0,1,1,1};
  int dy[9]={0,1,1,0,-1,-1,-1,0,1};
  //int coeff[9]={9,-1,-1,-1,-1,-1,-1,-1,-1};
  int coeff[9]={5,1,0,1,0,1,0,1,0};
  int p1,p2,p3,pr,pg,pb;
  unsigned char dir,*p;
  int pos;


  p=image+width*3+3;


 for (y = 1;y < height-1;y++){
	   for (x = 1;x < width-1;x++){
		p1=5*p[0];pr=p[0];
		p2=0;pg=p[1];
		p3=0;pb=p[2];
		for(dir = 1;dir < 9;dir++){
		   pos = dx[dir]*3+dy[dir]*width*3;
		   p1=p1-(int)(p[pos])*coeff[dir];
		   p2=p2-(int)(p[1+pos])*coeff[dir];
		   p3=p3-(int)(p[2+pos])*coeff[dir];
		}
	       p[0]=(unsigned char)(p1);
	       //p[1]=(unsigned char)(p2/5+pg)*5;
	       //p[2]=(unsigned char)(p3/5+pb)*5;
               p+=3;
	   }
	   p+=6;
 }

}

void contrast_enhance(char *image, int width, int height){
  int x,y;
  unsigned char *p;
  unsigned char minr=255,maxr=0,ming=255,maxg=0,minb=255,maxb=0;
  int diffr,diffg,diffb;

  p=image;


 for (y = 0;y < height;y++){
     for (x = 0;x < width;x++){
       if (*p>maxr) maxr=*p;
       if (*p<minr) minr=*p;
       if (*(p+1)>maxg) maxg=*(p+1);
       if (*(p+1)<ming) ming=*(p+1);
       if (*(p+2)>maxb) maxb=*(p+2);
       if (*(p+2)<minb) minb=*(p+2);
       p+=3;
     }
  }
  diffr=maxr-minr;
  diffg=maxg-ming;
  diffb=maxb-minb;
  p=image;
  for (y = 0;y < height;y++){
     for (x = 0;x < width;x++){
       p[0]=((p[0]-minr)*255)/diffr;
       p[1]=((p[1]-ming)*255)/diffg;
       p[2]=((p[2]-minb)*255)/diffb;
       p+=3;
     }
  }
}

void stretching_histo(char *image, int width, int height,int d){
  int x,y;
  int a,b;
  unsigned char *p;
  int p1,p2,p3;

  a=(127-d);
  b=-(d*127)/a;
  p=image;


 for (y = 0;y < height;y++){
     for (x = 0;x < width;x++){
          p1=(p[0]*127)/a+b;
	  p2=(p[1]*127)/a+b;
	  p3=(p[2]*127)/a+b;
	  if (p1<0) p1=0;
	  if (p1>255) p1=255;
	  p[0]=p1;
	  if (p2<0) p2=0;
	  if (p2>255) p2=255;
	  p[1]=p2;
	  if (p3<0) p3=0;
	  if (p3>255) p3=255;
	  p[2]=p3;
       p+=3;
     }
  }
}

void shrinking_histo(char *image, int width, int height,int d){
  int x,y;
  int a,b;
  unsigned char *p;
  int p1,p2,p3;

  b=d;
  a=(127-d);

  p=image;


 for (y = 0;y < height;y++){
     for (x = 0;x < width;x++){
          p1=(p[0]*a)/127+b;
	  p2=(p[1]*a)/127+b;
	  p3=(p[2]*a)/127+b;
	  p[0]=p1;
	  p[1]=p2;
	  p[2]=p3;
       p+=3;
     }
  }
}


void rgb2hsv(char *image, float *hsv, int width, int height){
  unsigned char minrgb,maxrgb;
  float delta;
  int x,y;
  float v,s;
  float h;
  unsigned char *p;

  p=image;



  for (y = 0;y < height;y++){
     for (x = 0;x < width;x++){
       maxrgb=p[0];
       if (p[1]>maxrgb) maxrgb=p[1];
       if (p[2]>maxrgb) maxrgb=p[2];
       minrgb=p[0];
       if (p[1]<minrgb) minrgb=p[1];
       if (p[2]<minrgb) minrgb=p[2];
       v=(float)maxrgb/255;
       delta=v-(float)minrgb/255;
       if (maxrgb==0) s=0;
       else s=(delta)/v;
	if (s!=0){
		if (p[2]==maxrgb) h=(((float)p[1]-(float)p[0]))/(delta*255);
		if (p[1]==maxrgb) h=2 + (((float)p[0]-(float)p[2]))/(delta*255);
		if (p[0]==maxrgb) h=4 + (((float)p[2]-(float)p[1]))/(delta*255);
                h=h*60;
		if (h<0) h+=360;
        }
        else h=0;
       p+=3;
       if (x==0 || x==width-1 || y==0 || y==height-1) {h=0.0;s=0.0;v=0.0;}
       hsv[0]=h;
       hsv[1]=s;
       hsv[2]=v;
       hsv+=3;
     }
  }
}
#define SEUIL_RED_S_MIN 0.7f
#define SEUIL_RED_V_MIN 0.2f
#define SEUIL_RED_V_MAX 1.0f
#define SEUIL_RED_H_MAX 30.0f
#define SEUIL_RED_H_MIN 340.0f
#define SEUIL_GREEN_S_MIN 0.25f
#define SEUIL_GREEN_V_MIN 0.1f
#define SEUIL_GREEN_V_MAX 0.7f
#define SEUIL_GREEN_H_MAX 180.0f
#define SEUIL_GREEN_H_MIN 80.0f
#define SEUIL_BLUE_S_MIN 0.6f
#define SEUIL_BLUE_V_MIN 0.2f
#define SEUIL_BLUE_V_MAX 0.9f
#define SEUIL_BLUE_H_MAX 235.0f
#define SEUIL_BLUE_H_MIN 210.0f

/*
#define SEUIL_RED_S_MIN 0.68f
#define SEUIL_RED_V_MIN 0.4f
#define SEUIL_RED_V_MAX 1.0f
#define SEUIL_RED_H_MAX 10.0f
#define SEUIL_RED_H_MIN 350.0f
#define SEUIL_GREEN_S_MIN 0.25f
#define SEUIL_GREEN_V_MIN 0.1f
#define SEUIL_GREEN_V_MAX 0.7f
#define SEUIL_GREEN_H_MAX 180.0f
#define SEUIL_GREEN_H_MIN 80.0f
#define SEUIL_BLUE_S_MIN 0.5f
#define SEUIL_BLUE_V_MIN 0.2f
#define SEUIL_BLUE_V_MAX 1.0f
#define SEUIL_BLUE_H_MAX 235.0f
#define SEUIL_BLUE_H_MIN 180.0f
*/
/* TC */
/* this param modified : max instead of min ! */
#define SEUIL_WHITE_S_MAX 0.3f
#define SEUIL_WHITE_S_MAX 0.0f /* disabled for testing blue !! */
#define SEUIL_WHITE_V_MIN 0.6f
#define SEUIL_WHITE_V_MAX 1.0f
#define SEUIL_WHITE_H_MAX 360.0f
#define SEUIL_WHITE_H_MIN 0.0f


float hue_read(float *image, int x, int y, int width, int height){
 return image [1+x*3+y*width*3];
}

void hue_write(float *image, int x, int y, int width, int height, float val){
 int    pos;
        pos = x*3+y*width*3;
	image [pos]=val;
  	image [1+pos]=0.0;
  	image [2+pos]=0.0;
}

int hue_detect_red(float *image, int p){
       
	if     (image [1+p]>=SEUIL_RED_S_MIN
	&& 	image [2+p]>=SEUIL_RED_V_MIN
	&& 	image [2+p]<=SEUIL_RED_V_MAX
     	&& 	(image [p]<=SEUIL_RED_H_MAX
       	|| 	image [p]>=SEUIL_RED_H_MIN)
	)
  return ROUGE;
  else
  return 0;
}

int hue_detect_green(float *image, int p){

	if     (image [1+p]>=SEUIL_GREEN_S_MIN 
	&& 	image [2+p]>=SEUIL_GREEN_V_MIN
	&& 	image [2+p]<=SEUIL_GREEN_V_MAX
     	&& 	image [p]<=SEUIL_GREEN_H_MAX
       	&& 	image [p]>=SEUIL_GREEN_H_MIN
	)
  return VERT;
  else
  return 0;
}

int hue_detect_blue(float *image, int p){
       
	if     (image [1+p]>=SEUIL_BLUE_S_MIN
	&& 	image [2+p]>=SEUIL_BLUE_V_MIN
	&& 	image [2+p]<=SEUIL_BLUE_V_MAX
     	&& 	image [p]<=SEUIL_BLUE_H_MAX
       	&& 	image [p]>=SEUIL_BLUE_H_MIN
	)
  return BLEU;
  else
  return 0;
}

/* TC */
int hue_detect_white(float *image, int p){
       
	if     (image [1+p]<=SEUIL_WHITE_S_MAX /* TC modified */
	&& 	image [2+p]>=SEUIL_WHITE_V_MIN
	&& 	image [2+p]<=SEUIL_WHITE_V_MAX
     	&& 	image [p]<=SEUIL_WHITE_H_MAX
       	&& 	image [p]>=SEUIL_WHITE_H_MIN
	)
  return BLANC;
  else
  return 0;
}


int hue_detect(float *image, int x, int y, int width, int height, int col){
 int p=x*3+y*width*3;
  
  if (col==-1){
	if (hue_detect_red(image,p)>0) return ROUGE;
	if (hue_detect_green(image,p)>0) return VERT;
	if (hue_detect_blue(image,p)>0) return BLEU;
	if (hue_detect_white(image,p)>0) return BLANC; /* TC */
  }
  else {
	if (col==ROUGE) return hue_detect_red(image,p);
	if (col==VERT) return hue_detect_green(image,p);
	if (col==BLEU) return hue_detect_blue(image,p);
	if (col==BLANC) return hue_detect_white(image,p); /* TC */
  }
 
  return 0; // TC : or -1 ???
}






int freeman_search(float *hsv, freeman_chain *chain, int y1, int width, int height, int *couleur){
 int x,y,precx,precy,precd;
 //int dx[9]={0,-1,-1,-1,0,1,1,1,0};
 //int dy[9]={1,1,0,-1,-1,-1,0,1,1};
 int dx[8]={-1,-1,0,1,1,1,0,-1};
 int dy[8]={0,1,1,1,0,-1,-1,-1};
 float * p=hsv;
 unsigned char fin=0;
 int i,indice;
 int perhaps,d;
 int pos;
 int first[height],last[height];
 int col=-1;

 y1++;
 for (y = y1 ; (y < height-1) && (fin ==0) ; y++){
 	for (x = 1 ; (x < width-1) && (fin ==0) ; x++){
		if (  (col=hue_detect(hsv,x,y,width,height,-1))>0 ){
		   	fin=0;
			for (i = 0 ; (i < 8) && (fin==0) ; i++){
			 	fin+=   hue_detect(hsv,x+dx[i],y+dy[i],width,height,col);
				precd=i;
			}
			if (fin==0) hue_write(hsv,x,y,width,height,0.0); // effacement point isolé
		}
	}
 }

 if (fin==0) return -2; // rien dans l'image, on sort
 chain->x=x;
 chain->y=y;
 chain->surface=0;
 chain->x1=width;
 chain->x2=-1;
 chain->y1=height;
 chain->y2=-1;
 precx=x;
 precy=y;
 pos=0;
 for (i = 0 ; i < height ; i++){
 	first[i]=width;
  	last[i]=-1;
 }

 do{
	perhaps=-1;
	d=-1;
	for (i = 0 ; i < 8 && d==-1; i++){
		indice = ((precd+6)%8+i)%8;
		if ( hue_detect(hsv,x+dx[indice],y+dy[indice],width,height,col))
			d=indice;

	}

	
	if (d==-1) return -1; // point isolé, on sort ! ne devrait pas arriver !
	chain->d[pos]=d;
	if (first[y]>x) first[y]=x;
	if (last[y]<x)  last[y]=x;
   	precx=x;
   	precy=y;
	precd=d;
	x+=dx[d];
	y+=dy[d];
 } while (pos++ < (MAX_POS-2) && !(pos >5 && abs(x-chain->x)<2 && abs(y-chain->y)<2));
 chain->x_bary=0;
 chain->y_bary=0;
 for (y = 0; y < height ; y++){
 	if (first[y]<width){
		if (first[y]<chain->x1) chain->x1=first[y];
		if (last[y]>chain->x2) chain->x2=last[y];
		if (y<chain->y1) chain->y1=y;
		if (y>chain->y2) chain->y2=y;
		//chain->y_bary+=y*(last[y]-first[y]);
	        //chain->x_bary+=((last[y]+first[y])*(last[y]-first[y]))/2;

		for (x = first[y] ; x <= last[y] ; x++){
			hue_write(hsv,x,y,width,height,0.0);
			chain->surface++;
			chain->x_bary+=x;
			chain->y_bary+=y;
		}
	}
 }
 chain->x_bary=chain->x_bary/chain->surface;
 chain->y_bary=chain->y_bary/chain->surface;
 *couleur=col;
 return pos;
}

void rgb_write(	char *image, int x, int y, int width, int height,
		unsigned char r, unsigned char g, unsigned char b){
	image[x*3+y*width*3]=r;
	image[1+x*3+y*width*3]=g;
	image[2+x*3+y*width*3]=b;
}

int freeman(char *image, int width, int height, object *a_object, int display){
 unsigned char *p=image;
 float hsv[MAX_WIDTH*MAX_HEIGHT*MAX_DEPTH];
 int x,y;
 //int dx[8]={0,-1,-1,-1,0,1,1,1};
 //int dy[8]={1,1,0,-1,-1,-1,0,1};
 int dx[8]={-1,-1,0,1,1,1,0,-1};
 int dy[8]={0,1,1,1,0,-1,-1,-1};
 freeman_chain chain;
 int pos,nb_pos;
 int i=0;
 int col;
 int nb_object=0;

 chain.y=0;
 rgb2hsv(p,hsv,width,height);
 do {
	nb_pos=freeman_search(hsv,&chain,chain.y+1,width,height,&col);
	//fprintf(stderr,"pos %d x %d y %d\n",nb_pos,chain.x,chain.y);
 
	x=chain.x;
	y=chain.y;
	pos=0;
	
	if (display){
		if (nb_pos>0 && chain.surface>100){
			rgb_write(p,x,y,width,height,255,255,255);
			do{
				x+=dx[chain.d[pos]];
				y+=dy[chain.d[pos]];
				rgb_write(p,x,y,width,height,255,255,255);
			}while(pos++<nb_pos);


		}
	}
	
	if ((nb_pos>0) && (chain.surface>100) && (nb_object < NB_OBJECT_MAX) ){
		a_object->x1=chain.x1;
		a_object->x2=chain.x2;
		a_object->y1=chain.y1;
		a_object->y2=chain.y2;
		a_object->x_bary=chain.x_bary;
		a_object->y_bary=chain.y_bary-5;
		a_object->surface=chain.surface;
		a_object->nb_pos=nb_pos;
		a_object->color=col;
		a_object++;
		nb_object++;
	}

       i++;
 
 }while (nb_pos!=-2);
 return nb_object;
}

void display(char *image, object *a_object, int nb_object, int width, int height){
 int i,x,y;
 unsigned char *p=image;
 	for (i=0; i<nb_object ; i++){
		for (x=a_object->x1;x<=a_object->x2;x++)
		rgb_write(p,x,a_object->y1,width,height,10,255,10);
		for (x=a_object->x1;x<=a_object->x2;x++)
		rgb_write(p,x,a_object->y2,width,height,10,255,10);
		for (y=a_object->y1;y<=a_object->y2;y++)
		rgb_write(p,a_object->x1,y,width,height,10,255,10);
		for (y=a_object->y1;y<=a_object->y2;y++)
		rgb_write(p,a_object->x2,y,width,height,10,255,10);
		for (x=a_object->x_bary-5;x<=a_object->x_bary+5;x++)
		rgb_write(p,x,a_object->y_bary,width,height,10,255,10);
		for (y=a_object->y_bary-5;y<=a_object->y_bary+5;y++)
		rgb_write(p,a_object->x_bary,y,width,height,10,255,10);
		fprintf(stderr,"x_bar %d y_bar %d\n",a_object->x_bary,a_object->y_bary);
		fprintf(stderr,"surface %d pos %d couleur %d\n",a_object->surface,a_object->nb_pos,a_object->color);
		a_object++;
	}
}


