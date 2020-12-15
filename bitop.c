#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

const int OPS = 10;
const int MIN = 1;
const int MAX = 99;

int add(int x, int y) {
	while (y) {
		x ^= y;
		y &= ~x;
		y <<= 1;
	}
	
	return x;
}

int sub(int x, int y) {
	while (y) {
		x ^= y;
		y &= x;
		y <<= 1;
	}
	
	return x;
}

int mul(int x, int y) {
	int prod = 0;
	
	for (int i = 0; i < y; i = add(i, 1)) {
		prod = add(prod, x);
	}
	
	return prod;
}

int divi(int x, int y) {
	int i;
	
	for (i = 0; mul(y, i) < x; i = add(i, 1));
	
	if (mul(y, i) != x) {
		i = sub(i, 1);
	}
	
	return i;
}

int rand_int(int min, int max) {
	return add(min, divi(mul(add(sub(max, min), 1), rand()), add(RAND_MAX, 1)));
}

int main() {
	srand(time(NULL));
	rand();
	
	for (int i = 0; i < OPS; i = add(i, 1)) {
		int x = rand_int(MIN, MAX);
		int y = rand_int(MIN, x);
		printf("%d + %d = %d!\n", x, y, add(x, y));
		printf("%d - %d = %d!\n", x, y, sub(x, y));
		printf("%d * %d = %d!\n", x, y, mul(x, y));
		printf("%d / %d = %d!\n", x, y, divi(x, y));
	}
}