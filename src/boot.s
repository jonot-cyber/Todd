.global mboot
.extern code
.extern bss
.extern end

mboot:
	.4byte 0x1BADB002 /* Multiboot magic number, shows that it is multiboot */
	.4byte (1 << 0) | (1 << 1) /* Setup boot flags */
	.4byte -(0x1BADB002 + (1 << 0) | (1 << 1)) /* Checksum to guarantee that it is intentional */
	/* All of the memory addresses */
	.4byte mboot
	.4byte code
	.4byte bss
	.4byte end
	.4byte start

.global start
.extern kmain

start:
	push %esp
	push %ebx /* Push multiboot information to the stack */

	cli /* Disable interrupts */
	call kmain /* Call main method */
	hlt /* Just tell the cpu to turn off if main ends */
  
