#pragma once

#include "exe.h"

namespace Lisp {
	namespace Methods {
		Lisp::EAST::Node* add(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
		Lisp::EAST::Node* sub(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
		Lisp::EAST::Node* mul(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
		Lisp::EAST::Node* div(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
		Lisp::EAST::Node* ifExpr(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
		Lisp::EAST::Node* eq(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
		Lisp::EAST::Node* define(Lisp::EAST::Node* n, Lisp::EAST::Scope* scope);
	};
};
