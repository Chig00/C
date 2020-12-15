#include <stdio.h>
#include <SDL.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

const char TITLE[] = "Colour Displayer";

int main(int argc, char** argv) {
	int red;
	int green;
	int blue;
	
	printf("RGB: ");
	scanf("%d%d%d", &red, &green, &blue);
	
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	SDL_FillRect(display, NULL, SDL_MapRGB(display->format, red, green, blue));
	SDL_UpdateWindowSurface(window);
	
	while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		SDL_PumpEvents();
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}