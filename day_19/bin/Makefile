all: hlt.bin

%.bin: %.nas
	nasm -o $@ $<

clean:
	rm -f *.bin
