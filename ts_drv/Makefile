obj-m += dwav.o

kernel=/usr/src/linux-headers-$(shell uname -r)

all:
	make -C $(kernel) M=$(PWD) modules

clean:
	rm -f *.ko *.o *.mod.c *.symvers *.order
