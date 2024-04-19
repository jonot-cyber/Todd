#ifndef EXE_H
#define EXE_H

#include "parser.h"
#include "scope.h"

enum ASTType {
	AST_PAIR,
	AST_QUOTE_PAIR,
	AST_SYMBOL,
	AST_QUOTE_SYMBOL,
	AST_INT,
	AST_PATH,
	AST_STRING,
	AST_METHOD,
};

/* Types of executables:
   a lambda
     parameters & pointer to a body node
   a defined method
     parameters & pointer to a body node
   a built-in method
     parameters & function pointer
*/

struct ASTNode {
	enum ASTType type;
	union {
		struct {
			struct ASTNode* params;
			struct ASTNode* node;
			struct ASTNode* (*method)(struct ASTNode*, struct Scope*);
		} method;
		struct {
			struct ASTNode* p1;
			struct ASTNode* p2;
		} pair;
		const char *span;
		unsigned num;
	} data;
	/* Whether the node can be found from the root in the gc sweep */
	int gc_mark;
};

struct ASTNode* exec_node(struct Scope*, struct ASTNode*);

void scope_exec(struct Scope*, struct ParserListContents*);

void output(struct ASTNode*);

#endif
