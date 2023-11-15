SOURCES=build/boot.o build/io.o build/vga.o build/monitor.o build/main.o 

CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin

clean:
	rm build/*

run: build/Kernel
	qemu-system-i386 -kernel build/Kernel

build/Kernel: $(SOURCES) src/link.ld
	ld -melf_i386 -Tsrc/link.ld -o $@ $(SOURCES)

build/main.o: src/main.cpp src/monitor.cpp
	g++ -c $(CFLAGS) $< -o $@

build/monitor.o: src/monitor.cpp src/io.cpp
	g++ -c $(CFLAGS) $< -o $@

build/io.o: src/io.cpp
	g++ -c $(CFLAGS) $< -o $@

build/boot.o: src/boot.s
	gcc -m32 -fPIE -c src/boot.s -o build/boot.o

build/vga.o: src/vga.cpp
	g++ -c $(CFLAGS) $< -o $@
