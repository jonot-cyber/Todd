project('Todd', 'cpp')

bootloader = declare_dependency(sources: 'src/boot.s', compile_args: ['-fPIE', '-m32'])

sources = [
	'src/boot.s',
	'src/main.cpp',
	'src/io.cpp',
	'src/monitor.cpp',
]

kernel = executable('Kernel', sources, link_args: ['-T../src/link.ld', '-m32', '-nostdlib'], cpp_args: ['-m32', '-nostdinc', '-fno-builtin'])

qemu = find_program('qemu-system-i386')

test('Run under qemu', qemu,  args: ['-kernel', kernel], timeout: -1)

