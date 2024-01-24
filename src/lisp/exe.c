#include "exe.h"
#include "methods.h"
#include "parser.h"

#include "../memory.h"
#include "../monitor.h"
#include "../string.h"

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
		ret->type = AST_SYMBOL;
		ret->data.span.start = span->start;
		ret->data.span.size = span->end - span->start;
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

struct ASTNode* execFunction(struct ASTNode* n, struct Scope* scope) {
	struct ASTNode* symbol = n->data.pair.p1;
	struct ASTNode* args = n->data.pair.p2;
	if (strcmp_span("+", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_add(args, scope);
	} else if (strcmp_span("-", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_sub(args, scope);
	} else if (strcmp_span("*", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_mul(args, scope);
	} else if (strcmp_span("/", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_div(args, scope);
	} else if (strcmp_span("if", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_if(args, scope);
	} else if (strcmp_span("=", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_eq(args, scope);
	} else if (strcmp_span("define", symbol->data.span.start, symbol->data.span.size) == 0) {
		return method_define(args, scope);
	}
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
		assert(n->data.pair.p1->type == AST_SYMBOL, "execNode: Not a function");
		return execFunction(n, scope);
	case AST_SYMBOL:
	{
		if (strcmp_span("#t", n->data.span.start, n->data.span.size) == 0) {
			return n;
		} else if (strcmp_span("#f", n->data.span.start, n->data.span.size) == 0) {
			return n;
		}
		i8* nameBuf = kmalloc(n->data.span.size + 1);
		memcpy(n->data.span.start, nameBuf, n->data.span.size);
		nameBuf[n->data.span.size] = '\0';
		struct ASTNode* r = scope_lookup(scope, nameBuf);
		assert(r != NULL, "Lisp::execNode: unknown symbol");
		kfree(nameBuf);
		return r;
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
		i8* buf = kmalloc(n->data.span.size + 1);
		memcpy(n->data.span.start, buf, n->data.span.size);
		buf[n->data.span.size] = '\0';
		write_string(buf);
		kfree(buf);
		break;
	}
	case AST_QUOTE_SYMBOL:
	{
		write_char('\'');
		i8* buf = kmalloc(n->data.span.size + 1);
		memcpy(n->data.span.start, buf, n->data.span.size);
		buf[n->data.span.size] = '\0';
		write_string(buf);
		kfree(buf);
		break;
	}
	case AST_STRING:
	{
		i8* buf = kmalloc(n->data.span.size + 1);
		memcpy(n->data.span.start, buf, n->data.span.size);
		buf[n->data.span.size] = '\0';
		printf("\"%s\"", buf);
		kfree(buf);
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

void scope_init(struct Scope* scope) {
	scope->level = 0;
	scope->used = 0;

	scope->data = kmalloc(sizeof(struct ScopeEntry) * 256);
}

void scope_add(struct Scope* scope, i8 const* name, struct ASTNode* node) {
	if (scope->used == 256) {
		// TODO: Grow scope
		return;
	}
	i8* buf = kmalloc(strlen(name) + 1);
	memcpy(name, buf, strlen(name));
	buf[strlen(name)] = '\0';
	for (u32 i = 0; i < scope->used; i++) {
		if (scope->data[i].level == scope->level && strcmp(buf, scope->data[i].name) == 0) {
			scope->data[i].node = node;
			return;
		}
	}
	scope->data[scope->used].name = buf;
	scope->data[scope->used].node = node;
	scope->data[scope->used].level = scope->level;
	scope->used++;
	return;
}

void scope_del_index(struct Scope* scope, u32 i) {
	kfree(scope->data[i].name);
	memcpy(scope->data+i+1, scope->data+i, scope->used - i);
	scope->used--;
}

void scope_del(struct Scope* scope, i8 const* name) {
	u32 idx = 999;
	for (u32 i = 0; i < scope->used; i++) {
		if (strcmp(name, scope->data[i].name) != 0) {
			continue;
		}
		idx = i;
		break;
	}
	if (idx == 999) {
		return;
	}
	scope_del_index(scope, idx);
}

void scope_in(struct Scope* scope) {
	scope->level++;
}

void scope_out(struct Scope* scope) {
	for (u32 i = 0; i < scope->used; i++) {
		if (scope->data[i].level >= scope->level) {
			// Item goes out of scope
			scope_del_index(scope, i--);
		}
	}
	scope->level--;
}

struct ASTNode* scope_lookup(struct Scope* scope, i8 const* name) {
	bool found = false;
	u32 nearestIdx = 0;
	u32 nearestLevel = 0;
	for (u32 i = 0; i < scope->used; i++) {
		if (strcmp(name, scope->data[i].name) != 0) {
			continue;
		}
		if (!found || scope->data[i].level > nearestLevel) {
			nearestLevel = scope->data[i].level;
			nearestIdx = i;
		}
		found = true;
	}
	if (!found) {
		return NULL;
	}
	return scope->data[nearestIdx].node;
}
