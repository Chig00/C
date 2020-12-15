#include <stdio.h>
#include <stdbool.h>

int main() {
	printf("Insert your simple mathematical calculation:\n");
	double numbers[2];
	char operation[2];
	scanf("%lf%s%lf", &numbers[0], operation, &numbers[1]);
	
	double result;
	
	switch (operation[0]){
		case '+':
			result = numbers[0] + numbers[1];
			break;
		
		case '-':
			result = numbers[0] - numbers[1];
			break;
		
		case '*':
			result = numbers[0] * numbers[1];
			break;
		
		case '/':
			result = numbers[0] / numbers[1];
			break;
			
		default:
			printf("Error in calculation.\n");
			return 1;
	}
	
	printf("Result: %lf\n", result);
}