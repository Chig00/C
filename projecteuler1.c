#include <stdio.h>

int main() {
	int sum = 0;
	for (int i = 0; i < 1000; i++) {
		if (i % 3 == 0 || i % 5 == 0) {
			printf("%d\n", i);
			sum += i;
		}
	}
	printf("Total of all multiples of 3 and 5 below 1000: %d\n", sum);
}