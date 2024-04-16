#ifndef USR_SYSTEM_H
#define USR_SYSTEM_H

void write(void *data, unsigned size);
void read(void *data, unsigned size);
void *malloc(unsigned size);
void free(void *ptr);
void writehex(unsigned num);

#endif
