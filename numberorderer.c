#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int comparer(const void* number_one, const void* number_two) {
	double result = *(const double*)number_one - *(const double*) number_two;
	
	if (result < 0) {
		return -1;
	}
	
	else if (result > 0) {
		return 1;
	}
	
	else {
		return 0;
	}
}

int main(int argc, char* argv[]) {
	double* numbers = NULL;
	int size = 0;
	
	printf("Enter numbers: ");
	
	while (true) {
		numbers = realloc(numbers, ++size * sizeof(double));
		scanf("%lf", &numbers[size - 1]);
		
		if (getchar() == '\n') {
			break;
		}
	}
	
	qsort(numbers, size, sizeof(double), comparer);
	
	printf("Your numbers:");
	
	for (int i = 0; i < size; i++) {
		printf(" %lf", numbers[i]);
	}
}