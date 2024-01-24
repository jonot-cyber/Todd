#ifndef EXE_H
#define EXE_H

#include "parser.h"

enum ASTType {
	AST_PAIR,
	AST_QUOTE_PAIR,
	AST_SYMBOL,
	AST_QUOTE_SYMBOL,
	AST_INT,
	AST_PATH,
	AST_STRING
};

struct ASTNode {
	enum ASTType type;
	union {
		struct {
			struct ASTNode* p1;
			struct ASTNode* p2;
		} pair;
		struct {
			const i8* start;
			u32 size;
		} span;
		u32 num;
	} data;
};

struct ScopeEntry {
	i8* name;
	struct ASTNode* node;
	u32 level;
};

struct Scope {
	u32 level;
	u32 used;
	struct ScopeEntry* data;
};

void scope_init(struct Scope*);
void scope_add(struct Scope*, i8 const*, struct ASTNode*);
void scope_del(struct Scope*, i8 const*);
void scope_del_index(struct Scope*, u32);
void scope_in(struct Scope*);
void scope_out(struct Scope*);
struct ASTNode* scope_lookup(struct Scope*, i8 const*);
void scope_exec(struct Scope*, struct ParserListContents*);

struct ASTNode* exec_node(struct Scope*, struct ASTNode*);

#endif
