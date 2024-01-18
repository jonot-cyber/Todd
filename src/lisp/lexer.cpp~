#include "lexer.h"

#include "../monitor.h"
#include "../memory.h"

bool isAlpha(i8 c) {
	return c >= 'A' && c <= 'z';
}

bool isDigit(i8 c) {
	return c >= '0' && c <= '9';
}

bool isValidSymbolChar(i8 c) {
	if (c == '"' || c == '\'' || c == '.' || c == '(' || c == ')') {
		return false;
	}
	return c > ' ' && c <= '~';
}

bool isValidSymbolStartChar(i8 c) {
	if (isDigit(c) || c == '\'') {
		return false;
	}
	return isValidSymbolChar(c);
}

Lisp::Token Lisp::lex(const i8** ptr) {
switchLabel:
	switch (**ptr) {
	case '\0':
		return {TokenType::EOF, *ptr, *ptr};
	case ';':
		// Skip comments;
		while (**ptr != '\n') (*ptr)++;
		(*ptr)++;
		goto switchLabel;
	case '(':
		return {TokenType::LEFT, *ptr, ++*ptr};
	case ')':
		return {TokenType::RIGHT, *ptr, ++*ptr};
	case '.':
		if (*(*ptr + 1) == '/') {
			const i8* pathStart = *ptr;
			(*ptr)++;
			while (isValidSymbolChar(**ptr) || **ptr == '.') {
				(*ptr)++;
			}
			return {TokenType::PATH, pathStart, *ptr};
		}
		return {TokenType::DOT, *ptr, ++*ptr};
	case '\'':
		if (*(*ptr+1) == '(') {
			const i8* start = *ptr;
			const i8* end = *ptr + 2;
			*ptr += 2;
			return {TokenType::QUOTE_LEFT, start, end};
		}
		if (isValidSymbolStartChar(*(*ptr+1))) {
			(*ptr)++;
			const i8* symbolStart = *ptr;
			while (isValidSymbolChar(*(*ptr+1))) {
				(*ptr)++;
			}
			return {TokenType::QUOTE_SYMBOL, symbolStart, ++*ptr};
		}
		break;
	case ' ':
	case '\n':
		(*ptr)++;
		goto switchLabel;
	case '"':
	{
		const i8* stringStart = *ptr;
		do {
			(*ptr)++;
		} while (**ptr != '"');
		return {TokenType::STRING, stringStart, ++*ptr};
	}
	default:
		if (isDigit(**ptr)) {
			const i8* digitStart = *ptr;
			while (isDigit(*(*ptr+1))) {
				(*ptr)++;
			}
			return {TokenType::INT, digitStart, ++*ptr};
		}
		if (isValidSymbolStartChar(**ptr)) {
			const i8* symbolStart = *ptr;
			while (isValidSymbolChar(*(*ptr+1))) {
				(*ptr)++;
			}
			return {TokenType::SYMBOL, symbolStart, ++*ptr};
		}
		// INVALID!
		Monitor::writeString("PANIC: Parse error\n");
		i8* buf = Memory::kmalloc<i8>(11);
		memcpy(*ptr, buf, 10);
		buf[10] = '\0';
		Monitor::writeString(buf);
		Memory::kfree(buf);
		halt();
		return {TokenType::EOF, nullptr, nullptr};
	}
	// This is illegal
	return {TokenType::EOF, nullptr, nullptr};
}
