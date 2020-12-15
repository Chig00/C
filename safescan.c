#include <stdio.h>
#include <stdbool.h>

int main() {
	while (true) {
		printf("Character: ");
		char c;
		scanf("%c", &c);
		while (getchar() != '\n');
		printf("Your character is: %c\n", c);
	}
}