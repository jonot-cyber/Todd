	.globl fork
	.globl switch_task
	.globl join
	.extern current_task
	.extern to_del

fork:
	/* Push some stuff over */
	pushfl
	cli
	pushl %ebp
	movl %esp, %ebp
	
	/* Copy the stack over */
	pushl %esp
	call move_stack

	/* Reset the stack */
	addl $4, %esp

	movl $0, %eax
	popl %ebp
	popfl
	ret

switch_task:
	pushfl
	cli
	pushl %ebp
	movl %esp, %ebp
	
	/* Save current stack */
	pushl %esp
	call save_stack
	addl $4, %esp

	/* Increment the task */
	call incr_task
	
	/* Load the stack to the stack pointer */
	call load_stack
	movl %eax, %esp

	/* Return 1 to show that we switched */
	movl $1, %eax
	popl %ebp
	popfl
	ret

join:
	pushfl
	cli
	pushl %ebp
	movl %esp, %ebp

	pushl %esp
	call save_stack
	addl $4, %esp

	movl current_task, %eax
	movl %eax, to_del

	call incr_task

	call load_stack
	movl %eax, %esp

	call delete_task
	movl $1, %eax
	popl %ebp
	popfl
	ret
