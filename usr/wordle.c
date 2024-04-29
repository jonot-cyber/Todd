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
#include "wordle_dictionary.h"
	const int word_count = 2314;
	const char *correct_word = &words[6 * (rand() % 2314)];
	
	printf("== [WORDLE] ==\n");
	int game_over = 0;
	int rounds = 0;
	while (!game_over) {
		rounds++;
		char word[6];
		printf("Enter a guess: ");
		gets(word, 6);
		putchar('\n');

		to_upper(word);
		int letters_matched[5] = {0, 0, 0, 0, 0};
		/* Mark all the green letters at the beginning */
		for (int i = 0; i < 5; i++)
			if (word[i] == correct_word[i])
				letters_matched[i] = 1;

		/* Now we match the yelllows */
		for (int i = 0; i < 5; i++) {
			if (word[i] == correct_word[i])
				set_foreground_color(VGA_LIGHT_GREEN);
			else {
				int found = 0;
				for (int j = 0; j < 5; j++)
					if (word[i] == correct_word[j]
					    && !letters_matched[j]) {
						found = 1;
						letters_matched[j] = 1;
						break;
					}
				if (found)
					set_foreground_color(VGA_LIGHT_BROWN);
				else
					set_foreground_color(VGA_LIGHT_GRAY);
			}
			putchar(word[i]);
		}
		set_foreground_color(VGA_WHITE);
		putchar('\n');

		/* Check if we win */
		game_over = 1;
		for (int i = 0; i < 5; i++)
			if (word[i] != correct_word[i]) {
				game_over = 0;
				break;
			}
	}
	printf("You won in %d guesses!\n", rounds);
}
