NAME := print-all-kernel-threads
KDIR := /usr/src/linux/
obj-m := $(NAME).o
$(NAME)-objs := main.o
PWD := $(shell pwd)

build:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean: 
	$(MAKE) -C $(KDIR) M=$(PWD) clean
insmod:
	sudo modprobe ./$(NAME).ko
rmmod:
	sudo modprobe -R ./$(NAME).ko
dmesg:
	sudo dmesg | rg "print-kernel-threads-info"

