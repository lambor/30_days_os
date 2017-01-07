CFLAGS := $(CFLAGS) -O1 -fno-builtin -MD -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -Wall -Wno-format -Wno-unused -gstabs -m32
CFLAGS += -g -c


BIN_DIRS=hello\
		 crack1 crack2 crack3 crack4 crack5 crack7\
		 bug1 bug2 bug3\
		 winhelo winhelo2 lines walk noodle color color2\
		 hello2 sosu3 type1 type hellocn\
		 notrec bball invader

.PHONY:all,clean
all: liblib.a
	@for dir in $(BIN_DIRS); do \
		$(MAKE) -C $$dir $$dir.bin \
		&& $(MAKE) -C $$dir copy;\
	done

liblib.a: api.nas.o string.c.o util.c.o
	ar rcs $@ $^

%.c.o: %.c
	gcc -nostdinc $(CFLAGS) -O0 -o $@ $<

%.nas.o: %.nas
	nasm -f elf $< -o $@

clean:
	@for dir in $(BIN_DIRS); do \
		$(MAKE) -C $$dir clean;\
	done
	rm -f *.bin *.a *.o *.d
