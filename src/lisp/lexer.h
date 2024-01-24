#ifndef LEXER_H
#define LEXER_H

#include "common.h"

enum LexerTokenType {
	LEFT,
	RIGHT,
	PATH,
	DOT,
	QUOTE_LEFT,
	QUOTE_SYMBOL,
	INT,
	SYMBOL,
	STRING,
	EOF,
};

struct LexerToken {
	enum LexerTokenType type;
	const i8* start;
	const i8* end;
};

struct LexerToken lex(const i8** ptr);

#endif
