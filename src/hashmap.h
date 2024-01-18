#pragma once

#include "common.h"
#include "memory.h"
#include "monitor.h"


/**
   Calculate a crc32 hash

   @param data is the data to hash
   @param len is how long the data is.
 */
u32 crc32(const u8* data, u32 len);

namespace Types {
	template <typename K, typename V>
	struct HashMapPair {
		K key;
		V value;
		HashMapPair<K, V>* next;
	};
	
	template <typename K, typename V>
	struct HashMap {
		const u32 MIN_BUCKETS = 64;
		
		HashMapPair<K, V>** buckets;
		u32 nBuckets;
		u32 nEntries;

		HashMap() {
			// Create 64 buckets to start
			nEntries = 0;
			nBuckets = MIN_BUCKETS;
			buckets = Memory::kmalloc<HashMapPair<K, V>*>(nBuckets);
			for (u32 i = 0; i < nBuckets; i++) {
				buckets[i] = nullptr;
			}			
		}

		~HashMap() {
			// Free our buckets
			Memory::kfree(buckets);
		}

		u32 getBucket(K* key) {			
			u32 hash = crc32((const u8*)key, sizeof(K));
			return hash % nBuckets;
		}

		void reInsert(HashMapPair<K, V>* node) {
			if (!node) {
				return;
			}
			reInsert(node->next);
			insert(node->key, node->value);
			Memory::kfree(node);
		}

		void postInsert() {
			nEntries++;
			if (nEntries > nBuckets * 2) { // Resize table
				Monitor::printf("Resizing table to %d buckets\n", nBuckets * 2);
				HashMapPair<K, V>** tmp = buckets;
				u32 tmpBuckets = nBuckets;
				nEntries = 0;
				nBuckets *= 2;
				buckets = Memory::kmalloc<HashMapPair<K, V>*>(nBuckets);
				for (u32 i = 0; i < tmpBuckets; i++) {
					reInsert(tmp[i]);
				}
				Memory::kfree(tmp);
			}
		}

		HashMapPair<K, V>* insert(K key, V value) {
			u32 i = getBucket(&key);
			if (buckets[i] != nullptr) {
				if (buckets[i]->key == key) {
					// Replace instead of insert
					buckets[i]->value = value;
					return buckets[i];
				}
				// Otherwise, use linked list
				HashMapPair<K, V>* node = buckets[i];
				while (node->next) {
					node = node->next;
				}
				node->next = Memory::kmalloc<HashMapPair<K, V>>();
				node->next->key = key;
				node->next->value = value;
				node->next->next = nullptr;
				postInsert();
				return node->next;
			} else {
				HashMapPair<K, V>* node = Memory::kmalloc<HashMapPair<K, V>>();
				node->key = key;
				node->value = value;
				node->next = nullptr;
				buckets[i] = node;
				postInsert();
				return buckets[i];
			}
		}

		void postRemove() {
			nEntries--; 
			if (nBuckets < MIN_BUCKETS * 2) {
				// Don't shrink the table below the minimum size
				return;
			}
			if (nEntries >= nBuckets / 4) {
				return;
			}
			HashMapPair<K, V>** tmp = buckets;
			u32 tmpBuckets = nBuckets;
			nBuckets /= 2;
			Monitor::printf("Resizing the table to %d buckets\n", nBuckets);
			nEntries = 0;
			buckets = Memory::kmalloc<HashMapPair<K, V>*>(nBuckets);
			for (u32 i = 0; i < tmpBuckets; i++) {
				reInsert(tmp[i]);
			}
			Memory::kfree(tmp);
		}

		void remove(K key) {
			u32 i = getBucket(&key);
			if (buckets[i] == nullptr) {
				// Nothing to delete;
				return;
			} else {
				HashMapPair<K, V>* node = buckets[i];
				if (node->key == key) {
					buckets[i] = node->next;
					Memory::kfree(node);
					postRemove();
					return;
				}
				while (node->next) {
					if (node->next->key == key) {
						node->next = node->next->next;
						Memory::kfree(node->next);
						postRemove();
						return;
					}
					node = node->next;
				}
				// Nothing found;
				return;
			}
		}

		V* operator[](K key) {
			u32 i = getBucket(&key);
			if (!buckets[i]) {
				return nullptr;
			}
			HashMapPair<K, V>* node = buckets[i];
			while (node->key != key) {
				if (!node->next) {
					return nullptr;
				}
				node = node->next;
			}
			return &node->value;
		}
	};
};
