/*
 * @(#) video_device.c - Abstraction layer for a video capture device
 *
 * (C) 2001, Nick Andrew <nick@nick-andrew.net>
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
#include "ccvt.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXWIDTH 640
#define MAXHEIGHT 480
#define MAXDEPTH 3

// Create a new instance of video_device_t

video_device_t	*vd_setup(int width, int height, int depth, int dev) {
	video_device_t *vd = (video_device_t *)malloc(sizeof(video_device_t));
	if (!vd) return vd;

	vd->width = width;
	vd->height = height;
	vd->depth = depth;
	vd->buffer_size = width * height * depth;
	vd->dev = dev;
	vd->use_mmap = 1;
	vd->capturing = 0;
	vd->debug = 0;
	vd->debug_fp = 0;

	return vd;
}

// Decide whether to capture frames using read() or mmap() and ioctl
// and if so, begin capturing frames

int	vd_setup_capture_mode(video_device_t *vd) {

	assert(vd != (video_device_t *)0);

	vd->vmbuf.size=vd->buffer_size;
	vd->vmbuf.frames=2;
	fprintf(stderr, "VIDIOCGMBUF return: size = %d\n", vd->vmbuf.size);
	fprintf(stderr, "frames: %d\n", vd->vmbuf.frames);
	fprintf(stderr, "\n");
	/*
	// mmap is okay!
	if (!vd->use_mmap) {
		fprintf(stderr, "Reversing earlier decision to NOT use mmap\n");
		vd->use_mmap = 1;
	}
        */
	vd->frame_buffer = mmap(0, vd->vmbuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, vd->dev, 0);
	if (vd->frame_buffer == (unsigned char *) -1) {
		perror("mmap vd->frame_buffer");
		return 0;
	}

	vd->vmmap.format = VIDEO_PALETTE_YUV420P;		// KLUDGE ...
	vd->vmmap.frame = 0;				// Start at frame 0
	vd->vmmap.width = vd->width;
	vd->vmmap.height = vd->height;

	fprintf(stderr, "Set Format: %08x\n", vd->vmmap.format);
	fprintf(stderr, "Set Frame: %d\n", vd->vmmap.frame);
	fprintf(stderr, "Set Width: %d\n", vd->vmmap.width);
	fprintf(stderr, "Set Height: %d\n", vd->vmmap.height);

	return 1;
}

// Destroy the instance of video_device_t (close device, free memory etc)
void	vd_close(video_device_t *vd) {
	assert(vd != (video_device_t *)0);

	if (vd->frame_buffer) {
		if (vd->use_mmap) {
			munmap(vd->frame_buffer, vd->vmbuf.size);
		} else {
			free(vd->frame_buffer);
		}
	}

	close(vd->dev);

	free(vd);
}

/*
 * Each video4linux video or audio device captures from one or more
 * source channels.
 *
 * Set the channel we will use, and the "norm" for capturing that channel.
 * (input == -1) => no change to the channel ?
 *
 * Return 0 if error
 */

// Set the current video source for this device
int	vd_setup_video_source(video_device_t *vd, int input, int norm) {

	assert(vd != (video_device_t *)0);

	vd->vchan.channel = input;	// Query desired channel

	if (ioctl(vd->dev, VIDIOCGCHAN, &vd->vchan)) {
		perror("VIDIOCGCHAN");
		return 0;
	}

	// Now set the channel and the norm for this channel

	vd->vchan.norm = norm;

	if (ioctl(vd->dev, VIDIOCSCHAN, &vd->vchan)) {
		perror("VIDIOCSCHAN");
		return 0;
	}

	// KLUDGE ... the API leaves colour settings and tuning undefined
	// after a channel change
	return 1;
}

// Get an image
unsigned char *	vd_get_image(video_device_t *vd) {
	int	len;
	int 	area;
	int 	i;
        unsigned char convmap[MAXWIDTH*MAXHEIGHT*MAXDEPTH], dst[4*MAXWIDTH*MAXHEIGHT];


	assert(vd != (video_device_t *)0);

	if (vd->use_mmap) {

		if (ioctl(vd->dev, VIDIOCMCAPTURE, &vd->vmmap)) {
			perror("VIDIOCMCAPTURE");
			return 0;
		}

		// VIDIOCSYNC causes the driver to block until the specified
		// frame is completely received

		if (ioctl(vd->dev, VIDIOCSYNC, &vd->vmmap.frame)) {
			perror("VIDIOCSYNC");
			return 0;
		}

		// Return the buffer, cause it should contain an image
	        vd->frame_buffer = vd->frame_buffer + vd->vmbuf.offsets[vd->vmmap.frame];
	}

	// Otherwise, we have to read the right number of bytes
	
	//v4l_yuv420p2rgb ( convmap, vd->frame_buffer, vd->width, vd->height, vd->depth * 8);
	area = vd->width * vd->height;
	ccvt_420p_bgr24(vd->width, vd->height, vd->frame_buffer, vd->frame_buffer + area, vd->frame_buffer + area + area/4, convmap);
	/*ccvt_420p_rgb32(vd->width, vd->height, vd->frame_buffer, vd->frame_buffer + area, vd->frame_buffer + area + area/4, dst);
	for (i = 0; i < area ; i++){
		convmap[i*3+2]=dst[i*4];
		convmap[i*3+1]=dst[i*4+1];
		convmap[i*3]  =dst[i*4+2];
		
	}*/
	memcpy (vd->frame_buffer, convmap, (size_t) vd->width * vd->height * vd->depth);

	return vd->frame_buffer;
}

// Get an image & process
int	vd_get_and_process_image(video_device_t *vd,void (*ptrfct)(unsigned char *, int, int)) {
	int	len,area,width=vd->width,height=vd->height;
	unsigned char frame0_ok, frame1_ok;
        unsigned char convmap[MAXWIDTH*MAXHEIGHT*MAXDEPTH], * image;

	assert(vd != (video_device_t *)0);
	vd->vmmap.frame=0;
	area = width *height;
	frame0_ok=1;
	if (ioctl(vd->dev, VIDIOCMCAPTURE, &vd->vmmap)) {
		perror("VIDIOCMCAPTURE");
		frame0_ok=0;
	}
	
	while(1){
		vd->vmmap.frame=1;
		frame1_ok=1;
		if (ioctl(vd->dev, VIDIOCMCAPTURE, &vd->vmmap)) {
			perror("VIDIOCMCAPTURE");
			frame1_ok=0;
		}
		// VIDIOCSYNC causes the driver to block until the specified
		// frame is completely received
		if (frame0_ok){
		vd->vmmap.frame=0;
		if (ioctl(vd->dev, VIDIOCSYNC, &vd->vmmap.frame)) {
			perror("VIDIOCSYNC");
			return 0;
		}
		
		printf("frame 0\n");

		// Return the buffer, cause it should contain an image
		image = vd->frame_buffer + vd->vmbuf.offsets[vd->vmmap.frame];

		//v4l_yuv420p2rgb (convmap, image, vd->width, vd->height, vd->depth * 8);
		ccvt_420p_bgr24(width, height, image,  image + area, image + area + area/4, convmap);
		ptrfct(convmap,width, height);
		}
		vd->vmmap.frame=0;
		frame0_ok=1;
		if (ioctl(vd->dev, VIDIOCMCAPTURE, &vd->vmmap)) {
			perror("VIDIOCMCAPTURE");
			frame0_ok=0;0;
		}
		if (frame1_ok){
		vd->vmmap.frame=1;
		if (ioctl(vd->dev, VIDIOCSYNC, &vd->vmmap.frame)) {
			perror("VIDIOCSYNC");
			return 0;
		}

		printf("frame 1\n");

		// Return the buffer, cause it should contain an image
		image = vd->frame_buffer + vd->vmbuf.offsets[vd->vmmap.frame];

		//v4l_yuv420p2rgb (convmap, image, vd->width, vd->height, vd->depth * 8);
		ccvt_420p_bgr24(width, height, image, image + area, image + area + area/4, convmap);
		ptrfct(convmap,width, height);
		}
	}
}

// Tell the device driver we are done with an image, and so capture further
// frames into that
int	vd_image_done(video_device_t *vd) {

	assert(vd != (video_device_t *)0);

	if (vd->use_mmap) {
		// vd->vmmap.frame contains the index of the recently-used buffer
		// So tell the driver to reuse this one for the next frame

		if (ioctl(vd->dev, VIDIOCMCAPTURE, &vd->vmmap)) {
			perror("VIDIOCMCAPTURE");
			return 0;
		}

		// Now cycle the frame number, so we sync the next frame
		if (++vd->vmmap.frame >= vd->vmbuf.frames) {
			vd->vmmap.frame = 0;
		}
	}

	return 1;
}
