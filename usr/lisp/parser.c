#include "stdio.h"
#include "string.h"
#include "system.h"

#include "parser.h"

#include "lexer.h"


const unsigned MAX_DEPTH = 64;

struct ParserListContents* parse_list_contents(const char**);
struct ParserValue* parse_value(const char** p);

void mempeek(const char* p, const unsigned c) {
	char *buf = malloc(c + 1);
	memcpy(p, buf, c);
	buf[c] = '\0';
	printf("%s", buf);
	free(buf);
}

struct ParserListContents* parse_list(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserListContents* contents;
	if (token.type != LEFT) {
		goto err;
	}
	contents = parse_list_contents(p);
	if (contents == 0) {
		goto err;
	}
	token = lex(p);
	if (token.type != RIGHT) {
		goto err;
	}
	return contents;
err:
	*p = start;
	return 0;
}

struct ParserListContents* parse_quote_list(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserListContents* contents;
	if (token.type != QUOTE_LEFT) {
		goto err;
	}
	contents = parse_list_contents(p);
	if (contents == 0) {
		goto err;
	}
	token = lex(p);
	if (token.type != RIGHT) {
		goto err;
	}
	return contents;
err:
	*p = start;
	return 0;
}


struct ParserListContents* parse_pair(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserValue* v1;
	struct ParserValue* v2;
	struct ParserListContents* ret;
	struct ParserListContents* nest;
	if (token.type != LEFT) {
		goto err;
	}
	v1 = parse_value(p);
	if (v1 == 0) {
		goto err;
	}
	token = lex(p);
	if (token.type != DOT) {
		goto err;
	}
	v2 = parse_value(p);
	if (v2 == 0) {
		goto err;
	}
	token = lex(p);
	if (token.type != RIGHT) {
		goto err;
	}
	ret = malloc(sizeof(struct ParserListContents));
	ret->v = v1;
	nest = malloc(sizeof(struct ParserListContents));
	ret->n = nest;
	nest->v = v2;
	nest->n = 0;
	return ret;
err:
	*p = start;
	return 0;
}

struct ParserSpan* parse_quote_symbol(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != QUOTE_SYMBOL) {
		goto err;
	}
	ret = malloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return 0;
}

struct ParserSpan* parse_symbol(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	if (token.type != SYMBOL) {
		*p = start;
		return 0;
	}
	struct ParserSpan* ret = malloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
}

struct ParserSpan* parse_number(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != INT) {
		goto err;
	}
	ret = malloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return 0;
}

struct ParserSpan* parse_path(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != PATH) {
		goto err;
	}
	ret = malloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return 0;
}

struct ParserSpan* parse_string(const char** p) {
	const char* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != STRING) {
		goto err;
	}
	ret = malloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return 0;
}

struct ParserValue* parse_value(const char** p) {
	struct ParserListContents* list = parse_list(p);
	if (list != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = list;
		ret->t = P_LIST;
		return ret;
	}
	struct ParserListContents* pair = parse_pair(p);
	if (pair != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = pair;
		ret->t = P_PAIR;
		return ret;
	}
	struct ParserListContents* quote_list = parse_quote_list(p);
	if (quote_list != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = quote_list;
		ret->t = P_QUOTE_LIST;
		return ret;
	}
	struct ParserSpan* quote_symbol = parse_quote_symbol(p);
	if (quote_symbol != 0){
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = quote_symbol;
		ret->t = P_QUOTE_SYMBOL;
		return ret;
	}
	struct ParserSpan* number = parse_number(p);
	if (number != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = number;
		ret->t = P_INT;
		return ret;
	}
	struct ParserSpan* symbol = parse_symbol(p);
	if (symbol != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = symbol;
		ret->t = P_SYMBOL;
		return ret;
	}
	struct ParserSpan* path = parse_path(p);
	if (path != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = path;
		ret->t = P_PATH;
		return ret;
	}	
	struct ParserSpan* string = parse_string(p);
	if (string != 0) {
		struct ParserValue* ret = malloc(sizeof(struct ParserValue));
		ret->data = string;
		ret->t = P_STRING;
		return ret;
	}
	return 0;
}

struct ParserListContents* parse_list_contents(const char** p) {
	const char* start = *p;
	struct ParserValue* value = parse_value(p);
	if (value == 0) {
		*p = start;
		return 0;
	}
	struct ParserListContents* ret = malloc(sizeof(struct ParserListContents));
	ret->v = value;
	start = *p;
	ret->n = parse_list_contents(p);
	if (ret->n == 0) {
		*p = start;
	}
	return ret;
}

struct ParserListContents* parse(const char** p) {
	struct ParserListContents* ret = parse_list_contents(p);
	return ret;
}
