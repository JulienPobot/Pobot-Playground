/*
 * @(#) video_device.h - Abstraction for a generic video device
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

#ifndef VIDEO_DEVICE_H
#define VIDEO_DEVICE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/videodev.h>

struct	video_device {
	int	width;
	int	height;
	int	depth;		// colour depth
	int	buffer_size;	// always width * height * depth
	int	use_mmap;	// mmap() available for capturing a frame
	int	capturing;	// our device is capturing frames for us
	int	debug;		// log all ioctls and results
	FILE	*debug_fp;	// fp for debug logging

	struct video_capability vcap;
	struct video_channel vchan;
	struct video_mbuf vmbuf;
	struct video_mmap vmmap;
	struct video_window vwin;

	unsigned char	*frame_buffer;	// not the video memory, but one image
	int	dev;		// fd of the physical device
};

typedef struct video_device video_device_t;

/* Functions defined which accept a pointer to video_device_t */

// Create a new instance of video_device_t
video_device_t *vd_setup(int width, int height, int depth, int dev);

// Decide whether to capture frames using read() or mmap() and ioctl
// and if so, begin capturing frames
int	vd_setup_capture_mode(video_device_t *vd);

// Turn debugging (ioctl logging) on or off
void	vd_debug(video_device_t *vd, int flag, FILE *fp);

// Destroy the instance of video_device_t (close device, free memory etc)
void	vd_close(video_device_t *vd);

// Get the device capabilities (internal function)
int	vd_get_capabilities(video_device_t *vd);

// Set the current video source for this device
int	vd_setup_video_source(video_device_t *vd, int input, int norm);

// Get an image
unsigned char *	vd_get_image(video_device_t *vd);

// Tell the device driver we are done with an image, and so put the
// next one into the buffer (if using mmap only)
int	vd_image_done(video_device_t *vd);

// Debugging ioctl
int	vd_ioctl(video_device_t *vd, int cmd, void *arg);

#endif
