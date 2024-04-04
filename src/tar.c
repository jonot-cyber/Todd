#include "tar.h"

#include "string.h"
#include "io.h"
#include "memory.h"

struct FSNode* fs_root = NULL;

/* Read octal number from a buffer */
u32 read_octal(u8* data, u32 len) {
	u32 ret = 0;
	for (u32 i = 0; i < len; i++) {
		ret *= 8;
		ret += data[i] - '0';
	}
	return ret;
}

u32 round_size(u32 size) {
	/* If we are right on the boundry, we can just return */
	if (size % 512 == 0) {
		return size;
	}
	return (size + 512) & 0xFFFFFE00;
}

void load_ustar(struct TarHeader* header) {
	struct FSNode* node = NULL;
	struct FSNode* first = NULL;
	/* A blank header will mark the end of the file */
	while (header->file_name[0] != '\0') {
		struct FSNode* last = node;
		node = kmalloc(sizeof(struct FSNode));
		/* The firs one, meaing the root node */
		if (last == NULL) {
			first = node;
		} else {
			last->next = node;
		}
		u32 file_name_size = strlen((i8*)header->file_name);
		i8* name = kmalloc(file_name_size + 1);
		memcpy(header->file_name, name, file_name_size);
		name[file_name_size + 1] = '\0';
		node->name = name;
		node->file_size = read_octal(header->file_size, 12);
		node->data = &header->data_ptr;
		node->next = NULL;
		/* Align the file size to 512 bytes */
		u32 file_size = round_size(node->file_size);
		header = (struct TarHeader*)(node->data + file_size);
	}
	fs_root = first;
}

struct FSNode* tar_find_file(struct FSNode* root, i8 const* const name) {
	while (root) {
		if (strcmp(root->name, name) == 0) {
			return root;
		}
		root = root->next;
	}
	return NULL;
}
