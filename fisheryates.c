#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int INT_C = 10;

int compare(const void* x, const void* y) {
	return *(int*) y - *(int*) x;
}

int main() {
	srand(time(NULL));
	int ints[INT_C];
	
	for (int i = 0; i < INT_C; i++) {
		ints[i] = i;
		printf("%d ", ints[i]);
	}
	
	printf("\n");
	
	for (int i = 0; i < INT_C - 1; i++) {
		int j = i + (INT_C - i) * rand() / (RAND_MAX + 1);
		int k = ints[j];
		ints[j] = ints[i];
		ints[i] = k;
	}
	
	for (int i = 0; i < INT_C; i++) {
		printf("%d ", ints[i]);
	}
	
	printf("\n");
	qsort(ints, INT_C, sizeof(*ints), compare);
	
	for (int i = 0; i < INT_C; i++) {
		printf("%d ", ints[i]);
	}
	
	printf("\n");
}