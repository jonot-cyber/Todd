#include "parser.h"

#include "lexer.h"
#include "memory.h"
#include "monitor.h"

const u32 MAX_DEPTH = 64;

struct ParserListContents* parseListContents(const i8**);
struct ParserValue* parseValue(const i8** p);

void mempeek(const i8* p, const u32 c) {
	i8* buf = kmalloc(c + 1);
	memcpy(p, buf, c);
	buf[c] = '\0';
	write_string(buf);
	kfree(buf);
}

struct ParserListContents* parseList(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserListContents* contents;
	if (token.type != LEFT) {
		goto err;
	}
	contents = parseListContents(p);
	if (contents == NULL) {
		goto err;
	}
	token = lex(p);
	if (token.type != RIGHT) {
		goto err;
	}
	return contents;
err:
	*p = start;
	return NULL;
}

struct ParserListContents* parseQuoteList(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserListContents* contents;
	if (token.type != QUOTE_LEFT) {
		goto err;
	}
	contents = parseListContents(p);
	if (contents == NULL) {
		goto err;
	}
	token = lex(p);
	if (token.type != RIGHT) {
		goto err;
	}
	return contents;
err:
	*p = start;
	return NULL;
}


struct ParserListContents* parsePair(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserValue* v1;
	struct ParserValue* v2;
	struct ParserListContents* ret;
	struct ParserListContents* nest;
	if (token.type != LEFT) {
		goto err;
	}
	v1 = parseValue(p);
	if (v1 == NULL) {
		goto err;
	}
	token = lex(p);
	if (token.type != DOT) {
		goto err;
	}
	v2 = parseValue(p);
	if (v2 == NULL) {
		goto err;
	}
	token = lex(p);
	if (token.type != RIGHT) {
		goto err;
	}
	ret = kmalloc(sizeof(struct ParserListContents));
	ret->v = v1;
	nest = kmalloc(sizeof(struct ParserListContents));
	ret->n = nest;
	nest->v = v2;
	nest->n = NULL;
	return ret;
err:
	*p = start;
	return NULL;
}

struct ParserSpan* parseQuoteSymbol(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != QUOTE_SYMBOL) {
		goto err;
	}
	ret = kmalloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return NULL;
}

struct ParserSpan* parseSymbol(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != SYMBOL) {
		goto err;
	}
	ret = kmalloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return NULL;
}

struct ParserSpan* parseNumber(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != INT) {
		goto err;
	}
	ret = kmalloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return NULL;
}

struct ParserSpan* parsePath(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != PATH) {
		goto err;
	}
	ret = kmalloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return NULL;
}

struct ParserSpan* parseString(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserSpan* ret;
	if (token.type != STRING) {
		goto err;
	}
	ret = kmalloc(sizeof(struct ParserSpan));
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return NULL;
}

struct ParserValue* parseValue(const i8** p) {
	struct ParserListContents* list = parseList(p);
	if (list != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = list;
		ret->t = P_LIST;
		return ret;
	}
	struct ParserListContents* pair = parsePair(p);
	if (pair != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = pair;
		ret->t = P_PAIR;
		return ret;
	}
	struct ParserListContents* quoteList = parseQuoteList(p);
	if (quoteList != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = quoteList;
		ret->t = P_QUOTE_LIST;
		return ret;
	}
	struct ParserSpan* quoteSymbol = parseQuoteSymbol(p);
	if (quoteSymbol != NULL){
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = quoteSymbol;
		ret->t = P_QUOTE_SYMBOL;
		return ret;
	}
	struct ParserSpan* number = parseNumber(p);
	if (number != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = number;
		ret->t = P_INT;
		return ret;
	}
	struct ParserSpan* symbol = parseSymbol(p);
	if (symbol != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = symbol;
		ret->t = P_SYMBOL;
		return ret;
	}
	struct ParserSpan* path = parsePath(p);
	if (path != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = path;
		ret->t = P_PATH;
		return ret;
	}	
	struct ParserSpan* string = parseString(p);
	if (string != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = string;
		ret->t = P_STRING;
		return ret;
	}
	return NULL;
}

struct ParserListContents* parseListContents(const i8** p) {
	const i8* start = *p;
	struct ParserValue* value = parseValue(p);
	if (value == NULL) {
		*p = start;
		return NULL;
	}
	struct ParserListContents* ret = kmalloc(sizeof(struct ParserListContents));
	ret->v = value;
	start = *p;
	ret->n = parseListContents(p);
	if (ret->n == NULL) {
		*p = start;
	}
	return ret;
}

struct ParserListContents* parse(const i8** p) {
	struct ParserListContents* ret = parseListContents(p);
	if (ret == NULL) {
		printf("PANIC: Failed to parse list contents\n");
		halt();
	}
	return ret;
}
