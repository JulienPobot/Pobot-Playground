/*
 * @(#) video-capture.c - modified from vidcat.c
 *
 * Copyright (C) 1998 Rasca, Berlin
 * EMail: thron@gmx.de
 * Modifications (C) 2001, Nick Andrew <nick@nick-andrew.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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
#ifdef HAVE_LIBZ
#include <zlib.h>
#endif
#ifdef HAVE_LIBPNG
#include <png.h>
#endif
#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif
//#include "v4l.h"
#include "image2file.h"
#include "image_processing.h"
// TC 
#include "detection_quilles.h"

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

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
char *couleur[3]={"Rouge","Vert","Bleu"};

/* TC : for beautiful printings ! */
char * etat_quille[4] = {"Couchée", "Debout", "Pile", "Pas une quille"};

/*
 */
void usage (char *pname)
{
	fprintf (stderr,
	"video-capture, Version %s\n"
	"Usage: %s <options>\n"
	" -s NxN                      define size of the output image (default:"
		" %dx%d)\n"
	" -f {ppm|jpeg|png}           output format of the image\n"
	" -i {tv|comp1|comp2|s-video} which input channel to use\n"
	" -q <quality>                only for jpeg: quality setting (1-100,"
		" default: %d)\n"
	" -d <device>                 video device (default: $VIDEO_DEV)\n"
	" -l <loops>                  loop to pick up <loops> frames\n"
	" -b                          make a raw PPM instead of an ASCII one\n"
	" -v                          Verbose mode (log ioctls issued)\n"
	" -F <imageFile.ppm>          take an image as input instead of capturing it with the video device\n" 
	"Example: vidcat | xsetbg stdin\n",
		VERSION, basename(pname), DEF_WIDTH, DEF_HEIGHT, QUAL_DEFAULT);
	exit (1);
}

/*
 */
void image_processing(char *image, int width, int height){
 object a_object[NB_OBJECT_MAX];
 int i, nb_object = 0;
 		// TC : disabled this mediane_filtering ... double used(and seg fault ?) 
	        //mediane_filtering(image, width, height);
    		//shrinking_histo(image, width, height,25);
		//stretching_histo(image, width, height,15);
		//contrast_enhance(image, width, height);
 
 		// TC : put display to 1 to see what's detected !!
		nb_object= freeman(image,width,height,a_object,1);

		//display(image,a_object,nb_object,width,height);
		
		// TC : from 0 to nb_object is better ! 
		for (i = 0; i < nb_object; i++){
			if ( (a_object[i].color != ROUGE) && (a_object[i].color != VERT) ) {
				a_object[i].etat_quille = NOT_QUILLE;
			}
			else {
				a_object[i].etat_quille = detectDebout(a_object[i].x1,
								       a_object[i].x2,
								       a_object[i].y1,
								       a_object[i].y2,
								       a_object[i].surface);
			}

			/* TC : For debuging purposes only */			
			fprintf(stderr,
				"x bary : %u\ty bary : %u\tsurface : %u\tpos : %u\tcol : %s\tquille : %s\n",
				a_object[i].x_bary,
				a_object[i].y_bary,
				a_object[i].surface,
				a_object[i].nb_pos,
				couleur[a_object[i].color-1],
				etat_quille[a_object[i].etat_quille]
				);
		}
}

/*
 * main()
 */
int
main (int argc, char *argv[])
{
	int width = DEF_WIDTH, height = DEF_HEIGHT, size, c;
	int	dev;		// fd of open video device
	video_device_t	*vd;	// the instance data
	char *image,*convmap;
	float *hsv;
	char *device = getenv("VIDEO_DEV");
	int max_try = 5;	/* we try 5 seconds/times to open the device */
	int quality = QUAL_DEFAULT;	/* default jpeg quality setting */
	int input = IN_DEFAULT;
	int norm  = NORM_DEFAULT;
	int loop = 1;		/* once around the loop only */
	int frame = 0;		/* the frame number we are capturing */
	int binary = 0;
	int no_save = 0;
	int mediane_filter=0;
	int contour_enhanceb=0;
	int contrast_enhanceb=0;
	int research_object=0;
	int continous_processing=0;

	int fileMode = 0; /* TC : if we use a file rather than a capture */
	const int maxFileNameSize = 50;
	char imgFileName [maxFileNameSize];
	
	int bytes = DEF_DEPTH;
#ifdef HAVE_LIBJPEG
	int format = FMT_JPEG;
#else
#ifdef HAVE_LIBPNG
	int format = FMT_PNG;
#else
	int format = FMT_PPM;
#endif
#endif
	/* TC : add F option for file mode */
	while ((c = getopt (argc, argv, "b:d:s:f:q:i:l:F:tmcorp")) != EOF) {
		switch (c) {
			case 'b':
				binary = 1;
				break;
			case 'd':
				device = optarg;
				break;
			case 's':
				sscanf (optarg, "%dx%d", &width, &height);
				break;
			case 'f':
				if (strcasecmp ("ppm", optarg) == 0)
					format = FMT_PPM;
				else if (strcasecmp ("png", optarg) == 0)
					format = FMT_PNG;
				else if (strcasecmp ("jpeg", optarg) == 0)
					format = FMT_JPEG;
				else
					format = FMT_UNKNOWN;
				break;
			case 'q':
				sscanf (optarg, "%d", &quality);
				break;
			case 'i':
				if (strcasecmp ("tv", optarg) == 0) {
					input = IN_TV;
				} else if (strcasecmp ("comp1", optarg) == 0) {
					input = IN_COMPOSITE1;
				} else if (strcasecmp ("comp2", optarg) ==0) {
					input = IN_COMPOSITE2;
				} else if (strcasecmp ("s-video", optarg) == 0) {
					input = IN_SVIDEO;
				}
				break;
			case 'l':
				loop = -1;
				sscanf(optarg, "%d", &loop);
				fprintf(stderr, "Looping %d times.\n", loop);
				break;
			case 't':
				no_save = 1;
				break;
			case 'm':
			        mediane_filter=1;
			        break;
		        case 'c':
				contour_enhanceb=1;
				break;
			case 'o':
				contrast_enhanceb=1;
				break;
			case 'F': /* TC : switch to file Mode  */
				fileMode = 1;
				strncpy(imgFileName, optarg, maxFileNameSize);
				imgFileName[maxFileNameSize-1] = '\0';
				break;
			case 'r':	
				research_object=1;
				break;	
			case 'p':
				continous_processing=1;
				no_save=1;
				break;
			default:
				usage (argv[0]);
				break;
		}
	}
        
	if (format != FMT_PPM && format != FMT_PNG && format != FMT_JPEG) {
		fprintf(stderr, "Unknown format (%d)\n", format);
		exit(8);
	}
	
	/* TC : check device param but no error if no device and file mode */
	if ((device == (char *) 0) && (!fileMode) ) {
		fprintf(stderr, "No device set - use -d <device>,  set $VIDEO_DEV, or use -F (file mode) \n");
		exit(8);
	}

	/* TC : test file mode : no need to capture ! */
	if (! fileMode) {
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

		vd = vd_setup(width, height, DEF_DEPTH, dev);


		if (!vd_setup_capture_mode(vd)) {
			fprintf(stderr, "Unable to setup capture mode.\n");
			exit(8);
		}

		if (!vd_setup_video_source(vd, input, norm)) {
			// unavailable channel selected
			fprintf(stderr, "Unable to setup video source!\n");
			exit(8);
		}

	} // test if file mode
	
	//frame = (loop == 1) ? -1 : 0;
        frame=-1;
	do {
//		fprintf(stderr, "Getting frame %d\n", frame);

		/* TC : if file mode => read from file instead of capturing */
		if (! fileMode) {
			if (!continous_processing){
				image = (char *) vd_get_image(vd);
				if (!image) {
					fprintf (stderr, "Error: Can't get image\n");
					exit(8);
				}
				vd_image_done(vd);
				fprintf(stderr,"image ok \n");
			}
			else
				vd_get_and_process_image(vd,image_processing);

			}
		else {
			image = getImageFromFile(imgFileName, width*height*3);
			if (!image) {
				fprintf (stderr, "Error: Can't get image\n");
				exit(8);
			}
			fprintf(stderr,"image ok \n");
			
		}

		if (!no_save) {
		if (mediane_filter==1) mediane_filtering(image, width, height);
    		//if (contour_enhanceb==1) shrinking_histo(image, width, height,25);
		if (contour_enhanceb==1) stretching_histo(image, width, height,15);
		if (contrast_enhanceb==1) contrast_enhance(image, width, height);
		if (research_object==1) image_processing(image,width,height);
		/* now save it */
		switch (format) {
			case FMT_PPM:
				put_image_ppm (image, width, height, binary, frame);
				break;
			case FMT_PNG:
				put_image_png (image, width, height, frame);
				break;
			case FMT_JPEG:
				put_image_jpeg (image, width, height, quality, frame);
				break;
			default:
				fprintf(stderr, "No known output format %d!\n", format);
				/*NOTREACHED*/
				break;
		}}

		// Now tell the driver we're done with it
		//vd_image_done(vd);

		frame++;

	} while (0);

	/* TC : clode capture device if not in file mode or free image mem space */
	if (! fileMode)  vd_close(vd);
	else free (image);	
	
	return 0;
}
