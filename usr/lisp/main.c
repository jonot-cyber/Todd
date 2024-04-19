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
	const char *tmp_buf = "(+ 1 2)";
	struct ParserListContents *lc = parse(&tmp_buf);
	scope_exec(&scope, lc);
	return 0;
	while (1) {
		char key;
		gets(&key, 1);
		putchar(key);
		/* Statement done */
		if (key == '\n' && balance == 0) {
			buf_i = 0;
			const char *to_parse = buf;
			struct ParserListContents *contents = parse(&to_parse);
			scope_exec(&scope, contents);
			memset(buf, 0, 256);
			printf("=> ");
		} else if (key == '\n') {
			printf(".. ");
		} else if (key == '\b') {
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
