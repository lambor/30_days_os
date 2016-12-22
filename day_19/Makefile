subdir=kernel
all: os.img
bootloader: Makefile ipl.nas
	nasm -o bootloader ipl.nas
os.img: bootloader
	dd if=bootloader of=os.img count=1 bs=512
	dd if=/dev/zero of=os.img bs=512 seek=1 skip=1 count=2879
.PHONY:kernel
kernel: $(subdir)/Makefile
	cd $(subdir); make;
mountos: os.img kernel
	mkdir -p /tmp/os_dir
	mount -o loop os.img /tmp/os_dir -o fat=12
	sleep 1
	cp $(subdir)/kernel /tmp/os_dir
	cp ipl.nas /tmp/os_dir
	cp Makefile /tmp/os_dir
	cp 1111.txt /tmp/os_dir
	sleep 1
	umount /tmp/os_dir
run: mountos
	qemu-system-i386 -fda os.img 
debug: mountos
	qemu-system-i386 -fda os.img -s -S
clean:
	rm -f bootloader os.img os.sys
	@cd $(subdir);make clean
