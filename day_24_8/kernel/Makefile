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
	objdump -dS main.out.o > main.out.txt
	objdump -d entry.out.o > entry.out.txt

main.o:main.c
	gcc -nostdinc $(CFLAGS) -Os -c -o $@ $<

main.out: main.o io.nas.o graphics.o font.o dsctbl.o int.o int.nas.o \
		string.o fifo.o mouse.o keyboard.o memory.o map_sheet.o timer.o\
		task.nas.o task.o task_console.o file.o console.o  console.nas.o\
	   	sys_api.o api.nas.o common.o\
		test.o
	ld $(LDFLAGS) -N -e bootmain -Ttext 0x280000 -o $@.o $^
	objcopy -S -O binary $@.o $@

entry.out: entry.gas.o
	ld $(LDFLAGS) -N -e start -Ttext 0xc400 -o $@.o $^
	objcopy -S -O binary -j .text $@.o $@

memory.o: memory.c	#not needed by book.
	gcc -nostdinc $(CFLAGS) -O0 -c -o $@ $<

file.o: file.c	#otherwise file cannot be found.
	gcc -nostdinc $(CFLAGS) -O0 -c -o $@ $<

sys_api.o: sys_api.c #otherwise the api cannot return value by eax
	gcc -nostdinc $(CFLAGS) -O0 -c -o $@ $<

api.nas.o: ../bin/api.nas
	nasm -f elf $< -o $@

#graphics.o: graphics.c
#	gcc -nostdinc $(CFLAGS) -O0 -c -o $@ $<

%.o: %.c
	gcc -nostdinc $(CFLAGS) -c -o $@ $<

%.nas.o: %.nas
	nasm -f elf $< -o $@

%.gas.o: %.S
	gcc -nostdinc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o kernel *.out _kernel *.d
