.globl GDTFlush

GDTFlush:
    mov %eax, 4(%esp)
    lgdt (%esp)

    mov %ax, 0x10
    mov %ds, %ax
    mov %es, %ax
    mov %fs, %ax
    mov %gs, %ax
    mov %ss, %ax
    jmp .flush
.flush:
    ret
