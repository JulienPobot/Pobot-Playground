all: speed_ctrl.o  set_speed

include ../../rtl.mk

set_speed: set_speed.c
	$(CC) ${USER_CFLAGS} ${INCLUDE} -Wall -O0 -o set_speed set_speed.c
	
clean:
	rm -f *.o set_speed

include $(RTL_DIR)/Rules.make
