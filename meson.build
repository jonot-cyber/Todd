project('Todd', 'cpp')

sources = [
	'src/boot.s',
	'src/common.cpp',
	'src/gdt.s',
	'src/gdt.cpp',
	'src/heap.cpp',
	'src/idt.cpp',
	'src/io.cpp',
	'src/keyboard.cpp',
	'src/main.cpp',
	'src/memory.cpp',
	'src/monitor.cpp',
	'src/ordered_array.cpp',
	'src/ps2.cpp',
	'src/isr.cpp',
	'src/timer.cpp',
]

kernel = executable('Kernel', sources, link_args: ['-T../src/link.ld', '-m32', '-nostdlib'], cpp_args: ['-m32', '-nostdinc', '-fno-builtin'])

qemu = find_program('qemu-system-i386')

test('Run under qemu', qemu,  args: ['-kernel', kernel], timeout: -1)

