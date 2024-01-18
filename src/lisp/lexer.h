#pragma once

#include "common.h"

namespace Lisp {
	enum TokenType {
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
	
	/**
	   Tokens from the lexer.
	 */
	struct Token {
		TokenType type;
		const i8* start;
		const i8* end;
	};
	
	Token lex(const i8** ptr);
};
