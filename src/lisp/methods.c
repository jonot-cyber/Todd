#include "methods.h"

#include "../elf.h"
#include "exe.h"
#include "memory.h"
#include "io.h"
#include "scope.h"
#include "string.h"
#include "tar.h"

struct ASTNode* method_add(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* ret = scope_kmalloc(scope);
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
	struct ASTNode* ret = scope_kmalloc(scope);
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
	struct ASTNode* ret = scope_kmalloc(scope);
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
	struct ASTNode* ret = scope_kmalloc(scope);
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
	assert(condition_res->type == AST_INT, "methods_if: Not a boolean");
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
		ret->data.num = false;
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
		u32 len = strlen(p1->data.span);
		i8* buf = kmalloc(len+1);
		memcpy(p1->data.span, buf, len);
		buf[len] = '\0';
		struct ScopeEntry new_entry;
		new_entry.level = 0;
		new_entry.name = buf;
		new_entry.node = ret;
		scope_add(scope, &new_entry);
		kfree(buf);
		return ret;
	} else if (p1->type == AST_PAIR) {
		struct ASTNode* name = p1->data.pair.p1;
		struct ASTNode* params = p1->data.pair.p2;
		assert(name->type == AST_SYMBOL, "methods_define: Function name must be a symbol");
		struct ASTNode* exe = scope_kmalloc(scope);
		exe->type = AST_METHOD;
		exe->data.method.params = params;
		exe->data.method.node = p2;
		u32 name_len = strlen(name->data.span);
		i8* buf = kmalloc_z(name_len + 1);
		memcpy(name->data.span, buf, name_len);
		struct ScopeEntry new_entry;
		new_entry.name = buf;
		new_entry.node = exe;
		scope_add(scope, &new_entry);
		kfree(buf);
		return NULL;
	} else {
		assert(false, "methods_define: Incorrect syntax for define");
		return NULL;
	}
}

struct ASTNode* method_display(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_display: weird arguments");
	struct ASTNode *param = exec_node(scope, args->data.pair.p1);
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
		if (!arg->data.num) {
			struct ASTNode* ret = scope_kmalloc(scope);
			ret->type = AST_INT;
			ret->data.num = false;
			return ret;
		}
		ptr = ptr->data.pair.p2;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = true;
	return ret;
}

struct ASTNode* method_or(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_or: weird arguments");
	struct ASTNode* ptr = args;
	while (ptr) {
		struct ASTNode* arg = ptr->data.pair.p1;
		arg = exec_node(scope, arg);
		if (arg->data.num) {
			struct ASTNode* ret = scope_kmalloc(scope);
			ret->type = AST_INT;
			ret->data.num = true;
			return ret;
		}
		ptr = ptr->data.pair.p2;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = false;
	return ret;
}

struct ASTNode* method_not(struct ASTNode* args, struct Scope* scope) {
	assert(args->type == AST_PAIR, "method_not: weird arguments");
	struct ASTNode* ptr = args;
	struct ASTNode* arg = exec_node(scope, ptr->data.pair.p1);
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	assert(arg->type == AST_INT, "method_not: Needs to be a boolean");
	ret->data.num = !arg->data.num;
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

	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_METHOD;
	ret->data.method.params = params;
	ret->data.method.node = body;
	return ret;
}

struct ASTNode* method_read(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* arg = args->data.pair.p1;
	struct ASTNode* file_path = exec_node(scope, arg);
	assert(file_path->type == AST_STRING, "must provide a file path");
	struct FSNode* ptr = fs_root;
	struct FSNode* find_res = tar_find_file(ptr, file_path->data.span);
	if (find_res == NULL) {
		struct ASTNode* fail_ret = scope_kmalloc(scope);
		fail_ret->type = AST_INT;
		fail_ret->data.num = 0;
		return fail_ret;
	}
	i8* data = kmalloc(ptr->file_size + 1);
	memcpy(ptr->data, data, ptr->file_size);
	data[ptr->file_size] = '\0';
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_STRING;
	ret->data.span = data;
	return ret;
}

/* Less than */
struct ASTNode* method_l(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	assert(node_1->type == AST_INT, "method_l: First argument isn't an int");
	assert(node_2->type == AST_INT, "method_l: Second argument isn't an int");
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num < node_2->data.num;
	return ret;
}

/* Greater than */
struct ASTNode* method_g(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	assert(node_1->type == AST_INT, "method_g: First argument isn't an int");
	assert(node_2->type == AST_INT, "method_g: Second argument isn't an int");
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num > node_2->data.num;
	return ret;
}

/* Less than or equal to */
struct ASTNode* method_le(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	assert(node_1->type == AST_INT, "method_le: First argument isn't an int");
	assert(node_2->type == AST_INT, "method_le: Second argument isn't an int");
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num <= node_2->data.num;
	return ret;
}

/* Greater than or equal to */
struct ASTNode* method_ge(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	assert(node_1->type == AST_INT, "method_ge: First argument isn't an int");
	assert(node_2->type == AST_INT, "method_ge: Second argument isn't an int");
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num >= node_2->data.num;
	return ret;
}

/* Not equal to */
struct ASTNode* method_ne(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* node_1 = exec_node(scope, args->data.pair.p1);
	struct ASTNode* node_2 = exec_node(scope, args->data.pair.p2->data.pair.p1);
	assert(node_1->type == AST_INT, "method_ne: First argument isn't an int");
	assert(node_2->type == AST_INT, "method_ne: Second argument isn't an int");
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_INT;
	ret->data.num = node_1->data.num != node_2->data.num;
	return ret;
}

struct ASTNode* method_lines(struct ASTNode* args, struct Scope* scope) {
	/* TODO: Finish this method */
	struct ASTNode* str = exec_node(scope, args->data.pair.p1);
	assert(str->type == AST_STRING, "method_lines: Invalid argument");
	i8* start_ptr = (i8*)str->data.span;
	i8* end_ptr = (i8*)str->data.span;
	while (true) {
		if (*end_ptr == '\0') {
			break;
		} else if (*end_ptr == '\n') {
			i8* buf = kmalloc(end_ptr - start_ptr + 1);
			memcpy(start_ptr, buf, end_ptr - start_ptr);
			buf[end_ptr - start_ptr] = '\0';
			start_ptr = end_ptr;
		}
		end_ptr++;
	}
	return NULL;
}

struct ASTNode* method_exec(struct ASTNode* args, struct Scope* scope) {
	struct ASTNode* arg = args->data.pair.p1;
	struct ASTNode* file_path = exec_node(scope, arg);
	assert(file_path->type == AST_STRING, "must provide a file path");
	struct FSNode* ptr = fs_root;
	struct FSNode* find_res = tar_find_file(ptr, file_path->data.span);
	if (find_res == NULL) {
		printf("Coudln't find that executable\n");
		struct ASTNode* fail_ret = scope_kmalloc(scope);
		fail_ret->type = AST_INT;
		fail_ret->data.num = 1;
		return fail_ret;
	}
	i32 status_code = elf_load(find_res->data);
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
	assert(args->type == AST_PAIR, "method_string_append: Invalid arguments");

	u32 buf_len = 0;
	i8 *buf = NULL;

	struct ASTNode *args_ptr = args;
	while (args_ptr) {
		struct ASTNode *node = exec_node(scope, args_ptr->data.pair.p1);
		assert(node->type == AST_STRING, "method_string_append: Not a string");

		u32 len = strlen(node->data.span);
		i8 *new_buf = kmalloc(buf_len + len + 1);

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
		struct ScopeEntry *entry = kmalloc_z(sizeof(struct ScopeEntry));
		entry->node = exec_node(scope, pair_value);
		entry->name = (i8*)pair_key->data.span;
		scope_add(scope, entry);
		kfree(entry);
		bindings = bindings->data.pair.p2;
	}
	struct ASTNode *res = exec_node(scope, body);
	scope_out(scope);
	return res;
}

struct ASTNode *method_apply(struct ASTNode *args, struct Scope *scope) {
	struct ASTNode *method = exec_node(scope, args->data.pair.p1);
	struct ASTNode *data = exec_node(scope, args->data.pair.p2->data.pair.p1);
	assert(method->type == AST_METHOD, "method_apply: Method isn't a method");
	assert(data->type == AST_QUOTE_PAIR, "method_apply: Arguments aren't arguments");
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
	if (args == NULL)
		return NULL;
	struct ASTNode *ret = scope_kmalloc(scope);
	ret->type = AST_PAIR;
	struct ASTNode *method_call = scope_kmalloc(scope);
	method_call->type = AST_QUOTE_PAIR;
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
