#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

const char* const TITLE = "Memory Trap by Chigozie Agomo";
const int WIN_W = 400;
const int WIN_H = 1;
const int LEAK = 1;

int main(int argc, char* argv[]) {
	SDL_Window* win = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIN_W, WIN_H, SDL_WINDOW_SHOWN
	);
	printf("ERROR: FATAL MEMORY LEAK.\n");
	while (true) {
		malloc(LEAK);
		SDL_WarpMouseInWindow(win, WIN_W / 2, WIN_H / 2);
	}
}