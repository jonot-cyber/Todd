#pragma once

#include "parser.h"

namespace Lisp {
	namespace EAST {
		enum Type {
			PAIR,
			QUOTE_PAIR,
			SYMBOL,
			QUOTE_SYMBOL,
			INT,
			PATH,
			STRING
		};

		struct Node {
			Type type;
			union {
				struct {
					Node* p1;
					Node* p2;
				} pair;
				struct {
					const i8* start;
					u32 size;
				} span;
				u32 num;
			} data;
		};

		struct Scope {
			struct Entry {
				i8* name;
				Node* node;
				u32 level;
			};

			u32 level;
			u32 used;
			Entry* data;

			void init();
			void add(i8 const* name, Node* node);
			void del(i8 const* name);
			void delIdx(u32 i);
			void in();
			void out();
			Node* lookup(i8 const* name);
			void exec(AST::ListContents* contents);
		};
	};
	
	void exe(AST::ListContents* contents);
	EAST::Node* execNode(EAST::Node*, EAST::Scope*);
}
