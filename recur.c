#include <stdio.h>

void recur(int x) {
	if (x < 9) {
		recur(x + 1);
	}
	
	printf("%d\n", x);
}

int main(int argc, char* argv[]) {
	recur(0);
}