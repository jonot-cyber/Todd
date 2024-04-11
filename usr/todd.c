#include "system.h"

static int len = 58;
static char* todd = "   ___\n__/.  \\    ,\n\\_____/___/|\n  /        |\n  \\_______/\n";

int main() {
	write(todd, len);
	return 0;
}
