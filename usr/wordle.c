#include "stdio.h"
#include "system.h"

void to_upper(char *word) {
	while (*word) {
		if (*word >= 'a' && *word <= 'z') {
			*word -= 'a';
			*word += 'A';
		}
		word++;
	}
	return;
}

int main() {
	const char *words[] = {
		"APPLE",
	};
	int word_count = sizeof(words) / sizeof(words[0]);
	const char *correct_word = words[rand() % word_count];
	
	printf("== [WORDLE] ==\n");
	int game_over = 0;
	while (!game_over) {
		char word[6];
		printf("Enter a guess: ");
		gets(word, 6);
		putchar('\n');

		to_upper(word);
		int letters_matched[5] = {0, 0, 0, 0, 0};
		putchar('\n');
	}
}
