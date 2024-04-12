#include "stdio.h"
#include "system.h"

int main() {
	char name_buf[16];
	printf("What is your name? ");
	gets(name_buf, 16);
	printf("Hello, %s\n", name_buf);
	return 0;
}
