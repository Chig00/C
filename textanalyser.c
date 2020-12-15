#include <stdio.h>
#include <stdbool.h>

const char* const DEFAULT_SOURCE = "source.txt";
const int COUNT_STACKS = 29;
const int WHITESPACE = 26;
const int PUNCTUATION = 27;
const int TOTAL = 28;

int main(int argc, char* argv[]) {
	const char* source_name;

	if (argc > 1) {
		source_name = argv[1];
	}

	else {
		source_name = DEFAULT_SOURCE;
	}

	FILE* source = fopen(source_name, "r");

	int counts[COUNT_STACKS] = {0};

	while (true) {
		char character;
		fscanf(source, "%c", &character);

		if (feof(source)) {
			break;
		}

		else if ('A' <= character && character <= 'Z') {
			counts[character - 'A']++;
		}

		else if ('a' <= character && character <= 'z') {
			counts[character - 'a']++;
		}

		else if (character == ' ' || character == '\n') {
			counts[WHITESPACE]++;
		}

		else {
			counts[PUNCTUATION]++;
		}

		counts[TOTAL]++;
	}

	fclose(source);

	for (int i = 0; i < 26; i++) {
		printf("%c / %c: %lf %c\n", 'A' + i, 'a' + i, 100.0 * counts[i] / counts[TOTAL], '%');
	}

	printf("\nLetters: %lf %c\n", 100.0 * (counts[TOTAL] - counts[WHITESPACE] - counts[PUNCTUATION]) / counts[TOTAL], '%');
	printf("Whitespace: %lf %c\n", 100.0 * counts[WHITESPACE] / counts[TOTAL], '%');
	printf("Punctuation: %lf %c\n", 100.0 * counts[PUNCTUATION] / counts[TOTAL], '%');
}