#include "file.h"
#include "stdio.h"
#include "string.h"
#include "system.h"

#include "methods.h"

#include "exe.h"
#include "scope.h"

struct ASTNode* method_add(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = 0;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		if (tmp->type != AST_INT) {
			printf("method_add: Parameter must be an integer\n");
			return 0;
		}
		ret->data.num += tmp->data.num;
		n = n->data.pair.p2;
	}
	return ret;
}


struct ASTNode* method_sub(struct ASTNode* n, struct Scope* scope) {
	int first = 0;
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		if (tmp->type != AST_INT) {
			printf("method_sub: Parameter must be an integer\n");
			return 0;
		}
		if (first) {
			ret->data.num = tmp->data.num;
		} else {
			ret->data.num -= tmp->data.num;
		}
		first = 0;
		n = n->data.pair.p2;
	}
	return ret;
}

struct ASTNode* method_mul(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = 1;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		if (tmp->type != AST_INT) {
			printf("method_sub: Parameter must be an integer\n");
			return 0;
		}
		ret->data.num *= tmp->data.num;
		n = n->data.pair.p2;
	}
	return ret;
}

struct ASTNode* method_div(struct ASTNode* n, struct Scope* scope) {
	int first = 1;
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	while (n) {
		struct ASTNode* tmp = exec_node(scope, n->data.pair.p1);
		if (tmp->type != AST_INT) {
			printf("method_sub: Parameter must be an integer\n");
			return 0;
		}
		if (first) {
			ret->data.num = tmp->data.num;
		} else {
			ret->data.num /= tmp->data.num;
		}
		first = 0;
		n = n->data.pair.p2;
	}
	return ret;
}

struct ASTNode* method_if(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* condition = n->data.pair.p1;
	struct ASTNode* if_true = n->data.pair.p2->data.pair.p1;
	struct ASTNode* if_false = n->data.pair.p2->data.pair.p2->data.pair.p1;

	struct ASTNode* condition_res = exec_node(scope, condition);
	if (condition_res->type != AST_INT) {
		printf("method_if: Not a boolean");
		return 0;
	}
	return exec_node(scope, condition_res->data.num ? if_true : if_false);
}


struct ASTNode* method_eq(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* p1 = n->data.pair.p1;
	struct ASTNode* p2 = n->data.pair.p2->data.pair.p1;
	p1 = exec_node(scope, p1);
	p2 = exec_node(scope, p2);

	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	if (p1->type != p2->type) {
		ret->data.num = 0;
		return ret;
	}
	ret->data.num = p1->data.num == p2->data.num;
	return ret;
}

struct ASTNode* method_define(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* p1 = n->data.pair.p1;
	struct ASTNode* p2 = n->data.pair.p2->data.pair.p1;

	if (p1->type == AST_SYMBOL) {
		struct ASTNode* ret = exec_node(scope, p2);
		unsigned len = strlen(p1->data.span);
		char* buf = malloc(len+1);
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
		struct ASTNode* name = p1->data.pair.p1;
		struct ASTNode* params = p1->data.pair.p2;
		if (name->type != AST_SYMBOL) {
			printf("method_define: Function name isn't a symbol");
			return 0;
		}
		struct ASTNode* exe = scope_kmalloc(scope);
		exe->type = AST_METHOD;
		exe->data.method.params = params;
		exe->data.method.node = p2;
		unsigned name_len = strlen(name->data.span);
		char* buf = malloc(name_len + 1);
		buf[name_len] = '\0';
		memcpy(name->data.span, buf, name_len);
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

struct ASTNode* method_display(struct ASTNode* args, struct Scope* scope) {
	if (args->type != AST_PAIR) {
		printf("method_display: weird arguments\n");
		return 0;
	}
	struct ASTNode *param = exec_node(scope, args->data.pair.p1);
	if (args->type != AST_PAIR) {
		printf("method_display: not a string\n");
		return 0;
	}
	printf("%s\n", param->data.span);
	return 0;
}

struct ASTNode* method_and(struct ASTNode* args, struct Scope* scope) {
	if (args->type != AST_PAIR) {
		printf("method_and: weird arguments\n");
		return 0;
	}
	struct ASTNode* ptr = args;
	while (ptr) {
		struct ASTNode* arg = ptr->data.pair.p1;
		arg = exec_node(scope, arg);
		if (!arg->data.num) {
			struct ASTNode* ret = scope_kmalloc(scope);
			ret->type = AST_INT;
			ret->data.num = 0;
			return ret;
		}
		ptr = ptr->data.pair.p2;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = 1;
	return ret;
}

struct ASTNode* method_or(struct ASTNode* args, struct Scope* scope) {
	if (args->type != AST_PAIR) {
		printf("method_or: weird arguments\n");
		return 0;
	}
	struct ASTNode* ptr = args;
	while (ptr) {
		struct ASTNode* arg = ptr->data.pair.p1;
		arg = exec_node(scope, arg);
		if (arg->data.num) {
			struct ASTNode* ret = scope_kmalloc(scope);
			ret->type = AST_INT;
			ret->data.num = 1;
			return ret;
		}
		ptr = ptr->data.pair.p2;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = 0;
	return ret;
}

struct ASTNode* method_not(struct ASTNode* args, struct Scope* scope) {
	if (args->type != AST_PAIR) {
		printf("method_not: weird arguments\n");
		return 0;
	}
	struct ASTNode* ptr = args;
	struct ASTNode* arg = exec_node(scope, ptr->data.pair.p1);
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	if (arg->type != AST_INT) {
		printf("method_not: Needs to be a boolean\n");
		return 0;
	}
	ret->data.num = !arg->data.num;
	return ret;
}

/*
  ((lambda (x) (* x 2)) 3) ; => 6
 */
struct ASTNode* method_lambda(struct ASTNode* args, struct Scope* scope) {
	if (args->type != AST_PAIR) {
		printf("method_lambda: Invalid syntax\n");
		return 0;
	}
	struct ASTNode* params = args->data.pair.p1;
	if (params->type != AST_PAIR) {
		printf("method_lambda: Invalid syntax [b]\n");
		return 0;
	}
	if (args->data.pair.p2->type != AST_PAIR) {
		printf("method_lambda: Invalid syntax [c]\n");
		return 0;
	}
	struct ASTNode* body = args->data.pair.p2->data.pair.p1;

	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_METHOD;
	ret->data.method.params = params;
	ret->data.method.node = body;
	return ret;
}

struct ASTNode* method_read(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* arg = args->data.pair.p1;
	struct ASTNode* file_path = exec_node(scope, arg);
	if (file_path->type != AST_STRING) {
		printf("must provide a file path\n");
		return 0;
	}
	struct FSNode *find_res = fopen(file_path->data.span);
	if (find_res == 0) {
		return 0;
	}
	char* data = malloc(find_res->size + 1);
	memcpy(find_res->data, data, find_res->size);
	data[find_res->size] = '\0';
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_STRING;
	ret->data.span = data;
	return ret;
}

/* Less than */
struct ASTNode* method_l(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	if (node_1->type != AST_INT || node_2->type != AST_INT) {
		printf("method_l: Both arguments need to be an int\n");
		return 0;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num < node_2->data.num;
	return ret;
}

/* Greater than */
struct ASTNode* method_g(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	if (node_1->type != AST_INT || node_2->type != AST_INT) {
		printf("method_g: Both arguments need to be an int\n");
		return 0;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num > node_2->data.num;
	return ret;
}

/* Less than or equal to */
struct ASTNode* method_le(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	if (node_1->type != AST_INT || node_2->type != AST_INT) {
		printf("method_le: Both arguments need to be an int\n");
		return 0;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num <= node_2->data.num;
	return ret;
}

/* Greater than or equal to */
struct ASTNode* method_ge(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	if (node_1->type != AST_INT || node_2->type != AST_INT) {
		printf("method_ge: Both arguments need to be an int\n");
		return 0;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num >= node_2->data.num;
	return ret;
}

/* Not equal to */
struct ASTNode* method_ne(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	if (node_1->type != AST_INT || node_2->type != AST_INT) {
		printf("method_ne: Both arguments need to be an int\n");
		return 0;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num != node_2->data.num;
	return ret;
}

struct ASTNode* method_exec(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* arg = args->data.pair.p1;
	struct ASTNode* file_path = exec_node(scope, arg);
	if (file_path->type != AST_STRING) {
		printf("method_exec: Must provide a file path\n");
		return 0;
	}
	struct FSNode *find_res = fopen(file_path->data.span);
	if (find_res == 0) {
		printf("Coudln't find that executable\n");
		return 0;
	}
	int status_code = exec(find_res->data);
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = status_code;
	return ret;
}

struct ASTNode *method_string_append(struct ASTNode *args, struct Scope *scope) {
	/* If we only have one argument, return it. We don't have to
	 * copy because we are a functional language */
	if (args->type == AST_STRING) {
		return args;
	}
	if (args->type != AST_PAIR) {
		printf("method_string_append: Invalid arguments\n");
		return 0;
	}

	unsigned buf_len = 0;
	char *buf = 0;

	struct ASTNode *args_ptr = args;
	while (args_ptr) {
		struct ASTNode *node = exec_node(scope, args_ptr->data.pair.p1);
		if (node->type != AST_STRING) {
			printf("method_string_append: Not a string\n");
			return 0;
		}

		unsigned len = strlen(node->data.span);
		char *new_buf = malloc(buf_len + len + 1);

		memcpy(buf, new_buf, buf_len);
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
	struct ASTNode *bindings = args->data.pair.p1;
	struct ASTNode *body = args->data.pair.p2->data.pair.p1;
	scope_in(scope);
	while (bindings) {
		struct ASTNode *pair = bindings->data.pair.p1;
		struct ASTNode *pair_key = pair->data.pair.p1;
		struct ASTNode *pair_value = pair->data.pair.p2->data.pair.p1;
		struct ScopeEntry *entry = malloc(sizeof(struct ScopeEntry));
		memset(entry, 0, sizeof(struct ScopeEntry));
		entry->node = exec_node(scope, pair_value);
		entry->name = (char*)pair_key->data.span;
		scope_add(scope, entry);
		free(entry);
		bindings = bindings->data.pair.p2;
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
	if (args == 0)
		return 0;
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_PAIR;
	struct ASTNode *method_call = scope_kmalloc(scope);
	method_call->type = AST_PAIR;
	method_call->data.pair.p1 = method;
	method_call->data.pair.p2 = args->data.pair.p1;
	ret->data.pair.p1 = exec_node(scope, method_call);
	ret->data.pair.p2 = recursive_map(method, args->data.pair.p2, scope);
	return ret;
}

struct ASTNode *method_map(struct ASTNode *args, struct Scope *scope) {
	struct ASTNode *method = exec_node(scope, args->data.pair.p1);
	struct ASTNode *data = exec_node(scope, args->data.pair.p2->data.pair.p1);
	return recursive_map(method, data, scope);
}
