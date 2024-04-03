#include "exe.h"
#include "parser.h"

#include "../io.h"
#include "../memory.h"
#include "../string.h"
#include "scope.h"

struct ASTNode* convert_value(struct Scope* scope, struct ParserValue* v) {
	if (v == NULL) {
		return NULL;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	switch (v->t) {
	case P_INT:
	{
		ret->type = AST_INT;
		struct ParserSpan* span = v->data;
		const i8* start = span->start;
		const i8* end = span->end;
		kfree(span);
		i8* buf = kmalloc(end - start + 1);
		memcpy(start, buf, end - start);
		buf[end - start] = '\0';
		ret->data.num = str_to_uint(buf);
		kfree(buf);
		break;
	}
	case P_QUOTE_LIST:
	{
		struct ParserListContents* lc = v->data;

		struct ASTNode* node = ret;
		while (lc) {
			node->type = AST_QUOTE_PAIR;
			node->data.pair.p1 = convert_value(scope, lc->v);
			if (lc->n) {
				node->data.pair.p2 = scope_kmalloc(scope);
			}
			node = node->data.pair.p2;
			lc = lc->n;
		}

		struct ParserListContents* free_ptr = v->data;
		free_ptr = free_ptr->n;
		while (free_ptr) {
			struct ParserListContents* next = free_ptr->n;
			kfree(free_ptr);
			free_ptr = next;
		}
		break;
	}
	case P_LIST:
	{
		struct ParserListContents* lc = v->data;

		struct ASTNode* node = ret;
		while (lc) {
			node->type = AST_PAIR;
			node->data.pair.p1 = convert_value(scope, lc->v);
			if (lc->n) {
				node->data.pair.p2 = scope_kmalloc(scope);
			}
			node = node->data.pair.p2;
			lc = lc->n;
		}
		/* Free all the pointer stuff */
		struct ParserListContents* free_ptr = v->data;
		/* Skip one, since it will be freed below */
		free_ptr = free_ptr->n;
		while (free_ptr) {
			struct ParserListContents* next = free_ptr->n;
			kfree(free_ptr);
			free_ptr = next;
		}
		break;
	}
	case P_SYMBOL:
	{
		struct ParserSpan* span = v->data;
		const i8* start = span->start;
		const i8* end = span->end;
		//kfree(v->data);
		i8* buf = kmalloc(end - start + 1);
		memcpy(start, buf, end - start);
		buf[end - start] = '\0';
		ret->type = AST_SYMBOL;
		ret->data.span = buf;
		break;
	}
	case P_STRING:
	{
		struct ParserSpan* span = v->data;
		i8* buf = kmalloc(span->end - span->start - 1);
		memcpy(span->start + 1, buf, span->end - span->start - 2);
		buf[span->end - span->start - 2] = '\0';
		ret->type = AST_STRING;
		ret->data.span = buf;
		break;
	}
	default:
		assert(false, "convert_value: Unknown type");
		break;
	}
	kfree(v);
	return ret;
}

struct ASTNode* convert_list(struct Scope* scope, struct ParserListContents* list) {
	if (list == NULL) {
		return NULL;
	}
	struct ASTNode* ret = scope_kmalloc(scope);
	ret->type = AST_PAIR;
	ret->data.pair.p1 = convert_value(scope, list->v);
	ret->data.pair.p2 = convert_list(scope, list->n);
	return ret;
}

struct ASTNode* exec_function(struct ASTNode* method, struct ASTNode* args, struct Scope* scope) {
	assert(method->type == AST_METHOD, "exec_function: Not a symbol name");

	// Call a method implemented in C
	if (method->data.method.method) {
		struct ASTNode* ret =  method->data.method.method(args, scope);
		return ret;
	}
	scope_in(scope);
	// A pointer to the parameters
	struct ASTNode* param_ptr = method->data.method.params;
	struct ASTNode* arg_ptr = args;
	// Map the arguments to the parameters
	while (param_ptr) {
		struct ASTNode* param = param_ptr->data.pair.p1;
		assert(param != NULL, "exec_function: Null parameter");
		assert(param->type == AST_SYMBOL, "exec_function: Parameter name isn't a symbol");
		struct ASTNode* arg_result = exec_node(scope, arg_ptr->data.pair.p1);
		struct ScopeEntry new_entry;
		memset(&new_entry, 0, sizeof(struct ScopeEntry));
		new_entry.node = arg_result;
		new_entry.name = (i8*)param->data.span;
		scope_add(scope, &new_entry);
		param_ptr = param_ptr->data.pair.p2;
		arg_ptr = arg_ptr->data.pair.p2;
	}
	// Execute the function
	struct ASTNode* ret = exec_node(scope, method->data.method.node);
	scope_out(scope);
	return ret;
}

struct ASTNode* exec_node(struct Scope* scope, struct ASTNode* n) {
	if (n == NULL) {
		return NULL;
	}
	switch (n->type) {
	case AST_INT:
	case AST_QUOTE_PAIR:
	case AST_STRING:
		return n;
	case AST_PAIR:
	{
		struct ASTNode* first_node = exec_node(scope, n->data.pair.p1);
		if (first_node->type == AST_METHOD) {
			return exec_function(first_node, n->data.pair.p2, scope);
		}
		/* We don't want to evaluate a list and its children *
		 if we don't need to. For example, see this code:
		 (define (id x) x) If we weren't lazily evaluating,
		 this code wouldn't work. It would try to evaluate the
		 id method before calling define. This would fail,
		 since the id method doesn't exist until we create
		 it. Lazy evaluation makes sure constructs like this
		 work. Also helps with performace.
		 */
		return n;
	}
	case AST_SYMBOL:
	{
		struct ScopeEntry* r = scope_lookup(scope, n->data.span);
		assert(r != NULL, "exec_node: unknown symbol");
		struct ASTNode* res = exec_node(scope, r->node);
		return res;
		break;
	}
	case AST_METHOD:
	{
		return n;
		break;
	}
	default:
		write_string("exe: bad type\n");
		break;
	}
	return NULL;
}

void output(struct ASTNode* n) {
	if (n == NULL) {
		write_string("NULL");
		return;
	}
	switch (n->type) {
	case AST_INT:
		printf("%d", n->data.num);
		break;
	case AST_PAIR:
		write_char('(');
		output(n->data.pair.p1);
		write_string(" . ");
		output(n->data.pair.p2);
		write_char(')');
		break;
	case AST_QUOTE_PAIR:
		write_string("'(");
		output(n->data.pair.p1);
		write_string(" . ");
		output(n->data.pair.p2);
		write_char(')');
		break;
	case AST_PATH:
	case AST_SYMBOL:
	{
		write_string(n->data.span);
		break;
	}
	case AST_QUOTE_SYMBOL:
	{
		write_char('\'');
		write_string(n->data.span);
		break;
	}
	case AST_STRING:
	{
		printf("\"%s\"", n->data.span);
		break;
	}
	default:
		printf("Unknown output type: %d\n", (u32)n->type);
		break;
	}
}

void scope_exec(struct Scope* scope, struct ParserListContents* l) {
	struct ASTNode* c = convert_list(scope, l);
	while (c) {
		struct ASTNode* res = exec_node(scope, c->data.pair.p1);
		output(res);
		write_char('\n');
		c = c->data.pair.p2;
	}
	scope_gc(scope);
	return;
}
