#include <stdio.h>
#include <limits.h>

int main() {
	for (int c = CHAR_MIN; c <= CHAR_MAX; c++) {
		printf("%d: %c\n", c, c);
	}
}