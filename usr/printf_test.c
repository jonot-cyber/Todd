#include "stdio.h"
#include "system.h"

const int BUF_SIZE = 256;

int main() {
	char *name_buf = malloc(BUF_SIZE);
	if (name_buf == 0) {
		return 1;
	}
	puts("What is your name? ");
	gets(name_buf, BUF_SIZE);
	printf("Hello, %s\n", name_buf);
	free(name_buf);
	return 0;
}
