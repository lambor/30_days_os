CFLAGS := $(CFLAGS) -O1 -fno-builtin -MD -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g -c

all: hello.bin

%.bin: %.header %.body
	cat $^ > $@

%.body: entry.nas.o %.o api.nas.o
	gcc -P -E -DSTACK_SIZE=$(STACK_ESP0) bin_ld.c -o $@.ld
	ld -T $@.ld -o $@.o $^
	@rm $@.ld
	objcopy -S -O binary $@.o $@.all
	dd if=$@.all of=$@.data bs=1 skip=$(DEC_STACK_ESP0)
	objcopy -S -O binary -j .text $@.o $@.text
	cat $@.text $@.data > $@

%.o: %.c
	gcc -nostdinc $(CFLAGS) -o $@ $<
%.nas.o: %.nas
	nasm -f elf $< -o $@


DATA_SEG_SIZE = 64*1024 	#64KB
DATA_SEG_RESV = 0
STACK_ESP0 = 0x400			#1KB
DEC_STACK_ESP0 = $(shell printf "%d" $(STACK_ESP0))
HEAP_BEGIN = 0

%.header: header.nas %.body
	nasm -DDATA_SEG_SIZE=$(DATA_SEG_SIZE) \
		 -DDATA_SEG_RESV=$(DATA_SEG_RESV) \
		 -DSTACK_ESP0=$(STACK_ESP0) \
		 -DDATA_SEC_SIZE=$(wc -c < $>.data) \
		 -DCODE_SEC_SIZE=$(wc -c < $>.text) \
		 -DHEAP_BEGIN=$(HEAP_BEGIN) \
		 -o $@ $<

clean:
	rm -f *.bin *.o *.d *.header *.ld *.data *.text *.all *.body
