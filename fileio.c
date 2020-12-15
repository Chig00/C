#include <stdio.h>

const char* FILE_NAME = "secretnumber.txt";
const char* MESSAGE = "321";

void write() {
	FILE* file = fopen(FILE_NAME, "w");
	fprintf(file, MESSAGE);
	fclose(file);
}

int read() {
	FILE* file = fopen(FILE_NAME, "r");
	int number;
	fscanf(file, "%d", &number);
	fclose(file);

	return number;
}

int main() {
	write();
	printf("The number is %d!\n", read());
}