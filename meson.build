project('Todd', 'cpp')

subdir('src')

qemu = find_program('qemu-system-i386')

test('Run under qemu', qemu,  args: ['-kernel', kernel], timeout: -1)
