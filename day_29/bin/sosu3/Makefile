.SUFFIXES:					#write empty rule to erase the built-in rules

.PHONY: all clean copy
DATA_SEG_SIZE = 64*1024 	#64KB
DATA_SEG_RESV = 0
STACK_ESP0 = 0x4000			#16KB
DEC_STACK_ESP0 = $(shell printf "%d" $(STACK_ESP0))
HEAP_BEGIN = 0
HEADER_SIZE = 64			#64	bytes

CFLAGS := $(CFLAGS) -O0 -fno-builtin -MD -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g -c

CSRC=$(wildcard *.c)
NASSRC=$(wildcard *.nas)

%.bin: %.header %.body
	cat $^ > $@

%.body: entry.o %.c.o 
	gcc -P -E -DSTACK_SIZE=$(STACK_ESP0) -DHEADER_SIZE=$(HEADER_SIZE) ../bin_ld.c -o $@.ld
	#incase section overlap
	ld -T $@.ld -o $@.o $^ ../dietlibc.a ../liblib.a -L/usr/lib/gcc/x86_64-linux-gnu/5/32 -lgcc --no-check-sections
	objcopy -S -O binary $@.o $@.all
	dd if=$@.all of=$@.data bs=1 skip=$$(($(DEC_STACK_ESP0)-$(HEADER_SIZE)))
	objcopy -S -O binary -j .text $@.o $@.text
	cat $@.text $@.data > $@

%.c.o: $(CSRC)
	gcc -nostdinc $(CFLAGS) -o $@ $^

%.nas.o: $(NASSRC)
	nasm -f elf $^ -o $@

entry.o: ../entry.nas
	nasm -f elf $< -o $@

%.header: ../header.nas %.body
	nasm -DDATA_SEG_SIZE=$(DATA_SEG_SIZE) \
		 -DDATA_SEG_RESV=$(DATA_SEG_RESV) \
		 -DSTACK_ESP0=$(STACK_ESP0) \
		 -DDATA_SEC_SIZE=$(shell wc -c < $*.body.data) \
		 -DCODE_SEC_SIZE=$(shell wc -c < $*.body.text) \
		 -DHEAP_BEGIN=$$(($(STACK_ESP0) + $(shell wc -c < $*.body.data))) \
		 -DHEADER_SIZE=$(HEADER_SIZE) \
		 -o $@ $<
copy:
	cp *.bin ../ 

clean:
	rm -f *.bin *.o *.d *.header *.ld *.data *.text *.all *.body
