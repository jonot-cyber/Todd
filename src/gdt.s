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

.extern isrHandler
.extern irqHandler

.jail:
    jmp .jail

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

isr0:
    cli
    pushl $0x0
    pushl $0x0
    jmp isr_common_stub

isr1:
    cli
    push $0x0
    push $0x1
    jmp isr_common_stub

isr2:
    cli
    push $0x0
    push $0x2
    jmp isr_common_stub

isr3:
    cli
    push $0x0
    push $0x3
    jmp isr_common_stub

isr4:
    cli
    push $0x0
    push $0x4
    jmp isr_common_stub

isr5:
    cli
    push $0x0
    push $0x5
    jmp isr_common_stub

isr6:
    cli
    push $0x0
    push $0x6
    jmp isr_common_stub

isr7:
    cli
    push $0x0
    push $0x7
    jmp isr_common_stub

isr8:
    cli
    push $0x8
    jmp isr_common_stub

isr9:
    cli
    push 0
    push $0x9
    jmp isr_common_stub

isr10:
    cli
    push $0xA
    jmp isr_common_stub

isr11:
    cli
    push $0xB
    jmp isr_common_stub

isr12:
    cli
    push $0xC
    jmp isr_common_stub

isr13:
    cli
    push $0xD
    jmp isr_common_stub

isr14:
    cli
    push $0xE
    jmp isr_common_stub

isr15:
    cli
    push $0x0
    push $0xF
    jmp isr_common_stub

isr16:
    cli
    push $0x0
    push $0x10
    jmp isr_common_stub

isr17:
    cli
    push $0x0
    push $0x11
    jmp isr_common_stub

isr18:
    cli
    push $0x0
    push $0x12
    jmp isr_common_stub

isr19:
    cli
    push $0x0
    push $0x13
    jmp isr_common_stub

isr20:
    cli
    push $0x0
    push $0x14
    jmp isr_common_stub

isr21:
    cli
    push $0x0
    push $0x15
    jmp isr_common_stub

isr22:
    cli
    push $0x0
    push $0x16
    jmp isr_common_stub

isr23:
    cli
    push $0x0
    push $0x17
    jmp isr_common_stub

isr24:
    cli
    push $0x0
    push $0x18
    jmp isr_common_stub

isr25:
    cli
    push $0x0
    push $0x19
    jmp isr_common_stub

isr26:
    cli
    push $0x0
    push $0x1A
    jmp isr_common_stub

isr27:
    cli
    push $0x0
    push $0x1B
    jmp isr_common_stub

isr28:
    cli
    push $0x0
    push $0x1C
    jmp isr_common_stub

isr29:
    cli
    push $0x0
    push $0x1D
    jmp isr_common_stub

isr30:
    cli
    push $0x0
    push $0x1E
    jmp isr_common_stub

isr31:
    cli
    push $0x0
    push $0x1F
    jmp isr_common_stub

irq0:
    cli
    push $0x0
    push $32
    jmp irq_common_stub

irq1:
    cli
    push $0x0
    push $33
    jmp irq_common_stub

irq2:
    cli
    push $0x0
    push $34
    jmp irq_common_stub

irq3:
    cli
    push $0x0
    push $35
    jmp irq_common_stub

irq4:
    cli
    push $0x0
    push $36
    jmp irq_common_stub

irq5:
    cli
    push $0x0
    push $37
    jmp irq_common_stub

irq6:
    cli
    push $0x0
    push $38
    jmp irq_common_stub

irq7:
    cli
    push $0x0
    push $39
    jmp irq_common_stub

irq8:
    cli
    push $0x0
    push $40
    jmp irq_common_stub

irq9:
    cli
    push $0x0
    push $41
    jmp irq_common_stub

irq10:
    cli
    push $0x0
    push $42
    jmp irq_common_stub

irq11:
    cli
    push $0x0
    push $43
    jmp irq_common_stub

irq12:
    cli
    push $0x0
    push $44
    jmp irq_common_stub

irq13:
    cli
    push $0x0
    push $45
    jmp irq_common_stub

irq14:
    cli
    push $0x0
    push $46
    jmp irq_common_stub

irq15:
    cli
    push $0x0
    push $47
    jmp irq_common_stub


isr_common_stub:
    pusha
    mov %ds,%ax
    push %eax
    mov $0x10,%ax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    call isrHandler
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

    call irqHandler

    pop %ebx
    mov %ebx, %ds
    mov %ebx, %es
    mov %ebx, %fs
    mov %ebx, %gs

    popa
    add $0x8, %esp
    sti
    iret