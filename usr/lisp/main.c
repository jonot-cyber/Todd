#include "stdio.h"
#include "string.h"

#include "exe.h"
#include "parser.h"
#include "scope.h"

static char buf[256];

int main() {
	struct Scope scope;
	scope_init(&scope);

	memset(buf, 0, 256);
	unsigned buf_i = 0;
	unsigned balance = 0;
	printf("=> ");
	while (1) {
		char key;
		read(&key, 1);
		/* Statement done */
		if (key == '\n' && balance == 0) {
			buf_i = 0;
			const char *to_parse = buf;
			struct ParserListContents *contents = parse(&to_parse);
			scope_exec(&scope, contents);
			memset(buf, 0, 256);
			printf("=> ");
		} else if (key == '\n')
			printf(".. ");
		else if (key == '\b') {
			if (buf[buf_i - 1] == '(')
				balance--;
			else if (buf[buf_i - 1] == ')')
				balance++;
			buf_i--;
		} else {
			if (key == '(')
				balance++;
			else if (key == ')')
				balance--;
			buf[buf_i++] = key;
		}
	}
}
