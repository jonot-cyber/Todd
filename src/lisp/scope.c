#pragma GCC push_options
#pragma GCC optimize ("O0")

#include "scope.h"

#include "memory.h"
#include "methods.h"
#include "monitor.h"
#include "string.h"
#include "timer.h"

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

void scope_init(struct Scope* scope) {
	scope->data = kmalloc_z(SCOPE_TABLES * sizeof(struct ScopeEntry*));

	struct ScopeEntry* tmp;
	tmp->node = NULL;
	tmp->params = NULL;

	// Add our methods
	tmp->method = method_add;
	tmp->name = "+";
	scope_add(scope, tmp);
	tmp->method = method_sub;
	tmp->name = "-";
	scope_add(scope, tmp);
	tmp->method = method_mul;
	tmp->name = "*";
	scope_add(scope, tmp);
	tmp->method = method_div;
	tmp->name = "/";
	scope_add(scope, tmp);
	tmp->method = method_if;
	tmp->name = "if";
	scope_add(scope, tmp);
	tmp->method = method_eq;
	tmp->name = "=";
	scope_add(scope, tmp);
	tmp->method = method_define;
	tmp->name = "define";
	scope_add(scope, tmp);
	tmp->method = method_display;
	tmp->name = "display";
	scope_add(scope, tmp);
	tmp->method = method_and;
	tmp->name = "and";
	scope_add(scope, tmp);
	tmp->method = method_or;
	tmp->name = "or";
	scope_add(scope, tmp);
	tmp->method = method_not;
	tmp->name = "not";
	scope_add(scope, tmp);
}

void scope_add(struct Scope* scope, struct ScopeEntry* entry) {
	struct ScopeEntry* entry_cpy = kmalloc(sizeof(struct ScopeEntry));
	memcpy(entry, entry_cpy, sizeof(struct ScopeEntry));
	u32 name_len = strlen(entry_cpy->name) + 1;
	i8* new_name_buf = kmalloc(name_len);
	memcpy(entry_cpy->name, new_name_buf, name_len);
	entry_cpy->level = scope->level;
	entry_cpy->next = NULL;
	entry_cpy->name = new_name_buf;
	u32 bucket_id = pjw_hash((u8*)entry_cpy->name, strlen(entry_cpy->name));
	bucket_id %= SCOPE_TABLES;
	if (scope->data[bucket_id]) {
		// TODO: Implement replacing
		entry_cpy->next = scope->data[bucket_id];
		scope->data[bucket_id] = entry_cpy;
	} else {
		scope->data[bucket_id] = entry_cpy;
		entry_cpy->next = NULL;
	}
}

void scope_del(struct Scope* scope, i8 const* name) {
	u32 bucket_id = pjw_hash((u8*)name, strlen(name));
	bucket_id %= SCOPE_TABLES;
	struct ScopeEntry* root = scope->data[bucket_id];
	assert(root != NULL,
	       "scope2_del: Attempted to delete nonexistent key");
	if (strcmp(root->name, name) == 0) {
		// We found our key at the root
		scope->data[bucket_id] = root->next;
		kfree(root);
		return;
	}
	// If it's not at the root, we have to keep going
	struct ScopeEntry* prev = root;
	struct ScopeEntry* ptr = root->next;
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

void scope_in(struct Scope* scope) {
	scope->level++;
}

struct ScopeEntry* scope2_recursively_delete(struct ScopeEntry* entry, u32 level) {
	if (entry == NULL) {
		return NULL;
	}
	if (entry->level < level) {
		return entry;
	}
	struct ScopeEntry* ret = scope2_recursively_delete(entry->next, level);
	// TODO: Most of the memory in the kernel isn't getting freed
	// at all. I'm going to need to start caring about that at
	// some point.
	kfree(entry);
	return ret;
}

void scope_out(struct Scope* scope) {
	for (u32 i = 0; i < SCOPE_TABLES; i++) {
		scope->data[i] = scope2_recursively_delete(scope->data[i], scope->level);
	}
	scope->level--;
}

struct ScopeEntry* scope_lookup(struct Scope* scope, i8 const* name) {
	u32 bucket_id = pjw_hash((u8*)name, strlen(name));
	bucket_id %= SCOPE_TABLES;
	struct ScopeEntry* ptr = scope->data[bucket_id];
	while (ptr) {
		if (strcmp(ptr->name, name) == 0) {
			// We found the entry! Return.
			break;
		}
		ptr = ptr->next;
	}
	return ptr;
}

#pragma GCC pop_options
