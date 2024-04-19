#ifndef SCOPE_H
#define SCOPE_H

#include "array.h"

// Implements hash function created by Peter J. Weinberger. See
// https://en.wikipedia.org/wiki/PJW_hash_function
unsigned pjw_hash(unsigned char *, unsigned);

struct Scope;

struct ScopeEntry {
	char *name;
	struct ASTNode* node;
	unsigned level;
	struct ScopeEntry* next;
};

struct Scope {
	struct ScopeEntry** data;
	struct Array nodes;
	unsigned level;
};

struct ASTNode* scope_kmalloc(struct Scope*);
void scope_init(struct Scope*);
void scope_add(struct Scope*, struct ScopeEntry*);
void scope_del(struct Scope*, char const*);
void scope_in(struct Scope*);
void scope_out(struct Scope*);
struct ScopeEntry* scope_lookup(struct Scope*, char const*);
void scope_gc(struct Scope*);

#endif
