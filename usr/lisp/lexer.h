#ifndef LEXER_H
#define LEXER_H

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
	const char *start;
	const char *end;
};

struct LexerToken lex(const char **ptr);

#endif
