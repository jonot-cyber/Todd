CFLAGS += -m32 -nostdinc -fno-builtin -g

SRCS = $(wildcard src/*.c src/lisp/*.c)
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

build/%.s.o: %.s
	@mkdir -p $(@D)
	$(CC) ${CFLAGS} -c -o $@ $^

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) ${CFLAGS} -Isrc/lisp -Isrc -c -o $@ $^

build/Kernel: build/src/boot.s.o build/src/gdt.s.o build/src/task.s.o $(OBJS)
	$(LD) ${LDFLAGS} -melf_i386 -Tsrc/link.ld -nostdlib -o $@ $^

clean:
	rm -r build/*

test: build/Kernel
	qemu-system-i386 -kernel build/Kernel

# Set QEMU to be debuggable
debug: build/Kernel
	qemu-system-i386 -s -S -kernel build/Kernel
