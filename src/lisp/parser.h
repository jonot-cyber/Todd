#ifndef PARSER_H
#define PARSER_H

#include "common.h"

enum ParserASTType {
	P_LIST_CONTENTS,
	P_VALUE,
	P_QUOTE_LIST,
	P_PAIR,
	P_LIST,
	P_QUOTE_SYMBOL,
	P_INT,
	P_SYMBOL,
	P_PATH,
	P_STRING,
	P_PROGRAM
};

struct ParserSpan {
	const i8* start;
	const i8* end;
};

struct ParserValue {
	enum ParserASTType t;
	void* data;
};

struct ParserListContents {
	struct ParserValue* v;
	struct ParserListContents* n;
};

struct ParserListContents* parse(const i8**);

#endif
