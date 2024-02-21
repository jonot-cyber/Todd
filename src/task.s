	.extern initial_esps
	.extern move_stack
	.extern stacks
	.extern c_tasks
	.extern i_task
	.globl fork
	.globl switch_task

fork:
	/* Push some stuff over */
	pushfl
	cli
	pushl %ebp
	
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
	/* Save current stack */
	movl $stacks, %ecx
	movl i_task, %edx
	movl %esp, (%ecx, %edx, 4)

	/* Increment the task, rounded */
	movl i_task, %eax
	incl %eax
	movl $0, %edx
	divl c_tasks
	movl %edx, i_task
	
	/* Load the stack from the array to the stack pointer */
	movl $stacks, %eax
	movl i_task, %ecx
	movl (%eax,%ecx,4), %esp

	/* Return 1 to show that we switched */
	movl $1, %eax
	popl %ebp
	popfl
	ret
