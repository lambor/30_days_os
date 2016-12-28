DATA_SEG_SIZE = 64*1024 	#64KB
DATA_SEG_RESV = 0
STACK_ESP0 = 0x400			#1KB
DEC_STACK_ESP0 = $(shell printf "%d" $(STACK_ESP0))
HEAP_BEGIN = 0
HEADER_SIZE = 64			#64	bytes

CFLAGS := $(CFLAGS) -O1 -fno-builtin -MD -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g -c

%.bin: %.header %.body
	cat $^ > $@

%.body: entry.nas.o %.o %.nas.o api.nas.o
	gcc -P -E -DSTACK_SIZE=$(STACK_ESP0) -DHEADER_SIZE=$(HEADER_SIZE) ../bin_ld.c -o $@.ld
	ld -T $@.ld -o $@.o $^
	objcopy -S -O binary $@.o $@.all
	dd if=$@.all of=$@.data bs=1 skip=$$(($(DEC_STACK_ESP0)-$(HEADER_SIZE)))
	objcopy -S -O binary -j .text $@.o $@.text
	cat $@.text $@.data > $@

entry.nas.o: ../entry.nas
	nasm -f elf $< -o $@

api.nas.o: ../api.nas
	nasm -f elf $< -o $@

CFILES=$(wildcard *.c)
NASFILES=$(wildcard *.nas)

%.o: $(CFILES)
	gcc -nostdinc $(CFLAGS) -o $@ $^
%.nas.o: $(NASFILES)
	nasm -f elf $^ -o $@

%.header: ../header.nas %.body
	nasm -DDATA_SEG_SIZE=$(DATA_SEG_SIZE) \
		 -DDATA_SEG_RESV=$(DATA_SEG_RESV) \
		 -DSTACK_ESP0=$(STACK_ESP0) \
		 -DDATA_SEC_SIZE=$(shell wc -c < $*.body.data) \
		 -DCODE_SEC_SIZE=$(shell wc -c < $*.body.text) \
		 -DHEAP_BEGIN=$(HEAP_BEGIN) \
		 -DHEADER_SIZE=$(HEADER_SIZE) \
		 -o $@ $<

clean:
	rm -f *.bin *.o *.d *.header *.ld *.data *.text *.all *.body
