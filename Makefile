SRCDIR := src
BUILDROOT := ~/Projects/Coding/buildroot-2019.11.1
#KDIR := /lib/modules/$(shell uname -r)
KDIR := $(BUILDROOT)/output/build/linux-headers-5.3.18/
ROOTFS := $(BUILDROOT)/output/target/root/

obj-m += rootkit.o
rootkit-y += $(SRCDIR)/rootkit.o $(SRCDIR)/backdoor.o $(SRCDIR)/cmd.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

debug:
	$(MAKE) -C $(KDIR) M=$(PWD) modules EXTRA_CFLAG=" -g -DDEBUG"

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

rootfs: debug
	cp rootkit.ko $(ROOTFS)
	$(MAKE) -C $(BUILDROOT) -j4

run: rootfs
	qemu-system-x86_64 \
		-kernel $(BUILDROOT)/output/images/bzImage \
		-hda $(BUILDROOT)/output/images/rootfs.ext2 \
		-append "root=/dev/sda rw" \
		-net nic,model=virtio \
		-net user,hostfwd=tcp::1337-:1337
