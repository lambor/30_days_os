CFLAGS := $(CFLAGS) -O1 -fno-builtin -MD -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g

LDFLAGS=-m elf_i386

all: hello.bin

%.bin: entry.nas.o %.o api.nas.o
	ld $(LDFLAGS) -N -e bin_entry -Ttext 0 -o $@.o $^
	objcopy -S -O binary $@.o $@

%.o: %.c
	gcc -nostdinc $(CFLAGS) -c -o $@ $<
%.nas.o: %.nas
	nasm -f elf $< -o $@

clean:
	rm -f *.bin *.o *.d
