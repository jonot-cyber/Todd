CFLAGS += -m32 -nostdinc -fno-builtin -g -fno-strict-aliasing -Wall -Wextra -Werror
ASFLAGS += -m32 -g -Wall -Wextra -Werror

USR_CFLAGS = ${CFLAGS} -fPIE -fPIC -ffreestanding
USR_ASFLAGS = ${ASFLAGS} -fPIE -fPIC -ffreestanding

SRCS = $(wildcard src/*.c src/lisp/*.c)
OBJS = $(patsubst %.c,build/%.o,$(SRCS))

all: build/initrd.tar build/Kernel

# Let's build some things for the ramdisk
build/usr.lib.system.s.o: usr/lib/system.s
	$(CC) ${USR_ASFLAGS} -c -o $@ $^

build/usr.lib.system.o: usr/lib/system.c
	$(CC) ${USR_CFLAGS} -Iusr/include -c -o $@ $^

build/usr.lib.stdio.o: usr/lib/stdio.c
	$(CC) ${USR_CFLAGS} -Iusr/include -c -o $@ $^

build/usr.todd.o: usr/todd.c
	$(CC) ${USR_CFLAGS} -Iusr/include -c -o $@ $^

build/usr.printf_test.o: usr/printf_test.c
	$(CC) ${USR_CFLAGS} -Iusr/include -c -o $@ $^

initrd/todd.elf: usr/include/system.h usr/link.ld build/usr.lib.system.o build/usr.lib.system.s.o build/usr.todd.o
	$(CC) ${USR_CFLAGS} -nostdlib -Tusr/link.ld -o $@ build/usr.todd.o build/usr.lib.system.s.o build/usr.lib.system.o

initrd/test.elf: usr/link.ld build/usr.lib.system.o build/usr.lib.system.s.o build/usr.printf_test.o build/usr.lib.stdio.o
	$(CC) ${USR_CFLAGS} -nostdlib -Tusr/link.ld -o $@ build/usr.printf_test.o build/usr.lib.system.s.o build/usr.lib.system.o build/usr.lib.stdio.o

build/%.s.o: %.s
	@mkdir -p $(@D)
	$(CC) ${ASFLAGS} -c -o $@ $^

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) ${CFLAGS} -Isrc -c -o $@ $^

build/Kernel: build/src/boot.s.o build/src/gdt.s.o build/src/task.s.o $(OBJS)
	$(LD) ${LDFLAGS} -melf_i386 -Tsrc/link.ld -nostdlib -o $@ $^

build/initrd.tar: initrd/todd.elf initrd/test.elf
	tar -c $^ > $@

clean:
	rm -rf build/*
	rm -rf initrd/*

test: build/Kernel build/initrd.tar
	qemu-system-i386 -kernel build/Kernel -initrd build/initrd.tar # -no-reboot -no-shutdown

# Set QEMU to be debuggable
debug: build/Kernel build/initrd.tar
	qemu-system-i386 -s -S -kernel build/Kernel -initrd build/initrd.tar # -no-reboot -no-shutdown
