#pragma once

#include "common.h"

namespace Lisp {
	namespace AST {
		enum Type {
			LIST_CONTENTS,
			VALUE,
			QUOTE_LIST,
			PAIR,
			LIST,
			QUOTE_SYMBOL,
			INT,
			SYMBOL,
			PATH,
			STRING,
			PROGRAM
		};
		struct Span {
			const i8* start;
			const i8* end;

			void output(u32 depth);
		};
		
		struct Value {
			Type t;
			void* data;

			void output(u32 depth);
		};
		struct ListContents {
			Value* v;
			ListContents* n;
			
			void output(u32 depth);
		};

		ListContents* parseListContents(const i8** p);
		Value* parseValue(const i8** p);
	};

	AST::ListContents* parse(const i8** p);
};
