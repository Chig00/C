#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define PLAYER_WIDTH 80
#define PLAYER_HEIGHT 80
#define MOVEMENT 80

const char TITLE[] = "Block Shift by Chigozie Agomo";

void new_colour(int* colour) {
	colour[0] = 1 + 255 * rand() / RAND_MAX;
	colour[1] = 1 + 255 * rand() / RAND_MAX;
	colour[2] = 1 + 255 * rand() / RAND_MAX;
	
	if (colour[0] == 256) {
		colour[0]--;
	}
	if (colour[1] == 256) {
		colour[1]--;
	}
	if (colour[2] == 256) {
		colour[2]--;
	}
}

int main(int argc, char** argv) {
	srand(time(NULL));
	
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool space = false;
	
	SDL_Init(SDL_INIT_EVERYTHING);
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	
	SDL_Rect player;
	player.x = 0;
	player.y = 0;
	player.w = PLAYER_WIDTH;
	player.h = PLAYER_HEIGHT;
	
	int colour[] = {255, 255, 255};
	
	while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
		
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT]) {
			if (!left) {
				player.x -= MOVEMENT;
				left = true;
			}
		}
		else if (left) {
			left = false;
		}
		
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT]) {
			if (!right) {
				player.x += MOVEMENT;
				right = true;
			}
		}
		else if (right) {
			right = false;
		}
		
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP]) {
			if (!up) {
				player.y -= MOVEMENT;
				up = true;
			}
		}
		else if (up) {
			up = false;
		}
		
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN]) {
			if (!down) {
				player.y += MOVEMENT;
				down = true;
			}
		}
		else if (down) {
			down = false;
		}
		
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE]) {
			if (!space) {
				new_colour(colour);
				space = true;
			}
		}
		else if (space) {
			space = false;
		}
		
		if (player.x < 0) {
			player.x = 0;
		}
		else if (player.x > WINDOW_WIDTH - PLAYER_WIDTH) {
			player.x = WINDOW_WIDTH - PLAYER_WIDTH;
		}
		
		if (player.y < 0) {
			player.y = 0;
		}
		else if (player.y > WINDOW_HEIGHT - PLAYER_HEIGHT) {
			player.y = WINDOW_HEIGHT - PLAYER_HEIGHT;
		}
		
		SDL_FillRect(display, &player, SDL_MapRGB(display->format, colour[0], colour[1], colour[2]));
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}