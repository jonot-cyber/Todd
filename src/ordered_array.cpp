#include "ordered_array.h"
#include "memory.h"

using namespace OrderedArray;

i8 standardLessThanPredicate(void* a, void* b) {
	return ((int)a < (int)b) ? 1 : 0;
}

OrderedArray::Array OrderedArray::Array::create(u32 maxSize, typename LessThanPredicate::Method lessThan) {
	Array toRet;
	toRet.array = (void**)Memory::kmalloc(maxSize*sizeof(void*));
	memset(reinterpret_cast<u8*>(toRet.array), 0, maxSize*sizeof(void*));
	toRet.size = 0;
	toRet.maxSize = maxSize;
	toRet.lessThan = lessThan;
	return toRet;
}

OrderedArray::Array OrderedArray::Array::place(void* addr, u32 maxSize, typename LessThanPredicate::Method lessThan) {
	Array toRet;
	toRet.array = (void**)addr;
	memset(reinterpret_cast<u8*>(toRet.array), 0, maxSize * sizeof(void*));
	toRet.size = 0;
	toRet.maxSize = maxSize;
	toRet.lessThan = lessThan;
	return toRet;
}

void Array::destroy() {
	// kfree(this->array);
}

void Array::insert(void* item) {
	if (!this->lessThan) {
		Monitor::putString("Array doesn't have lessThan");
		halt();
	}
	u32 iterator = 0;
	while (iterator < this->size && this->lessThan(this->array[iterator], item)) {
		iterator++;
	}
	if (iterator == this->size) {
		this->array[this->size++] = item;
	} else {
		void* tmp = this->array[iterator];
		this->array[iterator] = item;
		while (iterator < this->size) {
			iterator++;
			void* tmp2 = this->array[iterator];
			this->array[iterator] = tmp;
			tmp = tmp2;
		}
		this->size++;
	}
}

void* Array::lookup(u32 i) {
	if (i >= this->size) {
		Monitor::putString("Out of bounds!\n");
		halt();
	}
	return this->array[i];
}

void Array::remove(u32 i) {
	while (i < size) {
		array[i] = array[i+1];
		i++;
	}
	size--;
}
