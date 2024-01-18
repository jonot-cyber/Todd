#pragma once

#include "common.h"
#include "monitor.h"

namespace OrderedArray {
	// These two functions exist solely to get around dependency loops :)
	u32 shadowMalloc(u32 size);
	void shadowFree(void* ptr);
	
	template <typename T>
	struct LessThanPredicate {
		typedef i8 (*Method)(T, T);
	};

	template <typename T>
	struct Array {
		T* array;
		u32 size;
		u32 maxSize;
		typename LessThanPredicate<T>::Method lessThan;

		static Array create(u32 maxSize, typename LessThanPredicate<T>::Method lessThan) {
			return place(shadowMalloc(maxSize * sizeof(T)), maxSize, lessThan);
		}
		
		static Array place(void* addr, u32 maxSize, typename LessThanPredicate<T>::Method lessThan) {
			Array ret;
			ret.array = (T*)addr;
			memset(ret.array, 0, maxSize * sizeof(T));
			ret.size = 0;
			ret.maxSize = maxSize;
			ret.lessThan = lessThan;
			return ret;
		}

		void destroy() {
			shadowFree(array);
		}
		
		void insert(T item) {
			assert(lessThan, "OrderedArray::Array<T>: Array doesn't have less than");
			u32 i;
			for (i = 0; i < size && lessThan(array[i], item); i++);
			if (i == size) {
				array[size] = item;
			} else {
				T tmp = array[i];
				array[i] = item;
				while (i < size) {
					i++;
					T tmp2 = array[i];
					array[i] = tmp;
					tmp = tmp2;
				}
			}
			size++;
		}

		T& operator[](u32 i) {
			if (i >= size) {
				Monitor::writeString("PANIC: Out of bounds!\n");
				halt();
			}
			return array[i];
		}
		
		void remove(u32 i) {
			while (i < size) {
				array[i] = array[i+1];
				i++;
			}
			size--;
		}
	};

	template <typename T>
	i8 standardLessThanPredicate(T a, T b);
};
