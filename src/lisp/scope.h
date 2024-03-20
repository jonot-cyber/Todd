#ifndef SCOPE_H
#define SCOPE_H

#include "array.h"

// Implements hash function created by Peter J. Weinberger. See
// https://en.wikipedia.org/wiki/PJW_hash_function
u32 pjw_hash(u8*, u32);

struct Scope;

struct ScopeEntry {
	i8* name;
	struct ASTNode* node;
	u32 level;
	struct ScopeEntry* next;
};

struct Scope {
	struct ScopeEntry** data;
	struct Array nodes;
	u32 level;
};

struct ASTNode* scope_kmalloc(struct Scope*);
void scope_init(struct Scope*);
void scope_add(struct Scope*, struct ScopeEntry*);
void scope_del(struct Scope*, i8 const*);
void scope_in(struct Scope*);
void scope_out(struct Scope*);
struct ScopeEntry* scope_lookup(struct Scope*, i8 const*);
void scope_gc(struct Scope*);

#endif
