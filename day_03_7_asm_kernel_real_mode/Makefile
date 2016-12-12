all: os.img
bootloader: Makefile ipl.nas
	nasm -o bootloader ipl.nas
os.img: bootloader
	dd if=bootloader of=os.img count=1 bs=512
	dd if=/dev/zero of=os.img bs=512 seek=1 skip=1 count=2879
os.sys: os.nas
	nasm -o os.sys os.nas
mountos: os.img os.sys
	mkdir -p /tmp/os_dir
	mount -o loop os.img /tmp/os_dir -o fat=12
	sleep 1
	cp os.sys /tmp/os_dir
	sleep 1
	umount /tmp/os_dir
run: mountos
	qemu-system-i386 -fda os.img
clean:
	rm -f bootloader os.img os.sys
