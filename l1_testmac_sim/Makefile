TOOL_CHAIN=/opt/PetaLinux/petalinux-v2015.4-final
CC=$(TOOL_CHAIN)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-gcc
AR=$(TOOL_CHAIN)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-ar
LD=$(TOOL_CHAIN)/tools/linux-i386/arm-xilinx-linux-gnueabi/bin/arm-xilinx-linux-gnueabi-ld

APP = l1_testmac_sim
LDLIBS= 
APP_OBJS = l1_testmac_sim.o logger.o

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(APP_OBJS) $(LDLIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	-rm -f $(APP) *.elf *.gdb *.o