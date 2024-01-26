CFLAGS += -m32 -nostdinc -fno-builtin

SRCS = $(wildcard src/*.c src/lisp/*.c)
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

build/%.s.o: %.s
	@mkdir -p $(@D)
	$(CC) ${CFLAGS} -c -o $@ $^

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) ${CFLAGS} -Isrc/lisp -Isrc -c -o $@ $^

build/Kernel: build/src/boot.s.o build/src/gdt.s.o $(OBJS)
	$(LD) -melf_i386 -Tsrc/link.ld -nostdlib -o $@ $^

clean:
	rm -r build/*

test: build/Kernel
	qemu-system-i386 -kernel build/Kernel

echovars:
	echo $(OBJS)
