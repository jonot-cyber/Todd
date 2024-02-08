#ifndef METHODS_H
#define METHODS_H

#include "exe.h"
#include "scope.h"

struct ASTNode* method_add(struct ASTNode*, struct Scope*);
struct ASTNode* method_sub(struct ASTNode*, struct Scope*);
struct ASTNode* method_mul(struct ASTNode*, struct Scope*);
struct ASTNode* method_div(struct ASTNode*, struct Scope*);
struct ASTNode* method_if(struct ASTNode*, struct Scope*);
struct ASTNode* method_eq(struct ASTNode*, struct Scope*);
struct ASTNode* method_define(struct ASTNode*, struct Scope*);
struct ASTNode* method_display(struct ASTNode*, struct Scope*);
struct ASTNode* method_and(struct ASTNode*, struct Scope*);
struct ASTNode* method_or(struct ASTNode*, struct Scope*);
struct ASTNode* method_not(struct ASTNode*, struct Scope*);
struct ASTNode* method_call_cc(struct ASTNode*, struct Scope*);

#endif
