SOURCES=build/boot.o build/main.o

CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin

clean:
	rm build/*

run: build/Kernel
	qemu-system-i386 -display none -monitor stdio -kernel build/Kernel

build/Kernel: build/main.o build/boot.o src/link.ld
	ld -melf_i386 -Tsrc/link.ld -o $@ build/main.o build/boot.o

build/%.o: src/%.cpp
	g++ -c $(CFLAGS) $^ -o $@

build/boot.o: src/boot.s
	gcc -m32 -fPIE -c src/boot.s -o build/boot.o