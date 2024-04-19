#include "stdio.h"
#include "system.h"

#include "scope.h"

#include "exe.h"
#include "methods.h"
#include "string.h"

#define SCOPE_TABLES 512

struct ASTNode* scope_kmalloc(struct Scope* scope) {
	struct ASTNode* ret = malloc(sizeof(struct ASTNode));
	memset(ret, 0, sizeof(struct ASTNode));
	array_push(&scope->nodes, &ret);
	return ret;
}

unsigned pjw_hash(unsigned char* data, unsigned len) {
	unsigned ret = 0;
	for (unsigned i = 0; i < len; i++) {
		ret = (ret << 8) + data[i];
		unsigned char high = (ret << 28) & 0xF;
		if (high) {
			ret ^= (high >> 24);
			ret &= ~high;
		}
	}
	return ret;
}

void scope_add_builtin(struct Scope* scope, struct ASTNode* (*fn)(struct ASTNode*, struct Scope*), char* name) {
	// Create a temporary variable to store the scope entry
	struct ScopeEntry tmp;
	// Reset all the values so we don't forget any
	memset(&tmp, 0, sizeof(struct ScopeEntry));

	struct ASTNode* method_node = scope_kmalloc(scope);
	method_node->type = AST_METHOD;
	method_node->data.method.method = fn;

	tmp.node = method_node;
	tmp.name = name;
	scope_add(scope, &tmp);
}

void scope_init(struct Scope* scope) {
	/* Initialize an array of all the nodes in the scope */
	array_init(&scope->nodes, 4);

	scope->data = malloc(SCOPE_TABLES * sizeof(struct ScopeEntry*));
	memset(scope->data, 0, SCOPE_TABLES * sizeof(struct ScopeEntry *));

	scope_add_builtin(scope, method_add, "+");
	scope_add_builtin(scope, method_sub, "-");
	scope_add_builtin(scope, method_mul, "*");
	scope_add_builtin(scope, method_div, "/");
	scope_add_builtin(scope, method_if, "if");
	scope_add_builtin(scope, method_eq, "=");
	scope_add_builtin(scope, method_define, "define");
	scope_add_builtin(scope, method_display, "display");
	scope_add_builtin(scope, method_and, "and");
	scope_add_builtin(scope, method_or, "or");
	scope_add_builtin(scope, method_not, "not");
	scope_add_builtin(scope, method_lambda, "lambda");
	scope_add_builtin(scope, method_read, "read");

	scope_add_builtin(scope, method_l, "<");
	scope_add_builtin(scope, method_g, ">");
	scope_add_builtin(scope, method_le, "<=");
	scope_add_builtin(scope, method_ge, ">=");
	/* Haskell syntax is cool, and we get to use it in this :3 */
	scope_add_builtin(scope, method_ne, "/=");

	scope_add_builtin(scope, method_exec, "exec");
	scope_add_builtin(scope, method_string_append, "string-append");
	scope_add_builtin(scope, method_let, "let");
	scope_add_builtin(scope, method_apply, "apply");
	scope_add_builtin(scope, method_map, "map");
}

void scope_add(struct Scope* scope, struct ScopeEntry* entry) {
	struct ScopeEntry* entry_cpy = malloc(sizeof(struct ScopeEntry));
	memcpy(entry, entry_cpy, sizeof(struct ScopeEntry));
	unsigned name_len = strlen(entry_cpy->name);
	char* new_name_buf = malloc(name_len + 1);
	memcpy(entry_cpy->name, new_name_buf, name_len);
	new_name_buf[name_len] = '\0';
	entry_cpy->level = scope->level;
	entry_cpy->next = 0;
	unsigned bucket_id = pjw_hash((unsigned char*)entry_cpy->name, strlen(entry_cpy->name));
	entry_cpy->name = new_name_buf;
	bucket_id %= SCOPE_TABLES;
	if (scope->data[bucket_id]) {
		// TODO: Implement replacing
		entry_cpy->next = scope->data[bucket_id];
		scope->data[bucket_id] = entry_cpy;
	} else {
		scope->data[bucket_id] = entry_cpy;
		entry_cpy->next = 0;
	}
}

void scope_del(struct Scope* scope, char const* name) {
	unsigned bucket_id = pjw_hash((unsigned char*)name, strlen(name));
	bucket_id %= SCOPE_TABLES;
	struct ScopeEntry* root = scope->data[bucket_id];
	if (root == 0) {
		printf("scope_del: Attempted to delete nonexistent key\n");
	}
	if (strcmp(root->name, name) == 0) {
		// We found our key at the root
		scope->data[bucket_id] = root->next;
		free(root);
		return;
	}
	// If it's not at the root, we have to keep going
	struct ScopeEntry* prev = root;
	struct ScopeEntry* ptr = root->next;
	while (ptr) {
		if (strcmp(ptr->name, name) == 0) {
			// Delete this link
			prev->next = ptr->next;
			free(ptr);
			return;
		}
		prev = ptr;
		ptr = ptr->next;
	}
	printf("scope_del: Couldn't find key\n");
	return;
}

void scope_in(struct Scope* scope) {
	scope->level++;
}

struct ScopeEntry* scope_recursively_delete(struct ScopeEntry* entry, unsigned level) {
	if (entry == 0) {
		return 0;
	}
	if (entry->level < level) {
		return entry;
	}
	struct ScopeEntry* ret = scope_recursively_delete(entry->next, level);
	// TODO: Most of the memory in the kernel isn't getting freed
	// at all. I'm going to need to start caring about that at
	// some point.
	free(entry);
	return ret;
}

void scope_out(struct Scope* scope) {
	for (unsigned i = 0; i < SCOPE_TABLES; i++) {
		scope->data[i] = scope_recursively_delete(scope->data[i], scope->level);
	}
	scope->level--;
}

struct ScopeEntry* scope_lookup(struct Scope* scope, char const* name) {
	unsigned bucket_id = pjw_hash((unsigned char*)name, strlen(name));
	bucket_id %= SCOPE_TABLES;
	struct ScopeEntry* ptr = scope->data[bucket_id];
	while (ptr) {
		if (strcmp(ptr->name, name) == 0) {
			// We found the entry! Return.
			break;
		}
		ptr = ptr->next;
	}
	return ptr;
}

void scope_gc_mark_node(struct ASTNode* node) {
	if (!node)
		return;
	node->gc_mark = 1;
	if (node->type == AST_PAIR || node->type == AST_QUOTE_PAIR) {
		scope_gc_mark_node(node->data.pair.p1);
		scope_gc_mark_node(node->data.pair.p2);
	} else if (node->type == AST_METHOD) {
		scope_gc_mark_node(node->data.method.node);
		scope_gc_mark_node(node->data.method.params);
	}
}

void scope_gc(struct Scope* scope) {
	/* Mark */
	for (unsigned i = 0; i < SCOPE_TABLES; i++) {
		struct ScopeEntry* ptr = scope->data[i];
		while (ptr) {
			scope_gc_mark_node(ptr->node);
			ptr = ptr->next;
		}
	}
	/* Sweep */
	struct ASTNode** nodes_arr = scope->nodes.data;
	for (unsigned i = 0; i < scope->nodes.size; i++) {
		/* Get pointer to the node */
		struct ASTNode* ptr = nodes_arr[i];
		/* If a pointer hasn't been marked, we should be able to free it */
		if (!ptr->gc_mark) {
			/* If we have a span, free it */
			if (ptr->type == AST_SYMBOL
			    || ptr->type == AST_QUOTE_SYMBOL
			    || ptr->type == AST_PATH
			    || ptr->type == AST_STRING) {
				free((void*)ptr->data.span);
			}
			array_remove(&scope->nodes, i);
			i--;
			free(ptr);
			continue;
		}
		/* Reset the mark for the next run */
		ptr->gc_mark = 0;
	}
	return;
}
