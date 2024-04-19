#include "stdio.h"
#include "string.h"
#include "system.h"

#include "lexer.h"

int is_alpha(char c) {
	return c >= 'A' && c <= 'z';
}

int is_digit(char c) {
	return c >= '0' && c <= '9';
}

int is_valid_symbol_char(char c) {
	if (c == '"' || c == '\'' || c == '.' || c == '(' || c == ')') {
		return 0;
	}
	return c > ' ' && c <= '~';
}

int is_valid_symbol_start_char(char c) {
	if (is_digit(c) || c == '\'') {
		return 0;
	}
	return is_valid_symbol_char(c);
}

struct LexerToken lex(const char **ptr) {
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
			const char *path_start = *ptr;
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
			const char *start = *ptr;
			const char *end = *ptr + 2;
			*ptr += 2;
			struct LexerToken ret = {QUOTE_LEFT, start, end};
			return ret;
		}
		if (is_valid_symbol_start_char(*(*ptr+1))) {
			(*ptr)++;
			const char *symbol_start = *ptr;
			while (is_valid_symbol_char(*(*ptr+1))) {
				(*ptr)++;
			}
			struct LexerToken ret = {QUOTE_SYMBOL, symbol_start, ++*ptr};
			return ret;
		}
		break;
	case ' ':
	case '\n':
		(*ptr)++;
		goto switch_label;
	case '"':
	{
		const char* string_start = *ptr;
		do {
			(*ptr)++;
		} while (**ptr != '"');
		struct LexerToken ret = {STRING, string_start, ++*ptr};
		return ret;
	}
	default:
	{
		if (is_digit(**ptr)) {
			const char* digit_start = *ptr;
			while (is_digit(*(*ptr+1))) {
				(*ptr)++;
			}
			struct LexerToken ret = {INT, digit_start, ++*ptr};
			return ret;
		}
		if (is_valid_symbol_start_char(**ptr)) {
			const char* symbol_start = *ptr;
			while (is_valid_symbol_char(*(*ptr+1))) {
				(*ptr)++;
			}
			struct LexerToken ret = {SYMBOL, symbol_start, ++*ptr};
			return ret;
		}
		// INVALID!
		printf("PANIC: Parse error\n");
		char *buf = malloc(11);
		memcpy(*ptr, buf, 10);
		buf[10] = '\0';
		printf("%s", buf);
		free(buf);
		struct LexerToken ret = {EOF, 0, 0};
		return ret;
	}
	}
	// This is illegal
	struct LexerToken ret = {EOF, 0, 0};
	return ret;
}
