#pragma once

#include "common.h"
#include "monitor.h"

namespace OrderedArray {
	struct LessThanPredicate {
		typedef i8 (*Method)(void*, void*);
	};

	struct Array {
		void** array;
		u32 size;
		u32 maxSize;
		typename LessThanPredicate::Method lessThan;

		static Array create(u32 maxSize, typename LessThanPredicate::Method lessThan);
		static Array place(void* addr, u32 maxSize, typename LessThanPredicate::Method lessThan);

		void destroy();
		void insert(void* item);
		void* lookup(u32 i);
		void remove(u32 i);
	};

	i8 standardLessThanPredicate(void* a, void* b);
};
