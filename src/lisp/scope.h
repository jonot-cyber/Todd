#ifndef SCOPE_H
#define SCOPE_H

#include "common.h"

// Implements hash function created by Peter J. Weinberger. See
// https://en.wikipedia.org/wiki/PJW_hash_function
u32 pjw_hash(u8*, u32);

struct Scope;

struct ScopeEntry {
	i8* name;
	struct ASTNode* (*method)(struct ASTNode*, struct Scope*);
	struct ASTNode* node;
	struct ASTNode* params;
	u32 level;
	struct ScopeEntry* next;
};

struct Scope {
	struct ScopeEntry** data;
	u32 level;
};

void scope_init(struct Scope*);
void scope_add(struct Scope*, struct ScopeEntry*);
void scope_del(struct Scope*, i8 const*);
void scope_in(struct Scope*);
void scope_out(struct Scope*);
struct ScopeEntry* scope_lookup(struct Scope*, i8 const*);

#endif
