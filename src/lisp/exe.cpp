#include "exe.h"
#include "methods.h"
#include "parser.h"

#include "../memory.h"
#include "../monitor.h"
#include "../string.h"

Lisp::EAST::Node* execNode(Lisp::EAST::Node* n);

Lisp::EAST::Node* convertValue(Lisp::AST::Value* v) {
	if (v == nullptr) {
		return nullptr;
	}
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	switch (v->t) {
	case Lisp::AST::Type::INT:
	{
		ret->type = Lisp::EAST::INT;
		Lisp::AST::Span* span = (Lisp::AST::Span*)v->data;
		i8* buf = Memory::kmalloc<i8>(span->end - span->start + 1);
		memcpy(span->start, buf, span->end - span->start);
		buf[span->end - span->start] = '\0';
		ret->data.num = Types::String::toUint(buf);
		Memory::kfree(buf);
		break;
	}
	case Lisp::AST::Type::QUOTE_LIST:
	{
		Lisp::AST::ListContents* lc = (Lisp::AST::ListContents*)v->data;

		Lisp::EAST::Node* node = ret;
		while (lc) {
			node->type = Lisp::EAST::Type::QUOTE_PAIR;
			node->data.pair.p1 = convertValue(lc->v);
			if (lc->n) {
				node->data.pair.p2 = Memory::kmalloc<Lisp::EAST::Node>();
				memset(node->data.pair.p2, 0, sizeof(Lisp::EAST::Node));
			}
			node = node->data.pair.p2;
			lc = lc->n;
		}
		break;
	}
	case Lisp::AST::Type::LIST:
	{
		Lisp::AST::ListContents* lc = (Lisp::AST::ListContents*)v->data;

		Lisp::EAST::Node* node = ret;
		while (lc) {
			node->type = Lisp::EAST::Type::PAIR;
			node->data.pair.p1 = convertValue(lc->v);
			if (lc->n) {
				node->data.pair.p2 = Memory::kmalloc<Lisp::EAST::Node>();
				memset(node->data.pair.p2, 0, sizeof(Lisp::EAST::Node));
			}
			node = node->data.pair.p2;
			lc = lc->n;
		}
		break;
	}
	case Lisp::AST::Type::SYMBOL:
	{
		Lisp::AST::Span* span = (Lisp::AST::Span*)v->data;
		ret->type = Lisp::EAST::Type::SYMBOL;
		ret->data.span.start = span->start;
		ret->data.span.size = span->end - span->start;
		break;
	}
	default:
		Monitor::printf("Unknown type: %d\n", (u32)v->t);
		halt();
		break;
	}
	return ret;
}

Lisp::EAST::Node* convertList(Lisp::AST::ListContents* list) {
	if (list == nullptr) {
		return nullptr;
	}
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	ret->type = Lisp::EAST::PAIR;
	ret->data.pair.p1 = convertValue(list->v);
	ret->data.pair.p2 = convertList(list->n);
	return ret;
}

Lisp::EAST::Node* execFunction(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	Lisp::EAST::Node* symbol = n->data.pair.p1;
	Lisp::EAST::Node* args = n->data.pair.p2;
	if (strcmpSpan("+", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::add(args, scope);
	} else if (strcmpSpan("-", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::sub(args, scope);
	} else if (strcmpSpan("*", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::mul(args, scope);
	} else if (strcmpSpan("/", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::div(args, scope);
	} else if (strcmpSpan("if", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::ifExpr(args, scope);
	} else if (strcmpSpan("=", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::eq(args, scope);
	} else if (strcmpSpan("define", symbol->data.span.start, symbol->data.span.size) == 0) {
		return Lisp::Methods::define(args, scope);
	}
	return nullptr;
}

Lisp::EAST::Node* Lisp::execNode(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	if (n == nullptr) {
		return nullptr;
	}
	switch (n->type) {
	case Lisp::EAST::Type::INT:
		return n;
	case Lisp::EAST::Type::QUOTE_PAIR:
		return n;
	case Lisp::EAST::Type::PAIR:
		assert(n->data.pair.p1->type == Lisp::EAST::Type::SYMBOL, "execNode: Not a function");
		return execFunction(n, scope);
	case Lisp::EAST::Type::SYMBOL:
	{
		if (strcmpSpan("#t", n->data.span.start, n->data.span.size) == 0) {
			return n;
		} else if (strcmpSpan("#f", n->data.span.start, n->data.span.size) == 0) {
			return n;
		}
		i8* nameBuf = Memory::kmalloc<i8>(n->data.span.size + 1);
		memcpy(n->data.span.start, nameBuf, n->data.span.size);
		nameBuf[n->data.span.size] = '\0';
		auto r = scope->lookup(nameBuf);
		assert(r, "Lisp::execNode: unknown symbol");
		Memory::kfree(nameBuf);
		return r;
		break;
	}
	default:
		Monitor::writeString("exe: bad type\n");
		break;
	}
	return nullptr;
}

void output(Lisp::EAST::Node* n) {
	if (n == nullptr) {
		Monitor::writeString("NULL");
		return;
	}
	switch (n->type) {
	case Lisp::EAST::Type::INT:
		Monitor::printf("%d", n->data.num);
		break;
	case Lisp::EAST::Type::PAIR:
		Monitor::writeChar('(');
		output(n->data.pair.p1);
		Monitor::writeString(" . ");
		output(n->data.pair.p2);
		Monitor::writeChar(')');
		break;
	case Lisp::EAST::Type::QUOTE_PAIR:
		Monitor::writeString("'(");
		output(n->data.pair.p1);
		Monitor::writeString(" . ");
		output(n->data.pair.p2);
		Monitor::writeChar(')');
		break;
	case Lisp::EAST::Type::PATH:
	case Lisp::EAST::Type::SYMBOL:
	{
		i8* buf = Memory::kmalloc<i8>(n->data.span.size + 1);
		memcpy(n->data.span.start, buf, n->data.span.size);
		buf[n->data.span.size] = '\0';
		Monitor::writeString(buf);
		Memory::kfree(buf);
		break;
	}
	case Lisp::EAST::Type::QUOTE_SYMBOL:
	{
		Monitor::writeChar('\'');
		i8* buf = Memory::kmalloc<i8>(n->data.span.size + 1);
		memcpy(n->data.span.start, buf, n->data.span.size);
		buf[n->data.span.size] = '\0';
		Monitor::writeString(buf);
		Memory::kfree(buf);
		break;
	}
	case Lisp::EAST::Type::STRING:
	{
		i8* buf = Memory::kmalloc<i8>(n->data.span.size + 1);
		memcpy(n->data.span.start, buf, n->data.span.size);
		buf[n->data.span.size] = '\0';
		Monitor::printf("\"%s\"", buf);
		Memory::kfree(buf);
		break;
	}
	default:
		Monitor::printf("Unknown output type: %d\n", (u32)n->type);
		break;
	}
}

void Lisp::EAST::Scope::exec(Lisp::AST::ListContents* l) {
	Lisp::EAST::Node* c = convertList(l);
	while (c) {
		Lisp::EAST::Node* res = execNode(c->data.pair.p1, this);
		output(res);
		Monitor::writeChar('\n');
		c = c->data.pair.p2;
	}
	return;
}

void Lisp::exe(Lisp::AST::ListContents* l) {
	Lisp::EAST::Scope scope;
	scope.init();
	scope.exec(l);
	return;
}

void Lisp::EAST::Scope::init() {
	level = 0;
	used = 0;

	data = Memory::kmalloc<Lisp::EAST::Scope::Entry>(256);
}

void Lisp::EAST::Scope::add(i8 const* name, Node* node) {
	if (used == 256) {
		// TODO: Grow scope
		return;
	}
	i8* buf = Memory::kmalloc<i8>(strlen(name) + 1);
	memcpy(name, buf, strlen(name));
	buf[strlen(name)] = '\0';
	for (u32 i = 0; i < used; i++) {
		if (data[i].level == level && strcmp(buf, data[i].name) == 0) {
			data[i].node = node;
			return;
		}
	}
	data[used].name = buf;
	data[used].node = node;
	data[used].level = level;
	used++;
	return;
}

void Lisp::EAST::Scope::delIdx(u32 i) {
	Memory::kfree(data[i].name);
	memcpy(data+i+1, data+i, used - i);
	used--;
}

void Lisp::EAST::Scope::del(i8 const* name) {
	u32 idx = 999;
	for (u32 i = 0; i < used; i++) {
		if (strcmp(name, data[i].name) != 0) {
			continue;
		}
		idx = i;
		break;
	}
	if (idx == 999) {
		return;
	}
	delIdx(idx);
}

void Lisp::EAST::Scope::in() {
	level++;
}

void Lisp::EAST::Scope::out() {
	for (u32 i = 0; i < used; i++) {
		if (data[i].level >= level) {
			// Item goes out of scope
			delIdx(i);
			i--;
		}
	}
	level--;
}

Lisp::EAST::Node* Lisp::EAST::Scope::lookup(i8 const* name) {
	bool found = false;
	u32 nearestIdx = 0;
	u32 nearestLevel = 0;
	for (u32 i = 0; i < used; i++) {
		if (strcmp(name, data[i].name) != 0) {
			continue;
		}
		if (!found || data[i].level > nearestLevel) {
			nearestLevel = data[i].level;
			nearestIdx = i;
		}
		found = true;
	}
	if (!found) {
		return nullptr;
	}
	return data[nearestIdx].node;
}
