#ifndef SYSCALL_H
#define SYSCALL_H

/* Might as well put all the syscall names here */
enum Syscalls {
	SYSCALL_WRITE = 0,
	SYSCALL_READ = 1,
	SYSCALL_MALLOC = 2,
	SYSCALL_FREE = 3,
	SYSCALL_WRITEHEX = 4,
	SYSCALL_FOPEN = 5,
	SYSCALL_EXEC = 6,
	SYSCALL_SET_FG = 7,
	SYSCALL_SET_BG = 8,
	SYSCALL_RAND = 9,
};

void syscall_init();

#endif
