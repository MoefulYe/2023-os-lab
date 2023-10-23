NAME := print-family-info
KDIR := /usr/src/linux/
obj-m := $(NAME).o
$(NAME)-objs := main.o
PWD := $(shell pwd)

build:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean: 
	$(MAKE) -C $(KDIR) M=$(PWD) clean
insmod:
	sudo insmod $(NAME).ko
rmmod:
	sudo rmmod $(NAME)
