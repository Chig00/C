#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>

#define TITLE "Target Smash by Chigozie Agomo"
#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define TARGET_WIDTH 50
#define TARGET_HEIGHT 50
#define INITIAL_LIFE 5
#define LIFE_CONSTANT 3

typedef struct Target {
	SDL_Rect rect;
	double life;
	double birth;
} Target;

double get_time() {
	return (double) clock() / CLOCKS_PER_SEC;
}

double get_life(int score) {
	return INITIAL_LIFE * LIFE_CONSTANT / (double) (score + LIFE_CONSTANT);
}

int randint(int low, int high) {
	return low + (high - low + 1) * rand() / (RAND_MAX + 1);
}

void form_target(Target* target, int score) {
	target->rect.x = randint(0, WINDOW_WIDTH - TARGET_WIDTH);
	target->rect.y = randint(0, WINDOW_HEIGHT - TARGET_HEIGHT);
	target->life = get_life(score);
	target->birth = get_time();
}

bool update_target(Target target, SDL_Surface* display) {
	double age = get_time() - target.birth;
	if (age >= target.life) {
		return true;
	}
	SDL_FillRect(display, &target.rect, SDL_MapRGB(display->format, 255, (int) (255 * (1 - age / target.life)), (int) (255 * (1 - age / target.life))));
}

int main(int argc, char** argv) {
	srand(time(NULL));
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	
	int score = 0;
	bool click = false;
	SDL_Point mouse;
	
	Target target;
	target.rect.w = TARGET_WIDTH;
	target.rect.h = TARGET_HEIGHT;	
	form_target(&target, score);
	
	while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
		
		if (SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			if (!click) {
				if (SDL_PointInRect(&mouse, &target.rect)) {
					form_target(&target, score);
					score++;
					click = true;
				}
				else {
					break;
				}
			}
		}
		else if (click) {
			click = false;
		}
		
		if (update_target(target, display)) {
			break;
		}
		
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
	
	printf("Score: %d\n", score);
	
	while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
		SDL_PumpEvents();
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}