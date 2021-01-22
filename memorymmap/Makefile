#GPL-2.0
#Makefile for zcopy-rsockets
KERNELDIR:=/lib/modules/4.19.0/build/
PWD:=$(shell pwd)
EXTRA_CFLAGS= -O0

obj-m := kmmap.o
kmmap-y := k-mmap.o

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean