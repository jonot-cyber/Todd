CFLAGS += -m32 -nostdinc -fno-builtin -g -fno-strict-aliasing -Wall -Wextra -Werror
ASFLAGS += -m32 -g -Wall -Wextra -Werror

SRCS = $(wildcard src/*.c src/lisp/*.c)
INITRD_SRCS = $(wildcard initrd/*)
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

build/%.s.o: %.s
	@mkdir -p $(@D)
	$(CC) ${ASFLAGS} -c -o $@ $^

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) ${CFLAGS} -Isrc -c -o $@ $^

build/Kernel: build/src/boot.s.o build/src/gdt.s.o build/src/task.s.o $(OBJS)
	$(LD) ${LDFLAGS} -melf_i386 -Tsrc/link.ld -nostdlib -o $@ $^

build/initrd.tar: $(INITRD_SRCS)
	tar -c $^ > $@

clean:
	rm -r build/*

test: build/Kernel build/initrd.tar
	qemu-system-i386 -kernel build/Kernel -initrd build/initrd.tar # -no-reboot -no-shutdown

# Set QEMU to be debuggable
debug: build/Kernel build/initrd.tar
	qemu-system-i386 -s -S -kernel build/Kernel -initrd build/initrd.tar # -no-reboot -no-shutdown
