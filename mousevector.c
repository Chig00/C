#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#define TITLE "Mouse Vector by Chigozie Agomo"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

int main(int argc, char** argv) {
	int a = 0;
	int b = 0;
	int x = 0;
	int y = 0;
	bool click = false;
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
	SDL_UpdateWindowSurface(window);
	
	while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		if (SDL_GetMouseState(&a, &b) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			if (!click) {
				printf(
					"x-shift = %d\ny-shift = %d\nr-shift = %f\nt-shift = %f\n\n",
					a - x, b - y, pow(pow(a - x, 2) + pow(b - y, 2), 0.5), atan2(b - y, a - x) * 180 / acos(-1)
				);
				x = a;
				y = b;
				click = true;
			}
		}
		else if (click) {
			click = false;
		}
		
		SDL_PumpEvents();
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}