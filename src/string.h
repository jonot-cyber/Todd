#ifndef STRING_H
#define STRING_H

#include "common.h"

u32 strlen(const i8* src);
u32 strcmp(const i8* src1, const i8* src2);
u32 strcmp_span(const i8* src, const i8* start, u32 size);

i32 str_to_int(const i8*);
u32 str_to_uint(const i8*);

#endif
