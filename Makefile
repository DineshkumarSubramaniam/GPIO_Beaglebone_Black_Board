obj-m += gpio.o
 

KERN_DIR := /lib/modules/$(shell uname -r)/build/

all:
	make -C $(KERN_DIR) M=$(PWD) modules

 
clean:
	make -C $(KERN_DIR) M=$(PWD) clean
