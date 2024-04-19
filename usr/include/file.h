#ifndef USR_INCLUDE_FILE_H
#define USR_INCLUDE_FILE_H

struct FSNode {
	char *name;
	unsigned size;
	unsigned char *data;
	struct FSNode *next;
};

struct FSNode* fopen(const char *name);

#endif
