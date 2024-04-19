#ifndef USR_INCLUDE_STRING_H
#define USR_INCLUDE_STRING_H

void memset(void *buf, char c, unsigned size);

void memcpy(const void *src, void *dst, unsigned size);

int strcmp(const char *a, const char *b);

unsigned strlen(const char *a);

unsigned str_to_uint(const char *str);

#endif
