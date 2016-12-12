all: Makefile ipl.nas
	nasm -o helloos.img ipl.nas
run: all
	qemu-system-i386 -fda helloos.img
