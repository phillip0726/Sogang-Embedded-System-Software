#Makefile for a basic kernel module

obj-m   := stopwatch.o

CC := arm-none-linux-gnueabi-gcc
KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)


driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules 

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf .tmp*
	rm -rf *.order
	rm -rf *.symvers
