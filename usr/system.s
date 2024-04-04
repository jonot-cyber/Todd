	.globl _start
	.globl __sys0
	.globl __sys1
	.globl __sys2
	.extern main

__sys0:
	pushl %ebp
	movl %esp, %ebp
	movl 8(%esp), %eax
	int $3
	movl %ebp, %esp
	popl %ebp
	ret

__sys1:
	pushl %ebp
	movl %esp, %ebp
	movl 8(%esp), %eax
	movl 12(%esp), %ebx
	int $3
	movl %ebp, %esp
	popl %ebp
	ret

__sys2:
	pushl %ebp
	movl %esp, %ebp
	movl 8(%esp), %eax
	movl 12(%esp), %ebx
	movl 16(%esp), %ecx
	int $3
	movl %ebp, %esp
	popl %ebp
	ret

_start:
	pushl %ebp
	movl %esp, %ebp
	call main
	movl %ebp, %esp
	popl %ebp
	ret
