#include "scope.h"

#include "memory.h"
#include "methods.h"
#include "monitor.h"
#include "string.h"

#define SCOPE_TABLES 512

u32 pjw_hash(u8* data, u32 len) {
	u32 ret = 0;
	for (u32 i = 0; i < len; i++) {
		ret = (ret << 8) + data[i];
		u8 high = (ret << 28) & 0xF;
		if (high) {
			ret ^= (high >> 24);
			ret &= ~high;
		}
	}
	return ret;
}

void scope2_init(struct Scope2* scope) {
	scope->data = kmalloc(SCOPE_TABLES * sizeof(struct ScopeEntry2*));
	memset(scope->data, 0, SCOPE_TABLES * sizeof(struct ScopeEntry2*));

	struct ScopeEntry2* tmp;
	tmp->node = NULL;
	tmp->params = NULL;

	// Add our methods
	tmp->method = method_add;
	tmp->name = "+";
	scope2_add(scope, tmp);
	tmp->method = method_sub;
	tmp->name = "-";
	scope2_add(scope, tmp);
	tmp->method = method_mul;
	tmp->name = "*";
	scope2_add(scope, tmp);
	tmp->method = method_div;
	tmp->name = "/";
	scope2_add(scope, tmp);
	tmp->method = method_if;
	tmp->name = "if";
	scope2_add(scope, tmp);
	tmp->method = method_eq;
	tmp->name = "=";
	scope2_add(scope, tmp);
	tmp->method = method_define;
	tmp->name = "define";
	scope2_add(scope, tmp);
}

void scope2_add(struct Scope2* scope, struct ScopeEntry2* entry) {
	printf("Adding entry: %s\n", entry->name);
	struct ScopeEntry2* entry_cpy = kmalloc(sizeof(struct ScopeEntry2));
	memcpy(entry, entry_cpy, sizeof(struct ScopeEntry2));
	entry_cpy->level = scope->level;
	entry_cpy->next = NULL;
	u32 bucket_id = pjw_hash((u8*)entry_cpy->name, strlen(entry_cpy->name));
	bucket_id %= SCOPE_TABLES;
	if (scope->data[bucket_id]) {
		// TODO: Implement replacing
		entry_cpy->next = scope->data[bucket_id];
		scope->data[bucket_id] = entry_cpy->next;
	} else {
		scope->data[bucket_id] = entry_cpy;
		entry_cpy->next = NULL;
	}
}

void scope2_del(struct Scope2* scope, i8 const* name) {
	u32 bucket_id = pjw_hash((u8*)name, strlen(name));
	bucket_id %= SCOPE_TABLES;
	struct ScopeEntry2* root = scope->data[bucket_id];
	assert(root != NULL,
	       "scope2_del: Attempted to delete nonexistent key");
	if (strcmp(root->name, name) == 0) {
		// We found our key at the root
		scope->data[bucket_id] = root->next;
		kfree(root);
		return;
	}
	// If it's not at the root, we have to keep going
	struct ScopeEntry2* prev = root;
	struct ScopeEntry2* ptr = root->next;
	while (ptr) {
		if (strcmp(ptr->name, name) == 0) {
			// Delete this link
			prev->next = ptr->next;
			kfree(ptr);
			return;
		}
		prev = ptr;
		ptr = ptr->next;
	}
	assert(false, "scope2_del: Could not find key");
	return;
}

void scope2_in(struct Scope2* scope) {
	scope->level++;
}

struct ScopeEntry2* scope2_recursively_delete(struct ScopeEntry2* entry, u32 level) {
	if (entry == NULL) {
		return NULL;
	}
	if (entry->level < level) {
		return entry;
	}
	struct ScopeEntry2* ret = scope2_recursively_delete(entry->next, level);
	// TODO: Most of the memory in the kernel isn't getting freed
	// at all. I'm going to need to start caring about that at
	// some point.
	kfree(entry);
	return ret;
}

void scope2_out(struct Scope2* scope) {
	for (u32 i = 0; i < SCOPE_TABLES; i++) {
		scope->data[i] = scope2_recursively_delete(scope->data[i], scope->level);
	}
	scope->level--;
}

struct ScopeEntry2* scope2_lookup(struct Scope2* scope, i8 const* name) {
	u32 bucket_id = pjw_hash((u8*)name, strlen(name));
	bucket_id %= SCOPE_TABLES;
	struct ScopeEntry2* ptr = scope->data[bucket_id];
	while (ptr) {
		if (strcmp(ptr->name, name) == 0) {
			// We found the entry! Return.
			break;
		}
		ptr = ptr->next;
	}
	return ptr;
}
