.globl GDTFlush
.globl IDTFlush
.globl isr0
.globl isr1
.globl isr2
.globl isr3
.globl isr4
.globl isr5
.globl isr6
.globl isr7
.globl isr8
.globl isr9
.globl isr10
.globl isr11
.globl isr12
.globl isr13
.globl isr14
.globl isr15
.globl isr16
.globl isr17
.globl isr18
.globl isr19
.globl isr20
.globl isr21
.globl isr22
.globl isr23
.globl isr24
.globl isr25
.globl isr26
.globl isr27
.globl isr28
.globl isr29
.globl isr30
.globl isr31
.globl irq0
.globl irq1
.globl irq2
.globl irq3
.globl irq4
.globl irq5
.globl irq6
.globl irq7
.globl irq8
.globl irq9
.globl irq10
.globl irq11
.globl irq12
.globl irq13
.globl irq14
.globl irq15

.extern isr_handler
.extern irq_handler

GDTFlush:
	mov 4(%esp), %eax
	lgdtl (%eax)
	mov $0x10, %ax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs
	mov %eax, %ss
	ljmp $0x8,$.flush
.flush:
	ret

IDTFlush:
	mov 4(%esp), %eax
	lidtl (%eax)
	ret

.macro isr_noarg n
isr\n:
	cli
	pushl $0x0
	pushl $\n
	jmp isr_common_stub
	.endm

.macro isr_arg n
isr\n:
	cli
	push $\n
	jmp isr_common_stub
.endm

isr_noarg 0
isr_noarg 1
isr_noarg 2
isr_noarg 3
isr_noarg 4
isr_noarg 5
isr_noarg 6
isr_noarg 7
isr_arg 8
isr_noarg 9
isr_arg 10
isr_arg 11
isr_arg 12
isr_arg 13
isr_arg 14
isr_noarg 15
isr_noarg 16
isr_noarg 17
isr_noarg 18
isr_noarg 19
isr_noarg 20
isr_noarg 21
isr_noarg 22
isr_noarg 23
isr_noarg 24
isr_noarg 25
isr_noarg 26
isr_noarg 27
isr_noarg 28
isr_noarg 29
isr_noarg 30
isr_noarg 31

.macro irq n
irq\n:
	cli
	push $0
	push $(\n+32)
	jmp irq_common_stub
.endm

irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15


isr_common_stub:
	pusha
	mov %ds,%ax
	push %eax
	mov $0x10,%ax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs
	call isr_handler
	pop %eax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs
	popa
	add $0x8, %esp
	sti
	iret

irq_common_stub:
	pusha
	mov %ds, %ax
	push %eax
	mov $0x10, %ax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs

	call irq_handler

	pop %ebx
	mov %ebx, %ds
	mov %ebx, %es
	mov %ebx, %fs
	mov %ebx, %gs

	popa
	add $0x8, %esp
	sti
	iret
