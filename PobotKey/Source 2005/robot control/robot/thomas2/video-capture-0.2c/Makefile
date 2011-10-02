# Makefile for video-capture

VERSION=0.2
FILES=Makefile README index.html video_device.h video_device.c video-capture.c tlphoto.pl
DIST_LOC=slim.tull.net:/var/www/www.nick-andrew.net/projects/video-capture

CFLAGS=-DHAVE_LIBJPEG -O2 -g
LDFLAGS=-ljpeg

all:	video-capture

release:	.release

.release:	index.html tlphoto.pl README video-capture-$(VERSION).tar.gz
	$(MAKE) upload
	touch .release

upload:
	rsync -v -a -e ssh index.html tlphoto.pl README video-capture-$(VERSION).tar.gz $(DIST_LOC)/

README:		index.html
	lynx -dump index.html > README

#video-capture:	video-capture.o video_device.o v4l.o ccvt.o image2file.o image_processing.o
video-capture:	video-capture.o video_device.o  ccvt.o image2file.o image_processing.o detection_quilles.o detection_distance.o detection_ponts.o
video-capture.o:	video_device.h video-capture.c
video_device.o:	video_device.h video_device.c
ccvt.o: ccvt.h ccvt.S
#v4l.o:	v4l.h v4l.c
image2file.o: image2file.h image2file.c
image_processing.o: image_processing.h image_processing.c
detection_quilles.o: detection_quilles.h detection_quilles.c
detection_distance.o: detection_distance.h detection_distance.c
detection_ponts.o: detection_ponts.h detection_ponts.c

video-capture-$(VERSION).tar.gz:	$(FILES)
	tar zcvf video-capture-$(VERSION).tar.gz $(FILES)
