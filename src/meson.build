subdir('lisp')

sources = [
	'boot.s',
        'cmos.c',
	'common.c',
	'gdt.s',
	'gdt.c',
#       'hashmap.c',
	'heap.c',
	'idt.c',
	'io.c',
        'isr.c',
	'keyboard.c',
	'main.c',
	'memory.c',
	'monitor.c',
#	'ordered_array.c',
        'process.s',
	'ps2.c',
        'string.c',
#       'task.c',
	'timer.c',
]

kernel = executable('Kernel', sources, link_args: ['-T../src/link.ld', '-m32', '-nostdlib'], c_args: ['-m32', '-nostdinc', '-fno-builtin'], dependencies: lisp_dep)
