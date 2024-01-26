#ifndef SCOPE_H
#define SCOPE_H

#include "common.h"

// Implements hash function created by Peter J. Weinberger. See
// https://en.wikipedia.org/wiki/PJW_hash_function
u32 pjw_hash(u8*, u32);

struct Scope2;

struct ScopeEntry2 {
	i8* name;
	struct ASTNode* (*method)(struct ASTNode*, struct Scope2*);
	struct ASTNode* node;
	struct ASTNode* params;
	u32 level;
	struct ScopeEntry2* next;
};

struct Scope2 {
	struct ScopeEntry2** data;
	u32 level;
};

void scope2_init(struct Scope2*);
void scope2_add(struct Scope2*, struct ScopeEntry2*);
void scope2_del(struct Scope2*, i8 const*);
void scope2_in(struct Scope2*);
void scope2_out(struct Scope2*);
struct ScopeEntry2* scope2_lookup(struct Scope2*, i8 const*);

#endif
