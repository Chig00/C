#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* const PLOTTER_NAME = "plotter";
const char* const PLOTTER_HEAD =
	"#include <stdio.h>\n"
	"#include <stdlib.h>\n"
	"#include <stdbool.h>\n"
	"#include <math.h>\n"
	"#include <SDL.h>\n"
	"\n"
	"const char* const TITLE = \"Plotter by Chigozie Agomo v2.0\";\n"
	"const int AXIS_COLOUR[] = {0x40, 0x40, 0x40};\n"
	"const int PLOT_COLOUR[] = {0xff, 0xff, 0xff};\n"
	"const int QUIT = SDL_SCANCODE_ESCAPE;\n"
	"\n"
	"bool invalid(double output) {\n"
	"	if (isinf(output) || isnan(output)) {\n"
	"		return true;\n"
	"	}\n"
	"\n"
	"	return false;\n"
	"}\n"
	"\n"
	"int main(int argc, char* argv[]) {\n"
	"	double domain[2];\n"
	"	printf(\"Domain (min max): \");\n"
	"	scanf(\"%lf%lf\", &domain[0], &domain[1]);\n"
	"\n"
	"	SDL_Init(SDL_INIT_VIDEO);\n"
	"	SDL_DisplayMode display_mode;\n"
	"	SDL_GetDesktopDisplayMode(0, &display_mode);\n"
	"\n"
	"	double* outputs = malloc(display_mode.w * sizeof(double));\n"
	"\n"
	"	for (int i = 0; i < display_mode.w; i++) {\n"
	"		double x = domain[0] + (domain[1] - domain[0]) * i / (display_mode.w - 1);\n"
	"		outputs[i] = ";	
const char* const PLOTTER_TAIL =
	";\n"
	"		if (invalid(outputs[i])) {\n"
	"			outputs[i] = 0;\n"
	"		}\n"
	"	}\n"
	"\n"
	"	double range[] = {outputs[0], outputs[0]};\n"
	"\n"
	"	for (int i = 0; i < display_mode.w; i++) {\n"
	"		if (range[0] > outputs[i]) {\n"
	"			range[0] = outputs[i];\n"
	"		}\n"
	"\n"	
	"		else if (range[1] < outputs[i]) {\n"
	"			range[1] = outputs[i];\n"
	"		}\n"
	"	}\n"
	"\n"
	"	SDL_Window* window = SDL_CreateWindow(TITLE, 0, 0, display_mode.w, display_mode.h, SDL_WINDOW_FULLSCREEN);\n"
	"	SDL_Surface* window_surface = SDL_GetWindowSurface(window);\n"
	"\n"
	"	if (domain[0] <= 0 && 0 < domain[1]) {\n"
	"		int position = (display_mode.w - 1) * -domain[0] / (domain[1] - domain[0]);\n"
	"		SDL_Rect rect = {.x = position, .y = 0, .w = 1, .h = display_mode.h};\n"
	"		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, AXIS_COLOUR[0], AXIS_COLOUR[1], AXIS_COLOUR[2]));\n"
	"		SDL_UpdateWindowSurface(window);\n"
	"	}\n"
	"\n"
	"	if (range[0] <= 0 && 0 < range[1]) {\n"
	"		int position = (display_mode.h - 1) * (1 + range[0] / (range[1] - range[0]));\n"
	"		SDL_Rect rect = {.x = 0, .y = position, .w = display_mode.w, .h = 1};\n"
	"		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, AXIS_COLOUR[0], AXIS_COLOUR[1], AXIS_COLOUR[2]));\n"
	"		SDL_UpdateWindowSurface(window);\n"
	"	}\n"
	"\n"
	"	for (int i = 0; i <= display_mode.w; i++) {\n"
	"		int height = (display_mode.h - 1) * (1 - (outputs[i] - range[0]) / (range[1] - range[0]));\n"
	"		SDL_Rect rect = {.x = i, .y = height, .w = 1, .h = 1};\n"
	"		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, PLOT_COLOUR[0], PLOT_COLOUR[1], PLOT_COLOUR[2]));\n"
	"		SDL_UpdateWindowSurface(window);\n"
	"	}\n"
	"\n"
	"	free(outputs);\n"
	"\n"
	"	while (!SDL_GetKeyboardState(NULL)[QUIT]) {\n"
	"		SDL_PumpEvents();\n"
	"	}\n"
	"\n"
	"	SDL_DestroyWindow(window);\n"
	"	SDL_Quit();\n"
	"}\n";

int main() {
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
	
	char* plotter_name = malloc((strlen(PLOTTER_NAME) + strlen(".c") + 1) * sizeof(char));
	FILE* plotter = fopen(strcat(strcpy(plotter_name, PLOTTER_NAME), ".c"), "w");
	free(plotter_name);
	fprintf(plotter, "%s%s%s", PLOTTER_HEAD, function, PLOTTER_TAIL);
	fclose(plotter);
	
	
	char* compile_string =
		malloc((strlen("gcc ") + strlen(PLOTTER_NAME) + strlen(".c -l mingw32 -l sdl2main -l sdl2-o ") + strlen(PLOTTER_NAME) + 1)
		       * sizeof(char));
		
	if (system(strcat(strcat(strcat(strcpy(compile_string, "gcc "), PLOTTER_NAME),
	    ".c -l mingw32 -l sdl2main -l sdl2 -o "), PLOTTER_NAME))) {
		return EXIT_FAILURE;
	}
	
	system(PLOTTER_NAME);
}