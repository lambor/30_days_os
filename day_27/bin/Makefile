CFLAGS := $(CFLAGS) -O1 -fno-builtin -MD -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g -c


BIN_DIRS=hello\
		 crack1 crack2 crack3 crack4 crack5 crack7\
		 bug1 bug2 bug3\
		 winhelo winhelo2 lines walk noodle color color2\
		 hello2

.PHONY:all,clean
all: liblib.a
	@for dir in $(BIN_DIRS); do \
		$(MAKE) -C $$dir $$dir.bin \
		&& $(MAKE) -C $$dir copy;\
	done

liblib.a: api.nas.o string.c.o
	ar rcs $@ $^

%.c.o: %.c
	gcc -nostdinc $(CFLAGS) -o $@ $<

%.nas.o: %.nas
	nasm -f elf $< -o $@

clean:
	@for dir in $(BIN_DIRS); do \
		$(MAKE) -C $$dir clean;\
	done
	rm -f *.bin *.a *.o
