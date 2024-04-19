#include "system.h"

#include "file.h"

struct FSNode *fopen(const char *name) {
	return _fopen(name);
}
