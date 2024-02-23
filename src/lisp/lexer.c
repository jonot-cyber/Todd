#include "lexer.h"

#include "../io.h"
#include "../memory.h"
#include "lex_test.h"

bool is_alpha(i8 c) {
	return c >= 'A' && c <= 'z';
}

bool is_digit(i8 c) {
	return c >= '0' && c <= '9';
}

bool is_valid_symbol_char(i8 c) {
	if (c == '"' || c == '\'' || c == '.' || c == '(' || c == ')') {
		return false;
	}
	return c > ' ' && c <= '~';
}

bool is_valid_symbol_start_char(i8 c) {
	if (is_digit(c) || c == '\'') {
		return false;
	}
	return is_valid_symbol_char(c);
}

struct LexerToken lex(const i8** ptr) {
switch_label:
	switch (**ptr) {
	case '\0':
	{
		struct LexerToken ret = {EOF, *ptr, *ptr};
		return ret;
	}
	case ';':
		// Skip comments;
		while (**ptr != '\n') (*ptr)++;
		(*ptr)++;
		goto switch_label;
	case '(':
	{
		struct LexerToken ret = {LEFT, *ptr, ++*ptr};
		return ret;
	}
	case ')':
	{
		struct LexerToken ret = {RIGHT, *ptr, ++*ptr};
		return ret;
	}
	case '.':
		if (*(*ptr + 1) == '/') {
			const i8* path_start = *ptr;
			(*ptr)++;
			while (is_valid_symbol_char(**ptr) || **ptr == '.') {
				(*ptr)++;
			}
			struct LexerToken ret = {PATH, path_start, *ptr};
			return ret;
		}
		struct LexerToken ret = {DOT, *ptr, ++*ptr};
		return ret;
	case '\'':
		if (*(*ptr+1) == '(') {
			const i8* start = *ptr;
			const i8* end = *ptr + 2;
			*ptr += 2;
			struct LexerToken ret = {QUOTE_LEFT, start, end};
			return ret;
		}
		if (is_valid_symbol_start_char(*(*ptr+1))) {
			(*ptr)++;
			const i8* symbolStart = *ptr;
			while (is_valid_symbol_char(*(*ptr+1))) {
				(*ptr)++;
			}
			struct LexerToken ret = {QUOTE_SYMBOL, symbolStart, ++*ptr};
			return ret;
		}
		break;
	case ' ':
	case '\n':
		(*ptr)++;
		goto switch_label;
	case '"':
	{
		const i8* stringStart = *ptr;
		do {
			(*ptr)++;
		} while (**ptr != '"');
		struct LexerToken ret = {STRING, stringStart, ++*ptr};
		return ret;
	}
	default:
	{
		if (is_digit(**ptr)) {
			const i8* digitStart = *ptr;
			while (is_digit(*(*ptr+1))) {
				(*ptr)++;
			}
			struct LexerToken ret = {INT, digitStart, ++*ptr};
			return ret;
		}
		if (is_valid_symbol_start_char(**ptr)) {
			const i8* symbolStart = *ptr;
			while (is_valid_symbol_char(*(*ptr+1))) {
				(*ptr)++;
			}
			struct LexerToken ret = {SYMBOL, symbolStart, ++*ptr};
			return ret;
		}
		// INVALID!
		write_string("PANIC: Parse error\n");
		i8* buf = kmalloc(11 * sizeof(i8));
		memcpy(*ptr, buf, 10);
		buf[10] = '\0';
		write_string(buf);
		kfree(buf);
		halt();
		struct LexerToken ret = {EOF, NULL, NULL};
		return ret;
	}
	}
	// This is illegal
	struct LexerToken ret = {EOF, NULL, NULL};
	return ret;
}
