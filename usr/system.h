#ifndef USR_SYSTEM_H
#define USR_SYSTEM_H

extern void __sys0(unsigned);
extern void __sys1(unsigned, unsigned);
extern void __sys2(unsigned, unsigned, unsigned);

void write(void* data, unsigned size) {
	__sys2(0, (unsigned)data, size);
}

#endif
