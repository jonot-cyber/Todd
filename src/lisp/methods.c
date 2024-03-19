#include "methods.h"

#include "exe.h"
#include "memory.h"
#include "io.h"
#include "scope.h"
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
	struct ASTNode* if_true = n->data.pair.p2->data.pair.p1;
	struct ASTNode* if_false = n->data.pair.p2->data.pair.p2->data.pair.p1;

	struct ASTNode* condition_res = exec_node(scope, condition);
	assert(condition_res->type == AST_SYMBOL, "methods_if: Not a boolean");
	if (strcmp("#t", condition_res->data.span) == 0) {
		return exec_node(scope, if_true);
	} else {
		return exec_node(scope, if_false);
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
		i8* buf = kmalloc(len+1);
		memcpy(p1->data.span, buf, len);
		buf[len] = '\0';
		struct ScopeEntry* new_entry = kmalloc_z(sizeof(struct ScopeEntry));
		new_entry->level = 0;
		new_entry->name = buf;
		new_entry->node = ret;
		scope_add(scope, new_entry);
		return ret;
	} else if (p1->type == AST_PAIR) {
		struct ASTNode* name = p1->data.pair.p1;
		struct ASTNode* params = p1->data.pair.p2;
		assert(name->type == AST_SYMBOL, "methods_define: Function name must be a symbol");
		struct ASTNode* exe = kmalloc_z(sizeof(struct ASTNode));
		exe->type = AST_METHOD;
		exe->data.method.params = params;
		exe->data.method.node = p2;
		u32 name_len = strlen(name->data.span);
		i8* buf = kmalloc_z(name_len + 1);
		memcpy(name->data.span, buf, name_len);
		struct ScopeEntry* new_entry = kmalloc_z(sizeof(struct ScopeEntry));
		new_entry->name = buf;
		new_entry->node = exe;
		scope_add(scope, new_entry);
		kfree(new_entry);
		return NULL;
	} else {
		assert(false, "methods_define: Incorrect syntax for define");
		return NULL;
	}
}

struct ASTNode* method_display(struct ASTNode* args, struct Scope* scope) {
	UNUSED(scope);
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

/*
  ((lambda (x) (* x 2)) 3) ; => 6
 */
struct ASTNode* method_lambda(struct ASTNode* args, struct Scope* scope) {
	UNUSED(scope);
	assert(args->type == AST_PAIR, "method_lambda: Invalid syntax");
	struct ASTNode* params = args->data.pair.p1;
	assert(params->type == AST_PAIR, "method_lambda: Invalid syntax [b]");
	assert(args->data.pair.p2->type == AST_PAIR, "method_lambda: Invalid syntax [c]");
	struct ASTNode* body = args->data.pair.p2->data.pair.p1;

	struct ASTNode* ret = kmalloc_z(sizeof(struct ASTNode));
	ret->type = AST_METHOD;
	ret->data.method.params = params;
	ret->data.method.node = body;
	return ret;
}
