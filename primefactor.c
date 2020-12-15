#include <stdio.h>
#include <stdbool.h>

int main() {
	printf("Number: ");
	unsigned number;
	scanf("%d", &number);
	printf("%d =", number);
	bool first = true;
	
	while (number > 1) {
		for (int i = 2;; i++) {
			if (number % i == 0) {
				number /= i;
				
				if (!first) {
					printf(" x");
				}
				else {
					first = false;
				}
				
				printf(" %d", i);
				break;
			}
		}
	}
	
	printf("\n");
}