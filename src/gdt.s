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

.extern isrHandler

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
    pushl 0
    pushl 5
    jmp isr_common_stub

isr6:
    cli
    pushl 0
    pushl 6
    jmp isr_common_stub

isr7:
    cli
    pushl 0
    pushl 7
    jmp isr_common_stub

isr8:
    cli
    pushl 8
    jmp isr_common_stub

isr9:
    cli
    pushl 0
    pushl 9
    jmp isr_common_stub

isr10:
    cli
    pushl 10
    jmp isr_common_stub

isr11:
    cli
    pushl 11
    jmp isr_common_stub

isr12:
    cli
    pushl 12
    jmp isr_common_stub

isr13:
    cli
    pushl 13
    jmp isr_common_stub

isr14:
    cli
    pushl 14
    jmp isr_common_stub

isr15:
    cli
    pushl 0
    pushl 15
    jmp isr_common_stub

isr16:
    cli
    pushl 0
    pushl 16
    jmp isr_common_stub

isr17:
    cli
    pushl 0
    pushl 17
    jmp isr_common_stub

isr18:
    cli
    pushl 0
    pushl 18
    jmp isr_common_stub

isr19:
    cli
    pushl 0
    pushl 19
    jmp isr_common_stub

isr20:
    cli
    pushl 0
    pushl 20
    jmp isr_common_stub

isr21:
    cli
    pushl 0
    pushl 21
    jmp isr_common_stub

isr22:
    cli
    pushl 0
    pushl 22
    jmp isr_common_stub

isr23:
    cli
    pushl 0
    pushl 23
    jmp isr_common_stub

isr24:
    cli
    pushl 0
    pushl 24
    jmp isr_common_stub

isr25:
    cli
    pushl 0
    pushl 25
    jmp isr_common_stub

isr26:
    cli
    pushl 0
    pushl 26
    jmp isr_common_stub

isr27:
    cli
    pushl 0
    pushl 27
    jmp isr_common_stub

isr28:
    cli
    pushl 0
    pushl 28
    jmp isr_common_stub

isr29:
    cli
    pushl 0
    pushl 29
    jmp isr_common_stub

isr30:
    cli
    pushl 0
    pushl 30
    jmp isr_common_stub

isr31:
    cli
    pushl 0
    pushl 31
    jmp isr_common_stub

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