#include "parser.h"

#include "lexer.h"
#include "memory.h"
#include "monitor.h"

using namespace Lisp::AST;

const u32 MAX_DEPTH = 64;

void Span::output(u32 depth) {
	if (depth >= MAX_DEPTH) return;
	for (u32 i = 0; i < depth; i++) {
		Monitor::writeChar(' ');
	}
	Monitor::writeString("SPAN\n");
	for (u32 i = 0; i < depth+1; i++) {
		Monitor::writeChar(' ');
	}
	i8* buf = Memory::kmalloc<i8>(end - start + 1);
	memcpy(start, buf, end - start);
	buf[end - start] = '\0';
	Monitor::printf("%s\n", buf);
	Memory::kfree(buf);
	return;
}

void Value::output(u32 depth) {
	if (depth >= MAX_DEPTH) return;
	for (u32 i = 0; i < depth; i++) {
		Monitor::writeChar(' ');
	}
	Monitor::writeString("VALUE\n");
	for (u32 i = 0; i < depth+1; i++) {
		Monitor::writeChar(' ');
	}
	Monitor::printf("TYPE: %d\n", t);
	if (t == Type::LIST_CONTENTS || t == Type::LIST || t == Type::QUOTE_LIST || t == Type::PAIR) {
		((ListContents*)data)->output(depth + 1);
	} else {
		((Span*)data)->output(depth + 1);
	}
}

void ListContents::output(u32 depth) {
	if (depth >= MAX_DEPTH) return;
	for (u32 i = 0; i < depth; i++) {
		Monitor::writeChar(' ');
	}
	Monitor::writeString("LIST_CONTENTS\n");
	v->output(depth + 1);
	if (!n) {
		return;
	}
	n->output(depth + 1);
}

void mempeek(const i8* p, const u32 c) {
	i8* buf = Memory::kmalloc<i8>(c + 1);
	memcpy(p, buf, c);
	buf[c] = '\0';
	Monitor::writeString(buf);
	Memory::kfree(buf);
}

ListContents* parseList(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	ListContents* contents;
	if (token.type != Lisp::TokenType::LEFT) {
		goto err;
	}
	contents = parseListContents(p);
	if (contents == nullptr) {
		goto err;
	}
	token = Lisp::lex(p);
	if (token.type != Lisp::TokenType::RIGHT) {
		goto err;
	}
	return contents;
err:
	*p = start;
	return nullptr;
}

ListContents* parseQuoteList(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	ListContents* contents;
	if (token.type != Lisp::TokenType::QUOTE_LEFT) {
		goto err;
	}
	contents = parseListContents(p);
	if (contents == nullptr) {
		goto err;
	}
	token = Lisp::lex(p);
	if (token.type != Lisp::TokenType::RIGHT) {
		goto err;
	}
	return contents;
err:
	*p = start;
	return nullptr;
}


ListContents* parsePair(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	Value* v1;
	Value* v2;
	ListContents* ret;
	ListContents* nest;
	if (token.type != Lisp::TokenType::LEFT) {
		goto err;
	}
	v1 = parseValue(p);
	if (v1 == nullptr) {
		goto err;
	}
	token = Lisp::lex(p);
	if (token.type != Lisp::TokenType::DOT) {
		goto err;
	}
	v2 = parseValue(p);
	if (v2 == nullptr) {
		goto err;
	}
	token = Lisp::lex(p);
	if (token.type != Lisp::TokenType::RIGHT) {
		goto err;
	}
	ret = Memory::kmalloc<ListContents>();
	ret->v = v1;
	nest = Memory::kmalloc<ListContents>();
	ret->n = nest;
	nest->v = v2;
	nest->n = nullptr;
	return ret;
err:
	*p = start;
	return nullptr;
}

Span* parseQuoteSymbol(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	Span* ret;
	if (token.type != Lisp::TokenType::QUOTE_SYMBOL) {
		goto err;
	}
	ret = Memory::kmalloc<Span>();
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return nullptr;
}

Span* parseSymbol(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	Span* ret;
	if (token.type != Lisp::TokenType::SYMBOL) {
		goto err;
	}
	ret = Memory::kmalloc<Span>();
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return nullptr;
}

Span* parseNumber(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	Span* ret;
	if (token.type != Lisp::TokenType::INT) {
		goto err;
	}
	ret = Memory::kmalloc<Span>();
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return nullptr;
}

Span* parsePath(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	Span* ret;
	if (token.type != Lisp::TokenType::PATH) {
		goto err;
	}
	ret = Memory::kmalloc<Span>();
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return nullptr;
}

Span* parseString(const i8** p) {
	const i8* start = *p;
	Lisp::Token token = Lisp::lex(p);
	Span* ret;
	if (token.type != Lisp::TokenType::STRING) {
		goto err;
	}
	ret = Memory::kmalloc<Span>();
	ret->start = token.start;
	ret->end = token.end;
	return ret;
err:
	*p = start;
	return nullptr;
}

Value* Lisp::AST::parseValue(const i8** p) {
	ListContents* list = parseList(p);
	if (list != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = list;
		ret->t = Type::LIST;
		return ret;
	}
	ListContents* pair = parsePair(p);
	if (pair != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = pair;
		ret->t = Type::PAIR;
		return ret;
	}
	ListContents* quoteList = parseQuoteList(p);
	if (quoteList != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = quoteList;
		ret->t = Type::QUOTE_LIST;
		return ret;
	}
	Span* quoteSymbol = parseQuoteSymbol(p);
	if (quoteSymbol != nullptr){
		Value* ret = Memory::kmalloc<Value>();
		ret->data = quoteSymbol;
		ret->t = Type::QUOTE_SYMBOL;
		return ret;
	}
	Span* number = parseNumber(p);
	if (number != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = number;
		ret->t = Type::INT;
		return ret;
	}
	Span* symbol = parseSymbol(p);
	if (symbol != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = symbol;
		ret->t = Type::SYMBOL;
		return ret;
	}
	Span* path = parsePath(p);
	if (path != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = path;
		ret->t = Type::PATH;
		return ret;
	}	
	Span* string = parseString(p);
	if (string != nullptr) {
		Value* ret = Memory::kmalloc<Value>();
		ret->data = string;
		ret->t = Type::STRING;
		return ret;
	}
	return nullptr;
}

ListContents* Lisp::AST::parseListContents(const i8** p) {
	const i8* start = *p;
	Value* value = parseValue(p);
	if (value == nullptr) {
		*p = start;
		return nullptr;
	}
	ListContents* ret = Memory::kmalloc<ListContents>();
	ret->v = value;
	start = *p;
	ret->n = Lisp::AST::parseListContents(p);
	if (ret->n == nullptr) {
		*p = start;
	}
	return ret;
}

ListContents* Lisp::parse(const i8** p) {
	ListContents* ret = parseListContents(p);
	if (ret == nullptr) {
		Monitor::printf("PANIC: Failed to parse list contents\n");
		halt();
	}
	return ret;
}
