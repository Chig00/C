#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int RUNS = 10;
const int MIN = 1;
const int MAX = 99;

typedef struct {
	int num;
	int denom;
} Fraction;

int random() {
	return MIN + (MAX - MIN + 1) * rand() / (RAND_MAX + 1);
}

int get_hcf(int x, int y) {
	int hcf;
	
	for (int i = 1; i <= abs(x) && i <= abs(y); i++) {
		if (x / i == (double) x / i && y / i == (double) y / i) {
			hcf = i;
		}
	}
	
	return hcf;
}

int get_lcm(int x, int y) {
	return x * y / get_hcf(x, y);
}

Fraction add(Fraction x, Fraction y) {
	int lcm = get_lcm(x.denom, y.denom);
	x.num *= lcm / x.denom;
	x.denom = lcm;
	y.num *= lcm / y.denom;
	y.denom = lcm;
	
	Fraction res = {
		.num = x.num + y.num,
		.denom = x.denom
	};
	
	int hcf = get_hcf(res.num, res.denom);
	res.num /= hcf;
	res.denom /= hcf;
	
	return res;
}

Fraction sub(Fraction x, Fraction y) {
	int lcm = get_lcm(x.denom, y.denom);
	x.num *= lcm / x.denom;
	x.denom = lcm;
	y.num *= lcm / y.denom;
	y.denom = lcm;
	
	Fraction res = {
		.num = x.num - y.num,
		.denom = x.denom
	};
	
	int hcf = get_hcf(res.num, res.denom);
	res.num /= hcf;
	res.denom /= hcf;
	
	return res;
}

Fraction mul(Fraction x, Fraction y) {
	Fraction res = {
		.num = x.num * y.num,
		.denom = x.denom * y.denom
	};
	
	int hcf = get_hcf(res.num, res.denom);
	res.num /= hcf;
	res.denom /= hcf;
	
	return res;
}

Fraction divi(Fraction x, Fraction y) {
	Fraction res = {
		.num = x.num * y.denom,
		.denom = x.denom * y.num
	};
	
	int hcf = get_hcf(res.num, res.denom);
	res.num /= hcf;
	res.denom /= hcf;
	
	return res;
}

int main() {
	srand(time(NULL));
	rand();
	
	for (int i = 0; i < RUNS; i++) {
		Fraction f1 = {.num = random(), .denom = random()};
		Fraction f2 = {.num = random(), .denom = random()};
		Fraction res;
		
		res = add(f1, f2);
		printf("(%d/%d) + (%d/%d) = (%d/%d)!\n", f1.num, f1.denom, f2.num, f2.denom, res.num, res.denom);
		
		res = sub(f1, f2);
		printf("(%d/%d) - (%d/%d) = (%d/%d)!\n", f1.num, f1.denom, f2.num, f2.denom, res.num, res.denom);
		
		res = mul(f1, f2);
		printf("(%d/%d) * (%d/%d) = (%d/%d)!\n", f1.num, f1.denom, f2.num, f2.denom, res.num, res.denom);
		
		res = divi(f1, f2);
		printf("(%d/%d) / (%d/%d) = (%d/%d)!\n", f1.num, f1.denom, f2.num, f2.denom, res.num, res.denom);
	}
}