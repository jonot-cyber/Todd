subdir('lisp')

sources = [
	'boot.s',
        'cmos.cpp',
	'common.cpp',
	'gdt.s',
	'gdt.cpp',
        'hashmap.cpp',
	'heap.cpp',
	'idt.cpp',
	'io.cpp',
        'isr.cpp',
	'keyboard.cpp',
	'main.cpp',
	'memory.cpp',
	'monitor.cpp',
#	'ordered_array.cpp',
        'process.s',
	'ps2.cpp',
        'string.cpp',
#        'task.cpp',
	'timer.cpp',
]

kernel = executable('Kernel', sources, link_args: ['-T../src/link.ld', '-m32', '-nostdlib'], cpp_args: ['-m32', '-nostdinc', '-fno-builtin'], dependencies: lisp_dep)
