BIN_DIRS=hello\
		 crack1 crack2 crack3 crack4 crack5 crack7\
		 bug1 bug2 bug3\
		 winhelo winhelo2 lines walk noodle color color2

.PHONY:all,clean,lib
all:
	@for dir in $(BIN_DIRS); do \
		$(MAKE) -C $$dir $$dir.bin \
		&& $(MAKE) -C $$dir copy;\
	done

lib:
	


clean:
	@for dir in $(BIN_DIRS); do \
		$(MAKE) -C $$dir clean;\
	done
	rm -f *.bin *.a
