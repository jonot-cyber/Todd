#include "file.h"
#include "stdio.h"
#include "string.h"
#include "system.h"

#include "methods.h"

#include "exe.h"
#include "scope.h"

/* Propogate null value */
#define PNULL(c) if ((c) == 0) return 0
#define TCHECK(c, t) if ((c)->type != (t)) { printf("Invalid type!\n"); return 0; }
#define CAR(n) (n)->data.pair.p1
#define CDR(n) (n)->data.pair.p2

struct ASTNode *method_add(struct ASTNode *n, struct Scope *scope) {
	PNULL(scope);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = 0;
	while (n) {
		TCHECK(n, AST_PAIR);
		struct ASTNode *tmp = exec_node(scope, CAR(n));
		PNULL(tmp);
		TCHECK(tmp, AST_INT);
		ret->data.num += tmp->data.num;
		n = CDR(n);
	}
	return ret;
}


struct ASTNode *method_sub(struct ASTNode *n, struct Scope *scope) {
	PNULL(scope);
	int first = 1;
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	PNULL(n);
	while (n) {
		TCHECK(n, AST_PAIR);
		struct ASTNode *tmp = exec_node(scope, CAR(n));
		TCHECK(tmp, AST_INT);
		if (first)
			ret->data.num = tmp->data.num;
		else
			ret->data.num -= tmp->data.num;
		first = 0;
		n = CDR(n);
	}
	return ret;
}

struct ASTNode *method_mul(struct ASTNode *n, struct Scope *scope) {
	PNULL(scope);
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = 1;
	while (n) {
		TCHECK(n, AST_PAIR);
		struct ASTNode* tmp = exec_node(scope, CAR(n));
		TCHECK(tmp, AST_INT);
		ret->data.num *= tmp->data.num;
		n = CDR(n);
	}
	return ret;
}

struct ASTNode *method_div(struct ASTNode *n, struct Scope *scope) {
	int first = 1;
	PNULL(scope);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	PNULL(n);
	while (n) {
		TCHECK(n, AST_PAIR);
		struct ASTNode *tmp = exec_node(scope, CAR(n));
		TCHECK(tmp, AST_INT);
		if (first)
			ret->data.num = tmp->data.num;
		else
			ret->data.num /= tmp->data.num;
		first = 0;
		n = CDR(n);
	}
	return ret;
}

struct ASTNode *method_if(struct ASTNode *n, struct Scope *scope) {
	PNULL(n);
	TCHECK(n, AST_PAIR);
	struct ASTNode *condition = CAR(n);
	PNULL(condition);
	PNULL(CDR(n));
	TCHECK(CDR(n), AST_PAIR);
	struct ASTNode *if_true = CAR(CDR(n));
	PNULL(CDR(CDR(n)));
	TCHECK(CDR(CDR(n)), AST_PAIR);
	struct ASTNode *if_false = CAR(CDR(CDR(n)));

	struct ASTNode *condition_res = exec_node(scope, condition);
	PNULL(condition_res);
	TCHECK(condition_res, AST_INT);
	return exec_node(scope, condition_res->data.num ? if_true : if_false);
}


struct ASTNode *method_eq(struct ASTNode *n, struct Scope *scope) {
	PNULL(n);
	TCHECK(n, AST_PAIR);
	struct ASTNode *p1 = n->data.pair.p1;
	TCHECK(CDR(n), AST_PAIR);
	struct ASTNode *p2 = n->data.pair.p2->data.pair.p1;
	p1 = exec_node(scope, p1);
	PNULL(p1);
	p2 = exec_node(scope, p2);
	PNULL(p2);

	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	TCHECK(p1, AST_INT);
	TCHECK(p2, AST_INT);
	ret->data.num = p1->data.num == p2->data.num;
	return ret;
}

struct ASTNode *method_define(struct ASTNode *n, struct Scope *scope) {
	PNULL(n);
	TCHECK(n, AST_PAIR);
	TCHECK(CDR(n), AST_PAIR);
	struct ASTNode *p1 = CAR(n);
	PNULL(p1);
	struct ASTNode *p2 = CAR(CDR(n));

	if (p1->type == AST_SYMBOL) {
		struct ASTNode *ret = exec_node(scope, p2);
		unsigned len = strlen(p1->data.span);
		char *buf = malloc(len+1);
		memcpy(p1->data.span, buf, len);
		buf[len] = '\0';
		struct ScopeEntry new_entry;
		new_entry.level = 0;
		new_entry.name = buf;
		new_entry.node = ret;
		scope_add(scope, &new_entry);
		free(buf);
		return ret;
	} else if (p1->type == AST_PAIR) {
		TCHECK(p1, AST_PAIR);
		struct ASTNode *name = CAR(p1);
		TCHECK(name, AST_SYMBOL);
		struct ASTNode *params = CDR(p1);
		struct ASTNode *exe = scope_kmalloc(scope);
		exe->type = AST_METHOD;
		exe->data.method.params = params;
		exe->data.method.node = p2;

		unsigned name_len = strlen(name->data.span);
		char *buf = malloc(name_len + 1);
		memcpy(name->data.span, buf, name_len);
		buf[name_len] = '\0';

		struct ScopeEntry new_entry;
		new_entry.name = buf;
		new_entry.node = exe;
		scope_add(scope, &new_entry);
		free(buf);
		return 0;
	} else {
		printf("methods_define: Incorrect syntax for define");
		return 0;
	}
}

struct ASTNode *method_display(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *param = exec_node(scope, CAR(args));
	TCHECK(param, AST_STRING);
	printf("%s\n", param->data.span);
	return 0;
}

struct ASTNode *method_and(struct ASTNode *args, struct Scope *scope) {
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	struct ASTNode *ptr = args;
	while (ptr) {
		TCHECK(ptr, AST_PAIR);
		struct ASTNode* arg = CAR(ptr);
		arg = exec_node(scope, arg);
		PNULL(arg);
		TCHECK(arg, AST_INT);
		if (!arg->data.num) {
			ret->data.num = 0;
			return ret;
		}
		ptr = CDR(ptr);
	}
	ret->data.num = 1;
	return ret;
}

struct ASTNode *method_or(struct ASTNode *args, struct Scope *scope) {
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	struct ASTNode* ptr = args;
	while (ptr) {
		TCHECK(ptr, AST_PAIR);
		struct ASTNode *arg = CAR(ptr);
		arg = exec_node(scope, arg);
		PNULL(arg);
		TCHECK(arg, AST_INT);
		if (arg->data.num) {
			ret->data.num = 1;
			return ret;
		}
		ptr = CDR(ptr);
	}
	ret->data.num = 0;
	return ret;
}

struct ASTNode *method_not(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *arg = exec_node(scope, CAR(args));
	PNULL(arg);
	TCHECK(arg, AST_INT);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = !arg->data.num;
	return ret;
}

/*
  ((lambda (x) (* x 2)) 3) ; => 6
 */
struct ASTNode *method_lambda(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *params = CAR(args);
	PNULL(params);
	TCHECK(params, AST_PAIR);
	PNULL(CDR(args));
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *body = CAR(CDR(args));

	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_METHOD;
	ret->data.method.params = params;
	ret->data.method.node = body;
	return ret;
}

struct ASTNode *method_read(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *arg = CAR(args);
	struct ASTNode *file_path = exec_node(scope, arg);
	PNULL(file_path);
	TCHECK(file_path, AST_STRING);
	struct FSNode *find_res = fopen(file_path->data.span);
	PNULL(find_res);
	char *data = malloc(find_res->size + 1);
	memcpy(find_res->data, data, find_res->size);
	data[find_res->size] = '\0';
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_STRING;
	ret->data.span = data;
	return ret;
}

/* Less than */
struct ASTNode *method_l(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *node_1 = exec_node(scope, CAR(args));
	struct ASTNode *node_2 = exec_node(scope, CAR(CDR(args)));
	PNULL(node_1);
	TCHECK(node_1, AST_INT);
	PNULL(node_2);
	TCHECK(node_2, AST_INT);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num < node_2->data.num;
	return ret;
}

/* Greater than */
struct ASTNode *method_g(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *node_1 = exec_node(scope, CAR(args));
	struct ASTNode *node_2 = exec_node(scope, CAR(CDR(args)));
	PNULL(node_1);
	TCHECK(node_1, AST_INT);
	PNULL(node_2);
	TCHECK(node_2, AST_INT);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num > node_2->data.num;
	return ret;
}

/* Less than or equal to */
struct ASTNode *method_le(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *node_1 = exec_node(scope, CAR(args));
	struct ASTNode *node_2 = exec_node(scope, CAR(CDR(args)));
	PNULL(node_1);
	TCHECK(node_1, AST_INT);
	PNULL(node_2);
	TCHECK(node_2, AST_INT);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num <= node_2->data.num;
	return ret;
}

/* Greater than or equal to */
struct ASTNode *method_ge(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *node_1 = exec_node(scope, CAR(args));
	struct ASTNode *node_2 = exec_node(scope, CAR(CDR(args)));
	PNULL(node_1);
	TCHECK(node_1, AST_INT);
	PNULL(node_2);
	TCHECK(node_2, AST_INT);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num >= node_2->data.num;
	return ret;
}

/* Not equal to */
struct ASTNode *method_ne(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *node_1 = exec_node(scope, CAR(args));
	struct ASTNode *node_2 = exec_node(scope, CAR(CDR(args)));
	PNULL(node_1);
	TCHECK(node_1, AST_INT);
	PNULL(node_2);
	TCHECK(node_2, AST_INT);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num != node_2->data.num;
	return ret;
}

struct ASTNode *method_exec(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *arg = CAR(args);
	struct ASTNode *file_path = exec_node(scope, arg);
	PNULL(file_path);
	TCHECK(file_path, AST_STRING);
	struct FSNode *find_res = fopen(file_path->data.span);
	PNULL(find_res);
	int status_code = exec(find_res->data);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = status_code;
	return ret;
}

struct ASTNode *method_string_append(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);

	unsigned buf_len = 0;
	char *buf = 0;

	struct ASTNode *args_ptr = args;
	while (args_ptr) {
		if (args_ptr->type != AST_PAIR) {
			if (buf != 0)
				free(buf);
			return 0;
		}
		TCHECK(args_ptr, AST_PAIR);
		struct ASTNode *node = exec_node(scope, CAR(args_ptr));
		if (node == 0) {
			if (buf != 0)
				free(buf);
			return 0;
		}
		if (node->type != AST_STRING) {
			if (buf != 0)
				free(buf);
			return 0;
		}

		unsigned len = strlen(node->data.span);
		char *new_buf = malloc(buf_len + len + 1);

		memcpy(buf, new_buf, buf_len);
		if (buf != 0)
			free(buf);
		memcpy(node->data.span, new_buf+buf_len, len);

		buf_len += len;

		buf = new_buf;
		args_ptr = args_ptr->data.pair.p2;
	}
	buf[buf_len] = '\0';
	struct ASTNode *node = scope_kmalloc(scope);
	node->type = AST_STRING;
	node->data.span = buf;
	return node;
}

struct ASTNode *method_let(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *bindings = CAR(args);
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *body = CAR(CDR(args));
	scope_in(scope);
	while (bindings) {
		TCHECK(bindings, AST_PAIR);
		struct ASTNode *pair = CAR(bindings);
		PNULL(pair);
		TCHECK(pair, AST_PAIR);
		struct ASTNode *pair_key = CAR(pair);
		TCHECK(CDR(pair), AST_PAIR);
		struct ASTNode *pair_value = CAR(CDR(pair));
		struct ScopeEntry entry;
		entry.node = exec_node(scope, pair_value);
		entry.name = (char *)pair_key->data.span;
		scope_add(scope, &entry);
		bindings = CDR(bindings);
	}
	struct ASTNode *res = exec_node(scope, body);
	scope_out(scope);
	return res;
}

struct ASTNode *method_apply(struct ASTNode *args, struct Scope *scope) {
	struct ASTNode *method = exec_node(scope, args->data.pair.p1);
	struct ASTNode *data = exec_node(scope, args->data.pair.p2->data.pair.p1);
//	assert(method->type == AST_METHOD, "method_apply: Method isn't a method");
//	assert(data->type == AST_QUOTE_PAIR, "method_apply: Arguments aren't arguments");
	struct ASTNode *carry = data->data.pair.p1;
	data = data->data.pair.p2;
	while (data) {
		struct ASTNode *tmp_tmp_args = scope_kmalloc(scope);
		tmp_tmp_args->type = AST_PAIR;
		tmp_tmp_args->data.pair.p1 = data->data.pair.p1;
		struct ASTNode *tmp_args = scope_kmalloc(scope);
		tmp_args->type = AST_PAIR;
		tmp_args->data.pair.p1 = carry;
		tmp_args->data.pair.p2 = tmp_tmp_args;
		struct ASTNode *method_call = scope_kmalloc(scope);
		method_call->type = AST_PAIR;
		method_call->data.pair.p1 = method;
		method_call->data.pair.p2 = tmp_args;
		carry = exec_node(scope, method_call);
		data = data->data.pair.p2;
	}
	return carry;
}

struct ASTNode *recursive_map(struct ASTNode *method, struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_PAIR;
	struct ASTNode *method_call = scope_kmalloc(scope);
	method_call->type = AST_PAIR;
	method_call->data.pair.p1 = method;
	TCHECK(args, AST_QUOTE_PAIR);

	/* Create a temporary pair to store the recursive map data */
	struct ASTNode *tmp_args = scope_kmalloc(scope);
	tmp_args->type = AST_PAIR;
	CAR(tmp_args) = CAR(args);
	method_call->data.pair.p2 = tmp_args;

	ret->data.pair.p1 = exec_node(scope, method_call);
	ret->data.pair.p2 = recursive_map(method, CDR(args), scope);
	return ret;
}

struct ASTNode *method_map(struct ASTNode *args, struct Scope *scope) {
	PNULL(args);
	TCHECK(args, AST_PAIR);
	struct ASTNode *method = exec_node(scope, CAR(args));
	TCHECK(CDR(args), AST_PAIR);
	struct ASTNode *data = exec_node(scope, CAR(CDR(args)));
	return recursive_map(method, data, scope);
}
