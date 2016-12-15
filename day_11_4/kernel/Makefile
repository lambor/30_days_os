OBJDIR=.

CFLAGS := $(CFLAGS) -O1 -fno-builtin -I$(OBJDIR) -MD -fno-stack-protector                                                                                      
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g

LDFLAGS=-m elf_i386

all:kernel

kernel:entry.out main.out
	#ld $(LDFLAGS) -N -e start -Ttext $(addr) -o $@.out $^
	#objcopy -S -O binary -j .text $@.out $@
	#because the table_rgb is static, not locate in text section
	#objcopy -S -O binary $@.out $@
	cp entry.out _kernel
	cat main.out >> _kernel
	mv _kernel kernel

main.o:main.c
	gcc -nostdinc $(CFLAGS) -Os -c -o $@ $<

main.out: main.o io.nas.o graphics.o font.o dsctbl.o int.o int.nas.o \
		string.o fifo.o mouse_keyboard.o memory.o sheet.o test.o
	ld $(LDFLAGS) -N -e bootmain -Ttext 0x280000 -o $@.o $^
	objcopy -S -O binary $@.o $@
	objdump -d main.out.o > main.out.txt

entry.out: entry.gas.o
	ld $(LDFLAGS) -N -e start -Ttext 0xc400 -o $@.o $^
	objcopy -S -O binary -j .text $@.o $@

memory.o: memory.c
	gcc -nostdinc $(CFLAGS) -O0 -c -o $@ $<

%.o: %.c
	gcc -nostdinc $(CFLAGS) -c -o $@ $<

%.nas.o: %.nas
	nasm -f elf $< -o $@

%.gas.o: %.S
	gcc -nostdinc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o kernel *.out _kernel *.d
