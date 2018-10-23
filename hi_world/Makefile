TOOL_CHAIN=/opt/PetaLinux/petalinux-v2015.4-final
CC=$(TOOL_CHAIN)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-gcc
AR=$(TOOL_CHAIN)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-ar
LD=$(TOOL_CHAIN)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-ld

all: 
	$(CC) -o hi.o hello_world.c
clean:
	rm *.o