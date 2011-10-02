CC = iccavr
CFLAGS =  -IC:\icc\include\ -e -DATMEGA  -l -g -Mavr_enhanced 
ASFLAGS = $(CFLAGS)  -Wa-g
LFLAGS =  -LC:\icc\lib\ -g -ucrtatmega.o -bfunc_lit:0x48.0x4000 -dram_end:0x45f -bdata:0x60.0x45f -dhwstk_size:16 -beeprom:1.512 -fihx_coff -S2
FILES = Main.o 

my_robot_icc:	$(FILES)
	$(CC) -o my_robot_icc $(LFLAGS) @my_robot_icc.lk  -lcatmega
Main.o: C:/icc/include/iom163v.h C:/icc/include/macros.h C:/icc/include/signal.h C:/icc/include/stdlib.h C:/icc/include/_const.h C:/icc/include/limits.h C:/icc/include/math.h
Main.o:	D:\robotique\programs_ICCAVR\capteurs\Main.c
	$(CC) -c $(CFLAGS) D:\robotique\programs_ICCAVR\capteurs\Main.c
