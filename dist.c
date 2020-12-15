#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>

const char* TITLE = "Distribution Displayer by Chigozie Agomo";
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
const int QUIT = SDL_SCANCODE_ESCAPE;
const int DIST_SIZE = 12;

double distribute() {
	double val = 0;
	
	for (int i = 0; i < DIST_SIZE; i++) {
		val += (double) rand() / (RAND_MAX + 1);
	}
	
	val /= DIST_SIZE;
	
	return val;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	bool dist[WINDOW_WIDTH][WINDOW_HEIGHT];
	bool new_dist[WINDOW_WIDTH][WINDOW_HEIGHT];
	bool quit = false;
	
	for (int i = 0; i < WINDOW_WIDTH; i++) {
		for (int j = 0; j < WINDOW_HEIGHT; j++) {
			dist[i][j] = false;
		}
	}
	
	while (!quit) {
		quit = SDL_GetKeyboardState(NULL)[QUIT];
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0x0, 0x0, 0x0));
		
		for (int i = 0; i < WINDOW_WIDTH; i++) {
			for (int j = WINDOW_HEIGHT - 1; j > 0; j--) {
				bool full = true;
					
				for (int k = j; k < WINDOW_HEIGHT; k++) {
					if (!dist[i][k]) {
						full = false;
						break;
					}
				}
				
				if (!full) {
					new_dist[i][j] = dist[i][j - 1];
				}
			}
			
			new_dist[i][0] = false;
		}
		
		for (int i = 0; i < WINDOW_WIDTH; i++) {
			for (int j = 0; j < WINDOW_HEIGHT; j++) {
				dist[i][j] = new_dist[i][j];
			}
		}
		
		dist[(int) (WINDOW_WIDTH  * distribute())][0] = true;
		
		for (int i = 0; i < WINDOW_WIDTH; i++) {
			for (int j = 0; j < WINDOW_HEIGHT; j++) {
				if (dist[i][j]) {
					SDL_Rect rect = {.x = i, .y = j, .w = 1, .h = 1};
					SDL_FillRect(display, &rect, SDL_MapRGB(display->format, 0xff, 0xff, 0xff));
				}
			}
		}
		
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
}