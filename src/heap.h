#pragma once

#include "common.h"
#include "ordered_array.h"

namespace Heap {
	/** Memory address for the heap to start */
	const u32 START = 0xC0000000;
	/** Initial size of the heap */
	const u32 INITIAL_SIZE = 0x100000;
	/** Size of the heap index */
	const u32 INDEX_SIZE = 0x20000;
	/** Magic byte, used to see where heap headers and footers are */
	const u32 MAGIC = 0x9970DD05;
	/** Minimum heap size */
	const u32 MIN_SIZE = 0x70000;

	struct Header {
		u32 magic;
		u8 isHole;
		u32 size;
	};

	struct Footer {
		u32 magic;
		Header* header;
	};

	struct Heap {
		OrderedArray::Array<Header*> index;
		u32 startAddr;
		u32 endAddr;
		u32 maxAddr;
		u8 supervisor;
		u8 readonly;

		/**
		   Create a new heap

		   @param start is where the heap should start in memory
		   @param end is where it should end
		   @param max is the maximum size that the heap can be
		   @param supervisor is something or other, I don't really know.
		   @param readOnly is whether the heap should be readonly
		 */
		static Heap* create(u32 start, u32 end, u32 max, bool supervisor, bool readOnly);

		/**
		   Get the size of the heap.
		 */
		u32 size();

		void* alloc(u32, bool);
		void free(void*);

		/**
		   Find the index of the smallest hole that can fit an allocation

		   @param size is the size of the allocation
		   @param pageAlign is whether the allocation needs to be aligned to a page
		 */
		u32 findSmallestHole(u32 size, bool pageAlign);

		/**
		   Expand the heap to a new size

		   @param newSize is the new size to expand to
		 */
		void expand(u32 newSize);

		/**
		   Contract the heap to a new size

		   @param newSize is the new size to contract to
		*/
		u32 contract(u32 newSize);
	};
};
