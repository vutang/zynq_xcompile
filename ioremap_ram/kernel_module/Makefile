export ARCH:=arm
export CROSS_COMPILE:=arm-xilinx-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc

obj-m += mem_ioremap.o

KDIR  := /home/vutt6/Workspace/zynq/zc706/zc706_petalnx_20154/build/linux/kernel/xlnx-4.0-mod/
PWD		:= $(shell pwd)

default:
	${MAKE} -C ${KDIR} M=${PWD} modules

clean:
	${MAKE} -C ${KDIR} M=${PWD} clean