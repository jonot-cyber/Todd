#include "methods.h"

#include "exe.h"
#include "memory.h"
#include "monitor.h"
#include "scope.h"
#include "string.h"
#include "task.h"

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
	struct ASTNode* ifFalse = n->data.pair.p2->data.pair.p2->data.pair.p1;

	struct ASTNode* conditionRes = exec_node(scope, condition);
	assert(conditionRes->type == AST_SYMBOL, "methods_if: Not a boolean");
	if (strcmp("#t", conditionRes->data.span) == 0) {
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

	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_SYMBOL;
	if (p1->type != p2->type) {
		ret->data.span = FALSE_SYMBOL;
		return ret;
	}
	if (p1->data.num == p2->data.num) {
		ret->data.span = TRUE_SYMBOL;
	} else {
		ret->data.span = FALSE_SYMBOL;
	}
	return ret;
}

struct ASTNode* method_define(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* p1 = n->data.pair.p1;
	struct ASTNode* p2 = n->data.pair.p2->data.pair.p1;

	if (p1->type == AST_SYMBOL) {
		struct ASTNode* ret = exec_node(scope, p2);
		u32 len = strlen(p1->data.span);
		i8* buf = kmalloc(len);
		memcpy(p1->data.span, buf, len);
		buf[len] = '\0';
		struct ScopeEntry* new_entry = kmalloc(sizeof(struct ScopeEntry));
		new_entry->level = 0;
		new_entry->method = NULL;
		new_entry->name = buf;
		new_entry->next = NULL;
		new_entry->node = ret;
		new_entry->params = NULL;
		scope_add(scope, new_entry);
		return ret;
	} else if (p1->type == AST_PAIR) {
		struct ASTNode* name = p1->data.pair.p1;
		struct ASTNode* params = p1->data.pair.p2;
		assert(name->type == AST_SYMBOL, "methods_define: Function name must be a symbol");
		u32 name_len = strlen(name->data.span);
		i8* buf = kmalloc(name_len + 1);
		memcpy(name->data.span, buf, name_len);
		buf[name_len] = '\0';
		struct ScopeEntry* new_entry = kmalloc(sizeof(struct ScopeEntry));
		new_entry->name = buf;
		new_entry->method = NULL;
		new_entry->node = p2;
		new_entry->params = params;
		new_entry->next = NULL;
		scope_add(scope, new_entry);
		kfree(new_entry);
		return NULL;
	} else {
		assert(false, "methods_define: Incorrect syntax for define");
		return NULL;
	}
}

struct ASTNode* method_display(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_display: weird arguments");
	struct ASTNode* param = args->data.pair.p1;
	assert(param->type == AST_STRING, "method_display: not a string");
	printf("%s\n", param->data.span);
	return NULL;
}

struct ASTNode* method_and(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_and: weird arguments");
	struct ASTNode* ptr = args;
	while (ptr) {
		struct ASTNode* arg = ptr->data.pair.p1;
		arg = exec_node(scope, arg);
		if (strcmp(arg->data.span, "#f") == 0) {
			struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
			ret->type = AST_SYMBOL;
			ret->data.span = FALSE_SYMBOL;
			return ret;
		}
		ptr = ptr->data.pair.p2;
	}
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_SYMBOL;
	ret->data.span = TRUE_SYMBOL;
	return ret;
}

struct ASTNode* method_or(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_or: weird arguments");
	struct ASTNode* ptr = args;
	while (ptr) {
		struct ASTNode* arg = ptr->data.pair.p1;
		arg = exec_node(scope, arg);
		if (strcmp(arg->data.span, "#t") == 0) {
			struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
			ret->type = AST_SYMBOL;
			ret->data.span = TRUE_SYMBOL;
			return ret;
		}
		ptr = ptr->data.pair.p2;
	}
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_SYMBOL;
	ret->data.span = FALSE_SYMBOL;
	return ret;
}

struct ASTNode* method_not(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_not: weird arguments");
	struct ASTNode* ptr = args;
	struct ASTNode* arg = exec_node(scope, ptr->data.pair.p1);
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_SYMBOL;
	assert(arg->type == AST_SYMBOL, "method_not: Needs to be a boolean");
	if (strcmp(arg->data.span, "#t") == 0) {
		ret->data.span = FALSE_SYMBOL;
	} else if (strcmp(arg->data.span, "#f") == 0) {
		ret->data.span = TRUE_SYMBOL;
	} else {
		kfree(ret);
		return NULL;
	}
	return ret;
}

struct ASTNode* method_call_cc(struct ASTNode* args, struct Scope* scope) {
	return NULL;
}
