obj-m := nothing.o

KDIR := /home/lz/linux-2.6.28.10
PWD := $(shell pwd)

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=/home/lz/olimex/gcc-4.3.3/usr/bin/arm-linux-uclibcgnueabi- -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=/home/lz/olimex/gcc-4.3.3/usr/bin/arm-linux-uclibcgnueabi- clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=/home/lz/olimex/gcc-4.3.3/usr/bin/arm-linux-uclibcgnueabi- modules_install
