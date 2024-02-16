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
	
	/* Load in the size and size */
	movl initial_esps, %eax
	movl i_task, %ecx
	movl (%eax, %ecx, 4), %eax
	subl %esp, %eax
	movl %esp, %ecx
	pushl %eax
	pushl %ecx

	/* Copy the stack over */
	call move_stack

	/* Reset the stack */
	addl $8, %esp

	/* Save our stack in the array, and create a new process.*/
	movl $stacks, %ecx
	movl c_tasks, %edx
	movl %eax, (%ecx, %edx, 4)
	incl c_tasks

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
