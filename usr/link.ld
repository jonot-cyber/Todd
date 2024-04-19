/* Silly linker script :3 I don't know what I'm doing at all :D */
ENTRY(_start)
SECTIONS {
    .text : {
		. = ALIGN(4);
		*(.text)
		*(.text*)
    }
}
