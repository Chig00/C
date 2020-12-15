#include <stdbool.h>
#include <SDL.h>

const char* const TITLE = "Illusion by Chigozie Agomo";
const int QUIT = SDL_SCANCODE_ESCAPE;
const int WHITE = 0xff;
const int BLACK = 0x00;
const int SURFACE_DEPTH = 32;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const Uint32 SURFACE_MASKS[] = {
		0xff000000,
		0x00ff0000,
		0x0000ff00,
		0x000000ff
	};
#else
	const Uint32 SURFACE_MASKS[] = {
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000
	};
#endif

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_DisplayMode display_mode;
	SDL_GetDesktopDisplayMode(0, &display_mode);
	
	SDL_Window* window =
		SDL_CreateWindow(
			TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			display_mode.w, display_mode.h, SDL_WINDOW_FULLSCREEN
		)
	;
	SDL_Surface* window_surface = SDL_GetWindowSurface(window);
	
	int brightness = 0;
	int reverse = false;
	
	SDL_Surface* illusion =
		SDL_CreateRGBSurface(
			0, display_mode.w / 2, display_mode.h / 2, SURFACE_DEPTH,
			SURFACE_MASKS[0], SURFACE_MASKS[1], SURFACE_MASKS[2], SURFACE_MASKS[3]
		)
	;
	SDL_Rect rect = {.x = display_mode.w / 4, .y = display_mode.h / 4};
	
	for (int i = 0; i < display_mode.w / 2; i++) {
		for (int j = 0; j < display_mode.h / 2; j++) {
			SDL_Rect rect = {.x = i, .y = j, .w = 1, .h = 1};
			SDL_FillRect(illusion, &rect, SDL_MapRGB(illusion->format, WHITE * ((i + j) % 2), WHITE * ((i + j) % 2), WHITE * ((i + j) % 2)));
		}
	}
	
	while (!SDL_GetKeyboardState(NULL)[QUIT]) {
		SDL_FillRect(window_surface, NULL, SDL_MapRGB(window_surface->format, brightness, brightness, brightness));
		reverse ? brightness-- : brightness++;
		SDL_BlitSurface(illusion, NULL, window_surface, &rect);
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
		
		if (brightness == WHITE) {
			reverse = true;
		}
		
		else if (brightness == BLACK) {
			reverse = false;
		}
	}
	
	SDL_FreeSurface(illusion);
	SDL_DestroyWindow(window);
	SDL_Quit();
}