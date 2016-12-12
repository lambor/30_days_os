addr=0xc400
OBJDIR=.

CFLAGS := $(CFLAGS) -O1 -fno-builtin -I$(OBJDIR) -MD                                                                                        
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32


LDFLAGS=-m elf_i386

all:kernel

# kernel:entry.o main.o header.gas.o
kernel:entry.o main.o header.nasm.o header.c.o
	ld $(LDFLAGS) -N -e start -Ttext $(addr) -o $@.out $^
	#objdump -S $@.out >$@.asm
	#objcopy -S -O binary -j .text $@.out $@
	#because the table_rgb is static, not locate in text section
	objcopy -S -O binary $@.out $@

entry.o:entry.S
	gcc -nostdinc $(CFLAGS) -c -o $@ $<
main.o:main.c
	gcc -nostdinc $(CFLAGS) -Os -c -o $@ $<
screen.o:screen.c
	gcc -nostdinc $(CFLAGS) -c -o $@ $<
header.gas.o:header.S
	gcc -nostdinc $(CFLAGS) -c -o $@ $<
header.nasm.o:header.nas
	nasm -f elf $< -o $@
header.c.o:header.c
	gcc -nostdinc $(CFLAGS) -c -o $@ $<

#run:
	#qemu-system-i386 -drive file=kernel,if=floppy 

clean:
	@rm -f entry.o main.o screen.o header.gas.o header.nasm.o kernel.out kernel.asm kernel *.d
