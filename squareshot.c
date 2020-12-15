#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#define TITLE "Square Shot by Chigozie Agomo"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define SQUARE_WIDTH 100
#define SQUARE_HEIGHT 100
#define SQUARE_SPEED 5

typedef struct {
	SDL_Rect rect;
	double true_pos[2];
	double velocity[2];
	int colour[3];
} Square;

void accelerate_square(Square* square, int x, int y) {
	x -= square->rect.x + square->rect.w / 2;
	y -= square->rect.y + square->rect.h / 2;
	square->velocity[0] = SQUARE_SPEED * cos(atan2(y, x));
	square->velocity[1] = SQUARE_SPEED * sin(atan2(y, x));
}

void update_square(Square* square, SDL_Surface* display) {
	square->true_pos[0] += square->velocity[0];
	square->true_pos[1] += square->velocity[1];
	square->rect.x = square->true_pos[0];
	square->rect.y = square->true_pos[1];
	
	bool stop = false;
	
	if (square->rect.x < 0) {
		square->rect.x = 0;
		stop = true;
	}
	else if (square->rect.x + square->rect.w > WINDOW_WIDTH) {
		square->rect.x = WINDOW_WIDTH - square->rect.w;
		stop = true;
	}
	
	if (square->rect.y < 0) {
		square->rect.y = 0;
		stop = true;
	}
	else if (square->rect.y + square->rect.h > WINDOW_HEIGHT) {
		square->rect.y = WINDOW_HEIGHT - square->rect.h;
		stop = true;
	}
	
	if (stop) {
		square->true_pos[0] = square->rect.x;
		square->true_pos[1] = square->rect.y;
		square->velocity[0] = 0;
		square->velocity[1] = 0;
	}
	
	SDL_FillRect(
		display, &square->rect,
		SDL_MapRGB(
			display->format, square->colour[0],
			square->colour[1], square->colour[2]
		)
	);
}

int random_colour() {
	return 51 * (1 + 5 * rand() / (RAND_MAX + 1));
}

void switch_colour(Square* square) {
	square->colour[0] = random_colour();
	square->colour[1] = random_colour();
	square->colour[2] = random_colour();
}

int main(int argc, char** argv) {
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	Square square = {
		.rect = {
			.x = 0,
			.y = 0,
			.w = SQUARE_WIDTH,
			.h = SQUARE_HEIGHT
		},
		.true_pos = {0, 0},
		.velocity = {0, 0},
		.colour = {255, 255, 255}
	};
	int x;
	int y;
	bool click;
	bool press;
	
	while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
		
		if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			if (!click) {
				if (!square.velocity[0] & !square.velocity[1]) {
					accelerate_square(&square, x, y);
				}
				click = true;
			}
		}
		else if (click) {
			click = false;
		}
		
		if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE]) {
			if (!press) {
				switch_colour(&square);
				press = true;
			}
		}
		else if (press) {
			press = false;
		}
		
		update_square(&square, display);
		
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}