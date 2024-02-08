	.extern ret_ptr
	.extern jump_to
	.global fork
	.global call_cc
	
read_eip:
	popl %eax
	jmp %eax

fork:
	popl %eax
	movl %eax, ret_ptr
	call read_eip
	cmp $0x12345, %eax
	je .forken
	movl %eax, jump_to
	movl ret_ptr, %eax
	jmp *%eax
.forken:
	movl $0, %eax
	ret

call_cc:
	movl ret_ptr, %eax
	movl %eax, (%esp)
	movl $0x12345, %eax
	movl jump_to, %ebx
	jmp *%ebx
