#include "parser.h"

#include "io.h"
#include "lexer.h"
#include "memory.h"

const u32 MAX_DEPTH = 64;

struct ParserListContents* parse_list_contents(const i8**);
struct ParserValue* parse_value(const i8** p);

void mempeek(const i8* p, const u32 c) {
	i8* buf = kmalloc(c + 1);
	memcpy(p, buf, c);
	buf[c] = '\0';
	write_string(buf);
	kfree(buf);
}

struct ParserListContents* parse_list(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserListContents* contents;
	if (token.type != LEFT) {
		goto err;
	}
	contents = parse_list_contents(p);
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

struct ParserListContents* parse_quote_list(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserListContents* contents;
	if (token.type != QUOTE_LEFT) {
		goto err;
	}
	contents = parse_list_contents(p);
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


struct ParserListContents* parse_pair(const i8** p) {
	const i8* start = *p;
	struct LexerToken token = lex(p);
	struct ParserValue* v1;
	struct ParserValue* v2;
	struct ParserListContents* ret;
	struct ParserListContents* nest;
	if (token.type != LEFT) {
		goto err;
	}
	v1 = parse_value(p);
	if (v1 == NULL) {
		goto err;
	}
	token = lex(p);
	if (token.type != DOT) {
		goto err;
	}
	v2 = parse_value(p);
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

struct ParserSpan* parse_quote_symbol(const i8** p) {
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

struct ParserSpan* parse_symbol(const i8** p) {
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

struct ParserSpan* parse_number(const i8** p) {
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

struct ParserSpan* parse_path(const i8** p) {
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

struct ParserSpan* parse_string(const i8** p) {
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

struct ParserValue* parse_value(const i8** p) {
	struct ParserListContents* list = parse_list(p);
	if (list != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = list;
		ret->t = P_LIST;
		return ret;
	}
	struct ParserListContents* pair = parse_pair(p);
	if (pair != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = pair;
		ret->t = P_PAIR;
		return ret;
	}
	struct ParserListContents* quote_list = parse_quote_list(p);
	if (quote_list != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = quote_list;
		ret->t = P_QUOTE_LIST;
		return ret;
	}
	struct ParserSpan* quote_symbol = parse_quote_symbol(p);
	if (quote_symbol != NULL){
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = quote_symbol;
		ret->t = P_QUOTE_SYMBOL;
		return ret;
	}
	struct ParserSpan* number = parse_number(p);
	if (number != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = number;
		ret->t = P_INT;
		return ret;
	}
	struct ParserSpan* symbol = parse_symbol(p);
	if (symbol != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = symbol;
		ret->t = P_SYMBOL;
		return ret;
	}
	struct ParserSpan* path = parse_path(p);
	if (path != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = path;
		ret->t = P_PATH;
		return ret;
	}	
	struct ParserSpan* string = parse_string(p);
	if (string != NULL) {
		struct ParserValue* ret = kmalloc(sizeof(struct ParserValue));
		ret->data = string;
		ret->t = P_STRING;
		return ret;
	}
	return NULL;
}

struct ParserListContents* parse_list_contents(const i8** p) {
	const i8* start = *p;
	struct ParserValue* value = parse_value(p);
	if (value == NULL) {
		*p = start;
		return NULL;
	}
	struct ParserListContents* ret = kmalloc(sizeof(struct ParserListContents));
	ret->v = value;
	start = *p;
	ret->n = parse_list_contents(p);
	if (ret->n == NULL) {
		*p = start;
	}
	return ret;
}

struct ParserListContents* parse(const i8** p) {
	struct ParserListContents* ret = parse_list_contents(p);
	if (ret == NULL) {
		printf("PANIC: Failed to parse list contents\n");
		halt();
	}
	return ret;
}
