#include "exe.h"
#include "parser.h"

#include "../memory.h"
#include "../monitor.h"
#include "../string.h"
#include "scope.h"

struct ASTNode* execNode(struct ASTNode* n);

struct ASTNode* convertValue(struct ParserValue* v) {
	if (v == NULL) {
		return NULL;
	}
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	switch (v->t) {
	case P_INT:
	{
		ret->type = AST_INT;
		struct ParserSpan* span = v->data;
		i8* buf = kmalloc(span->end - span->start + 1);
		memcpy(span->start, buf, span->end - span->start);
		buf[span->end - span->start] = '\0';
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
			node->data.pair.p1 = convertValue(lc->v);
			if (lc->n) {
				node->data.pair.p2 = kmalloc(sizeof(struct ASTNode));
				memset(node->data.pair.p2, 0, sizeof(struct ASTNode));
			}
			node = node->data.pair.p2;
			lc = lc->n;
		}
		break;
	}
	case P_LIST:
	{
		struct ParserListContents* lc = v->data;

		struct ASTNode* node = ret;
		while (lc) {
			node->type = AST_PAIR;
			node->data.pair.p1 = convertValue(lc->v);
			if (lc->n) {
				node->data.pair.p2 = kmalloc(sizeof(struct ASTNode));
				memset(node->data.pair.p2, 0, sizeof(struct ASTNode));
			}
			node = node->data.pair.p2;
			lc = lc->n;
		}
		break;
	}
	case P_SYMBOL:
	{
		struct ParserSpan* span = v->data;
		i8* buf = kmalloc(span->end - span->start + 1);
		memcpy(span->start, buf, span->end - span->start);
		buf[span->end - span->start] = '\0';
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
		printf("Unknown type: %d\n", (u32)v->t);
		halt();
		break;
	}
	return ret;
}

struct ASTNode* convertList(struct ParserListContents* list) {
	if (list == NULL) {
		return NULL;
	}
	struct ASTNode* ret = kmalloc(sizeof(struct ASTNode));
	ret->type = AST_PAIR;
	ret->data.pair.p1 = convertValue(list->v);
	ret->data.pair.p2 = convertList(list->n);
	return ret;
}

void output(struct ASTNode*);

struct ASTNode* execFunction(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* symbol = n->data.pair.p1;
	struct ASTNode* args = n->data.pair.p2;
	struct ScopeEntry* entry = scope_lookup(scope, symbol->data.span);
	assert(entry != NULL, "execFunction: Function not found");
	if (entry->method) {
		// Builtin method
		return entry->method(args, scope);
	} else if (entry->params) {
		scope_in(scope);
		struct ASTNode* param_ptr = entry->params;
		struct ASTNode* arg_ptr = args;
		while (param_ptr) {
			struct ASTNode* param = param_ptr->data.pair.p1;
			assert(param->type == AST_SYMBOL, "execFunction: Parameter name isn't a symbol");
			struct ASTNode* arg_result = exec_node(scope, arg_ptr->data.pair.p1);
			struct ScopeEntry* new_entry = kmalloc(sizeof(struct ScopeEntry));
			new_entry->node = arg_result;
			new_entry->method = NULL;
			new_entry->params = NULL;
			new_entry->level = 0;
			new_entry->name = (i8*)param->data.span;
			scope_add(scope, new_entry);
			param_ptr = param_ptr->data.pair.p2;
			arg_ptr = arg_ptr->data.pair.p2;
		}
		struct ASTNode* ret = exec_node(scope, entry->node);
		scope_out(scope);
		return ret;
	}
	assert(false, "execFunction: Not a method");
	return NULL;
}

struct ASTNode* exec_node(struct Scope* scope, struct ASTNode* n) {
	if (n == NULL) {
		return NULL;
	}
	switch (n->type) {
	case AST_INT:
		return n;
	case AST_QUOTE_PAIR:
		return n;
	case AST_PAIR:
	{
		struct ASTNode* p1 = n->data.pair.p1;
		assert(p1->type == AST_SYMBOL, "exec_node: Not a function");
		return execFunction(n, scope);
	}
	case AST_SYMBOL:
	{
		if (strcmp("#t", n->data.span) == 0) {
			return n;
		} else if (strcmp("#f", n->data.span) == 0) {
			return n;
		}
		struct ScopeEntry* r = scope_lookup(scope, n->data.span);
		assert(r != NULL, "exec_node: unknown symbol");
		if (r->method != NULL || r->params != NULL) {
			// Just return methods directly;
			return n;
		}
		return exec_node(scope, r->node);
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
	struct ASTNode* c = convertList(l);
	while (c) {
		struct ASTNode* res = exec_node(scope, c->data.pair.p1);
		output(res);
		write_char('\n');
		c = c->data.pair.p2;
	}
	return;
}
