
obj-m := proc_read.o
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	@rm -rf *.o *.ko *.mod.c *.order *.symvers
