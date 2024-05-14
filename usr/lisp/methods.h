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
struct ASTNode* method_lambda(struct ASTNode*, struct Scope*);
struct ASTNode* method_read(struct ASTNode*, struct Scope*);

struct ASTNode* method_l(struct ASTNode*, struct Scope*);
struct ASTNode* method_g(struct ASTNode*, struct Scope*);
struct ASTNode* method_le(struct ASTNode*, struct Scope*);
struct ASTNode* method_ge(struct ASTNode*, struct Scope*);
struct ASTNode* method_ne(struct ASTNode*, struct Scope*);

struct ASTNode* method_exec(struct ASTNode*, struct Scope*);
struct ASTNode* method_string_append(struct ASTNode*, struct Scope*);
struct ASTNode* method_let(struct ASTNode*, struct Scope*);
struct ASTNode* method_apply(struct ASTNode*, struct Scope*);
struct ASTNode* method_map(struct ASTNode*, struct Scope*);
struct ASTNode *method_int_to_string(struct ASTNode *, struct Scope*);

/* List functions */
struct ASTNode *method_list(struct ASTNode *, struct Scope *);
struct ASTNode *method_car(struct ASTNode *, struct Scope *);
struct ASTNode *method_cdr(struct ASTNode *, struct Scope *);
struct ASTNode *method_quote(struct ASTNode *, struct Scope *);
struct ASTNode *method_unquote(struct ASTNode *, struct Scope *);
struct ASTNode *method_range(struct ASTNode *, struct Scope *);

#endif
