	.extern main
	.globl _start
	.globl __sys0
	.globl __sys1
	.globl __sys2

__tot_end:
	movl %ebp, %esp
	popl %ebp
	ret

__sys_end:
	movl 8(%esp), %eax
	int $3
	jmp __tot_end
	
__sys0:
	pushl %ebp
	movl %esp, %ebp
	jmp __sys_end

__sys1:
	pushl %ebp
	movl %esp, %ebp
	movl 12(%esp), %ebx
	jmp __sys_end

__sys2:
	pushl %ebp
	movl %esp, %ebp
	movl 12(%esp), %ebx
	movl 16(%esp), %ecx
	jmp __sys_end

_start:
	pushl %ebp
	movl %esp, %ebp
	call main
	jmp __tot_end
