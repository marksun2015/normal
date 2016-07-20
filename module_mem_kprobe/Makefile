PWD=$(shell pwd)
KERNEL_SRC=../kernel
CROSS_COMPILE=arm-none-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

obj-m :=mmap_test.o
PROJECT_TIME=`date "+%Y%m%d%H%M"`

module-objs :=mmap_test.o

all:make_ct_build make_module

make_module:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=arm modules
	$(CC) -o test_malloc test_malloc.c

clean:
	$(RM) *.ko
	$(RM) *.o
	$(RM) *.mod.c
	$(RM) Module*
	$(RM) modules*
	$(RM) test_malloc

make_ct_build:
	@echo "#define BUILE_DATE \""$(PROJECT_TIME)"\"" 
