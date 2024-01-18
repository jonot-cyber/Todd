#include "methods.h"

#include "memory.h"
#include "exe.h"
#include "string.h"

const i8* TRUE_SYMBOL = "#t";
const i8* FALSE_SYMBOL = "#f";

Lisp::EAST::Node* Lisp::Methods::add(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	ret->type = Lisp::EAST::Type::INT;
	ret->data.num = 0;
	while (n) {
		Lisp::EAST::Node* tmp = execNode(n->data.pair.p1, scope);
		assert(tmp->type == Lisp::EAST::Type::INT, "execFunction: Parameter must be integer");
		ret->data.num += tmp->data.num;
		n = n->data.pair.p2;
	}
	return ret;
}


Lisp::EAST::Node* Lisp::Methods::sub(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	bool first = true;
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	ret->type = Lisp::EAST::Type::INT;
	while (n) {
		Lisp::EAST::Node* tmp = execNode(n->data.pair.p1, scope);
		assert(tmp->type == Lisp::EAST::Type::INT, "execFunction: Parameter must be integer");
		if (first) {
			ret->data.num = tmp->data.num;
		} else {
			ret->data.num -= tmp->data.num;
		}
		first = false;
		n = n->data.pair.p2;
	}
	return ret;
}

Lisp::EAST::Node* Lisp::Methods::mul(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	ret->type = Lisp::EAST::Type::INT;
	ret->data.num = 1;
	while (n) {
		Lisp::EAST::Node* tmp = execNode(n->data.pair.p1, scope);
		assert(tmp->type == Lisp::EAST::Type::INT, "execFunction: Parameter must be integer");
		ret->data.num *= tmp->data.num;
		n = n->data.pair.p2;
	}
	return ret;
}

Lisp::EAST::Node* Lisp::Methods::div(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	bool first = true;
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	ret->type = Lisp::EAST::Type::INT;
	while (n) {
		Lisp::EAST::Node* tmp = execNode(n->data.pair.p1, scope);
		assert(tmp->type == Lisp::EAST::Type::INT, "execFunction: Parameter must be integer");
		if (first) {
			ret->data.num = tmp->data.num;
		} else {
			ret->data.num /= tmp->data.num;
		}
		first = false;
		n = n->data.pair.p2;
	}
	return ret;
}

Lisp::EAST::Node* Lisp::Methods::ifExpr(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	Lisp::EAST::Node* condition = n->data.pair.p1;
	Lisp::EAST::Node* ifTrue = n->data.pair.p2->data.pair.p1;
	Lisp::EAST::Node* ifFalse = n->data.pair.p2->data.pair.p2;

	Lisp::EAST::Node* conditionRes = execNode(condition, scope);
	assert(conditionRes->type == Lisp::EAST::Type::SYMBOL, "Lisp::Methods::ifExpr: Not a boolean");
	if (strcmpSpan("#t", conditionRes->data.span.start, conditionRes->data.span.size) == 0) {
		return execNode(ifTrue, scope);
	} else {
		return execNode(ifFalse, scope);
	}
}


Lisp::EAST::Node* Lisp::Methods::eq(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	Lisp::EAST::Node* p1 = n->data.pair.p1;
	Lisp::EAST::Node* p2 = n->data.pair.p2->data.pair.p1;
	p1 = execNode(p1, scope);
	p2 = execNode(p2, scope);

	// TODO: Implement non ints
	assert(p1->type == p2->type && p1->type == Lisp::EAST::Type::INT,
	       "Lisp::Methods::eq: Can only compare ints at the moment");
	Lisp::EAST::Node* ret = Memory::kmalloc<Lisp::EAST::Node>();
	ret->type = Lisp::EAST::Type::SYMBOL;
	ret->data.span.size = 2;
	if (p1->data.num == p2->data.num) {
		ret->data.span.start = TRUE_SYMBOL;
	} else {
		ret->data.span.start = FALSE_SYMBOL;
	}
	return ret;
}

Lisp::EAST::Node* Lisp::Methods::define(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope) {
	Lisp::EAST::Node* p1 = n->data.pair.p1;
	Lisp::EAST::Node* p2 = n->data.pair.p2->data.pair.p1;

	// TODO: Functions
	assert(p1->type == Lisp::EAST::Type::SYMBOL, "Lisp::Methods::define: Can only define a symbol as a name");

	auto ret = execNode(p2, scope);
	i8* buf = Memory::kmalloc<i8>(p1->data.span.size);
	memcpy(p1->data.span.start, buf, p1->data.span.size);
	buf[p1->data.span.size] = '\0';
	scope->add(buf, ret);
	Memory::kfree(buf);
	return ret;
}
