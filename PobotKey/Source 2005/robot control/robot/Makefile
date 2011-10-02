RTL_DIR = /usr/src/rtlinux-3.2-pre2
RTLINUX_DIR = /usr/src/rtlinux-3.2-pre2/linux
INCLUDE= -I/usr/src/rtlinux-3.2-pre2/linux/include -I/usr/src/rtlinux-3.2-pre2/include -I/usr/src/rtlinux-3.2-pre2/include/compat
CFLAGS = -D__KERNEL__ -Wall -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -pipe -mpreferred-stack-boundary=2 -march=i686 -DMODULE -DMODVERSIONS -include /usr/src/linux-2.4.20/include/linux/modversions.h -g -D__RTL__ -D_LOOSE_KERNEL_NAMES -O2 -I/usr/src/rtlinux-3.2-pre2/linux/include -I/usr/src/rtlinux-3.2-pre2/include -I/usr/src/rtlinux-3.2-pre2/include/compat -I/usr/src/rtlinux-3.2-pre2/include/posix


robot :  fuzzy.o mbrship.o fuzzy_navigator.o set_speed.o video_device.o  ccvt.o  image_processing.o  detection_quilles.o  robot.c
	gcc robot.c -W -O2 -pthread -lm -o robot   fuzzy.o mbrship.o fuzzy_navigator.o  set_speed.o video_device.o  ccvt.o  image_processing.o detection_quilles.o

fuzzy.o : fuzzy.c fuzzy.h
	gcc -c fuzzy.c -Wall -O2

mbrship.o :  mbrship.c  fuzzy.h
	gcc -c mbrship.c -Wall -O2

fuzzy_navigator.o :  fuzzy_navigator.c   mbrship.h fuzzy.h controller.h
	gcc -c fuzzy_navigator.c -Wall -O2

set_speed.o : set_speed.c
	gcc  ${INCLUDE} -c set_speed.c -Wall -O2

image_processing.o: image_processing.h image_processing.c
	gcc -c image_processing.c -Wall -O2

ccvt.o: ccvt.h ccvt.S
	gcc -c ccvt.S -Wall -O2

video_device.o:	video_device.h video_device.c
	gcc -c video_device.c -Wall -O2

detection_quilles.o: detection_quilles.h detection_quilles.c
	gcc -c detection_quilles.c -Wall -O2

