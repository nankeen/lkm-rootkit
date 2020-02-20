SRCDIR := src
obj-m += rootkit.o
rootkit-y += $(SRCDIR)/rootkit.o $(SRCDIR)/backdoor.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
