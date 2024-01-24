#include "methods.h"

#include "memory.h"
#include "exe.h"
#include "string.h"

const i8* TRUE_SYMBOL = "#t";
const i8* FALSE_SYMBOL = "#f";

struct ASTNode* method_add(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_INT;
	ret->data.num = 0;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		assert(tmp->type == AST_INT, "method_add: Parameter must be integer");
		ret->data.num += tmp->data.num;
		n = n->data.pair.p2;
	}
	return ret;
}


struct ASTNode* method_sub(struct ASTNode* n, struct Scope* scope) {
	bool first = true;
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_INT;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		assert(tmp->type == AST_INT, "execFunction: Parameter must be integer");
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

struct ASTNode* method_mul(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_INT;
	ret->data.num = 1;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		assert(tmp->type == AST_INT, "execFunction: Parameter must be integer");
		ret->data.num *= tmp->data.num;
		n = n->data.pair.p2;
	}
	return ret;
}

struct ASTNode* method_div(struct ASTNode* n, struct Scope* scope) {
	bool first = true;
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_INT;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		assert(tmp->type == AST_INT, "execFunction: Parameter must be integer");
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

struct ASTNode* method_if(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* condition = n->data.pair.p1;
	struct ASTNode* ifTrue = n->data.pair.p2->data.pair.p1;
	struct ASTNode* ifFalse = n->data.pair.p2->data.pair.p2;

	struct ASTNode* conditionRes = exec_node(scope, condition);
	assert(conditionRes->type == AST_SYMBOL, "methods_if: Not a boolean");
	if (strcmp_span("#t", conditionRes->data.span.start, conditionRes->data.span.size) == 0) {
		return exec_node(scope, ifTrue);
	} else {
		return exec_node(scope, ifFalse);
	}
}


struct ASTNode* method_eq(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* p1 = n->data.pair.p1;
	struct ASTNode* p2 = n->data.pair.p2->data.pair.p1;
	p1 = exec_node(scope, p1);
	p2 = exec_node(scope, p2);

	// TODO: Implement non ints
	assert(p1->type == p2->type && p1->type == AST_INT,
	       "methods_eq: Can only compare ints at the moment");
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_SYMBOL;
	ret->data.span.size = 2;
	if (p1->data.num == p2->data.num) {
		ret->data.span.start = TRUE_SYMBOL;
	} else {
		ret->data.span.start = FALSE_SYMBOL;
	}
	return ret;
}

struct ASTNode* method_define(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* p1 = n->data.pair.p1;
	struct ASTNode* p2 = n->data.pair.p2->data.pair.p1;

	// TODO: Functions
	assert(p1->type == AST_SYMBOL, "methods_define: Can only define a symbol as a name");

	struct ASTNode* ret = exec_node(scope, p2);
	i8* buf = kmalloc(p1->data.span.size * sizeof(i8));
	memcpy(p1->data.span.start, buf, p1->data.span.size);
	buf[p1->data.span.size] = '\0';
	scope_add(scope, buf, ret);
	kfree(buf);
	return ret;
}
