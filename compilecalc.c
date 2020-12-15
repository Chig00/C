#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const char* const CALC_NAME = "tempcalc";
const char* const CALC_HEAD = 
	"#include <stdio.h>\n"
	"#include <stdbool.h>\n"
	"#include <math.h>\n"
	"\n"
	"int main() {\n"
	"	printf(\"= %lf\\n\", (double) (";
const char* const CALC_TAIL =
	"));\n"
	"}";

int main() {
	printf("Enter the expression that you wish to be evaluated:\n");
	char* expression = NULL;
	char next_char;
	int exp_length = 0;
	
	while (true) {
		expression = realloc(expression, sizeof(char) * ++exp_length);
		next_char = getchar();
		
		if (next_char == '\n') {
			expression[exp_length - 1] = '\0';
			break;
		}
		
		else {
			expression[exp_length - 1] = next_char;
		}
	}
	
	char* source_name = malloc(strlen(CALC_NAME) + strlen(".c") + 1);
	strcat(strcpy(source_name, CALC_NAME), ".c");
	
	FILE* source_file = fopen(source_name, "w");
	free(source_name);
	fprintf(source_file, "%s%s%s", CALC_HEAD, expression, CALC_TAIL);
	fclose(source_file);
	
	char* compile_command = malloc(strlen("gcc ") + 2 * strlen(CALC_NAME) + strlen(".c -o ") + 1);
	strcat(strcat(strcat(strcpy(compile_command, "gcc "), CALC_NAME), ".c -o "), CALC_NAME);
	system(compile_command);
	system(CALC_NAME);
}