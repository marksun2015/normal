PWD=$(shell pwd)
KERNEL_SRC=../kernel.qt
CROSS_COMPILE=arm-none-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

obj-m :=testmodule.o
PROJECT_TIME=`date "+%Y%m%d%H%M"`

module-objs :=testmodule.o

all:make_ct_build make_module

make_module:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=arm modules
clean:
	$(RM) *.ko
	$(RM) *.o
	$(RM) *.mod.c
	$(RM) Module*
	$(RM) modules*

make_ct_build:
	@echo "#define BUILE_DATE \""$(PROJECT_TIME)"\"" 
