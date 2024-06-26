#include "string.h"

#include "memory.h"

u32 strlen(const i8 *src) {
	assert(src != NULL, "strlen: src is null");
	const i8 *ptr = src;
	while (*ptr)
		ptr++;
	return ptr - src;
}

u32 strcmp(const i8 *src1, const i8 *src2) {
	/* Checked to prevent weird strings */
	for (u32 i = 0; i < 1000; i++) {
		if (src1[i] == '\0' && src2[i] == '\0')
			return 0;
		if (src1[i] == src2[i])
			continue;
		if (src1[i] < src2[i])
			return -1;
		return 1;
	}
	assert(false, "strcmp: Maxium string length exceeded");
	return 0;
}

u32 strcmp_span(const i8 *src, const i8 *start, u32 size) {
	for (u32 i = 0; i < size; i++) {
		if (src[i] == '\0')
			return -1;
		if (src[i] == start[i])
			continue;
		if (src[i] < start[i])
			return -1;
		return 1;
	}
	return 0;
}

i32 str_to_int(const i8 *s) {
	bool is_negative = false;
	i32 ret = 0;
	
	u32 i = 0;
	if (s[0] == '-') {
		is_negative = true;
		i++;
	}

	u32 len = strlen(s);
	for (; i < len; i++) {
		u8 c = s[i] - '0';
		assert(c < 10, "str_to_int: Cannot convert string to integer\n");
		ret *= 10;
		ret += c;
	}
	if (is_negative)
		return -ret;
	return ret;
}

u32 str_to_uint(const i8 *s) {
	i32 ret = 0;
	u32 len = strlen(s);
	for (u32 i = 0; i < len; i++) {
		u8 c = s[i] - '0';
		assert(c < 10, "str to int: Cannot convert string to integer\n");
		ret *= 10;
		ret += c;
	}
	return ret;
}
