#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

const char* const OUTPUT_PROGRAM_NAME = "plotout";
const char* const OUTPUT_PROGRAM_HEAD =
	"#include <stdio.h>\n"
	"#include <math.h>\n"
	"\n"
	"const char* const OUTPUT_FILE_NAME = \"plotoutputs.txt\";\n"
	"\n"
	"int main(int argc, char* argv[]) {\n"
	"	double x;\n"
	"	sscanf(argv[1], \"%lf\", &x);\n"
	"	FILE* output_file = fopen(OUTPUT_FILE_NAME, \"a\");\n"
	"	fprintf(output_file, \"%lf\\n\", ";
const char* const OUTPUT_PROGRAM_TAIL =
	");\n"
	"	fclose(output_file);\n"
	"}";
const char* const OUTPUT_FILE_NAME = "plotoutputs.txt";
const int MAX_ARG_LENGTH = 100;
const char* const TITLE = "Function Plotter by Chigozie Agomo";
const int QUIT = SDL_SCANCODE_ESCAPE;
const int AXIS_COLOUR[] = {0x60, 0x60, 0x60};
const int PLOT_COLOUR[] = {0xff, 0xff, 0xff};

int main(int argc, char* argv[]) {
	char* function = NULL;
	char character;
	int function_length = 0;
	
	printf("f(x) = ");
	
	while ((character = getchar()) != '\n') {
		function = realloc(function, (function_length + 1) * sizeof(char));
		function[function_length] = character;
		function_length++;
	}
	
	function = realloc(function, (function_length + 1) * sizeof(char));
	function[function_length] = '\0';
	
	double domain[2];
	printf("Domain (min max): ");
	scanf("%lf%lf", &domain[0], &domain[1]);
	
	char* output_source_name = malloc((strlen(OUTPUT_PROGRAM_NAME) + strlen(".c") + 1) * sizeof(char));
	FILE* output_source = fopen(strcat(strcpy(output_source_name, OUTPUT_PROGRAM_NAME), ".c"), "w");
	free(output_source_name);
	
	fprintf(output_source, "%s%s%s", OUTPUT_PROGRAM_HEAD, function, OUTPUT_PROGRAM_TAIL);
	
	fclose(output_source);
	
	char* compile_string =
		malloc((strlen("gcc ") + strlen(OUTPUT_PROGRAM_NAME) + strlen(".c -o ") + strlen(OUTPUT_PROGRAM_NAME) + 1) * sizeof(char));
		
	if (system(strcat(strcat(strcat(strcpy(compile_string, "gcc "), OUTPUT_PROGRAM_NAME), ".c -o "), OUTPUT_PROGRAM_NAME))) {
		return EXIT_FAILURE;
	}
	
	free(compile_string);
	
	fclose(fopen(OUTPUT_FILE_NAME, "w"));
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_DisplayMode display_mode;
	SDL_GetDesktopDisplayMode(0, &display_mode);
	
	char* program_string = malloc((strlen(OUTPUT_PROGRAM_NAME) + MAX_ARG_LENGTH + 2) * sizeof(char));
	char* input = malloc(MAX_ARG_LENGTH * sizeof(char));
	
	for (int i = 0; i < display_mode.w; i++) {
		sprintf(input, "%lf", domain[0] + (domain[1] - domain[0]) * i / (display_mode.w - 1));
		system(strcat(strcat(strcpy(program_string, OUTPUT_PROGRAM_NAME), " "), input));
	}
	
	free(program_string);
	free(input);
	
	FILE* output_file = fopen(OUTPUT_FILE_NAME, "r");
	double* outputs = malloc(display_mode.w * sizeof(double));
	
	for (int i = 0; i < display_mode.w; i++) {
		fscanf(output_file, "%lf", &outputs[i]);
	}
	
	fclose(output_file);
	
	double range[] = {outputs[0], outputs[0]};
	
	for (int i = 0; i < display_mode.w; i++) {
		if (range[0] > outputs[i]) {
			range[0] = outputs[i];
		}
		
		else if (range[1] < outputs[i]) {
			range[1] = outputs[i];
		}
	}
	
	SDL_Window* window = SDL_CreateWindow(TITLE, 0, 0, display_mode.w, display_mode.h, SDL_WINDOW_FULLSCREEN);
	SDL_Surface* window_surface = SDL_GetWindowSurface(window);
	
	if (domain[0] <= 0 && 0 < domain[1]) {
		int position = (display_mode.w - 1) * -domain[0] / (domain[1] - domain[0]);
		SDL_Rect rect = {.x = position, .y = 0, .w = 1, .h = display_mode.h};
		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, AXIS_COLOUR[0], AXIS_COLOUR[1], AXIS_COLOUR[2]));
		SDL_UpdateWindowSurface(window);
	}
	
	if (range[0] <= 0 && 0 < range[1]) {
		int position = (display_mode.h - 1) * -range[0] / (range[1] - range[0]);
		SDL_Rect rect = {.x = 0, .y = position, .w = display_mode.w, .h = 1};
		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, AXIS_COLOUR[0], AXIS_COLOUR[1], AXIS_COLOUR[2]));
		SDL_UpdateWindowSurface(window);
	}
	
	for (int i = 0; i <= display_mode.w; i++) {
		int height = (display_mode.h - 1) * (1 - (outputs[i] - range[0]) / (range[1] - range[0]));
		SDL_Rect rect = {.x = i, .y = height, .w = 1, .h = 1};
		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, PLOT_COLOUR[0], PLOT_COLOUR[1], PLOT_COLOUR[2]));
		SDL_UpdateWindowSurface(window);
	}
	
	free(outputs);
	
	while (!SDL_GetKeyboardState(NULL)[QUIT]) {
		SDL_PumpEvents();
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
}