#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>

const char* TITLE = "Target Smash 3D by Chigozie Agomo";
const int WINDOW_W = 800;
const int WINDOW_H = 800;
const int QUIT = SDL_SCANCODE_ESCAPE;
const int RESET = SDL_SCANCODE_R;
const int PAUSE = SDL_SCANCODE_P;
const double BASE_LIFE = 10;
const double LIFE_DECAY = 3;
const double PI = acos(-1);
const int TARGET_W = 200;
const int TARGET_H = 200;
const int TARGET_R = 0xff;
const int TARGET_G = 0x3f;
const int TARGET_B = 0X3f;
const double SENSITIVITY = 0.001;

typedef struct {
	double x;
	double y;
	bool click;
} Player;

typedef struct {
	double x;
	double y;
	double birth;
	double death;
	bool active;
} Target;

void reset(Player* player, Target* target) {
	player->x = 0;
	player->y = 0;
	player->click = false;
	target->active = false;
}

double get_time() {
	return (double) clock() / CLOCKS_PER_SEC;
}

double life_calc(int score) {
	return BASE_LIFE * LIFE_DECAY / (score + LIFE_DECAY);
}

double rand_ang() {
	return 2 * PI * rand() / RAND_MAX - PI;
}

void activate(Target* target, int score) {
	target->x = rand_ang();
	target->y = rand_ang() / 2;
	target->active = true;
	target->birth = get_time();
	target->death = target->birth + life_calc(score);
}

int set_lum(Target target) {
	return 0xff * (1 - (get_time() - target.birth) / (target.death - target.birth));
}

void look(Player* player) {
	int x;
	int y;
	SDL_GetMouseState(&x, &y);
	player->x += SENSITIVITY * (x - WINDOW_W / 2);
	player->y += SENSITIVITY * (y - WINDOW_H / 2);

	while (player->x > PI) {
		player->x -= 2 * PI;
	}

	while (player->x < -PI) {
		player->x += 2 * PI;
	}

	if (player->y > PI / 2) {
		player->y = PI / 2;
	}

	else if (player->y < -PI / 2) {
		player->y = -PI / 2;
	}
}

bool view(Player* player, Target* target, int* score, SDL_Surface* display) {
	double ang_x = target->x - player->x;
	double ang_y = target->y - player->y;
	
	if (ang_x > PI) {
		ang_x -= 2 * PI;
	}
	
	else if (ang_x < -PI) {
		ang_x += 2 * PI;
	}
	
	SDL_Rect rect = {.w = TARGET_W, .h = TARGET_H};
	rect.x = WINDOW_W / 2 * (1 + ang_x / (PI / 3)) - rect.w;
	rect.y = WINDOW_H / 2 * (1 + ang_y / (PI / 3)) - rect.h;
	
	SDL_FillRect(display, &rect, SDL_MapRGB(display->format, TARGET_R, TARGET_G, TARGET_B));
	
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		while (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			SDL_PumpEvents();
		}
		
		SDL_Point point = {.x = WINDOW_W / 2, .y = WINDOW_H / 2};
		
		if (SDL_PointInRect(&point, &rect)) {
			++*score;
			target->active = false;
		}
		
		else {
			return true;
		}
	}
	
	return false;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	Player player;
	Target target;
	bool quit = false;

	while (!quit) {
		bool end = false;
		int score = 0;
		reset(&player, &target);

		while (!(quit || end)) {
			quit = SDL_GetKeyboardState(NULL)[QUIT];
			end = SDL_GetKeyboardState(NULL)[RESET];

			if (SDL_GetKeyboardState(NULL)[PAUSE]) {
				while (SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}

				while (!SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}

				while (SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}
			}

			if (!target.active) {
				activate(&target, score);
			}
			
			if (get_time() > target.death) {
				break;
			}

			int lum = set_lum(target);

			SDL_WarpMouseInWindow(window, WINDOW_W / 2, WINDOW_H / 2);
			SDL_FillRect(display, NULL, SDL_MapRGB(display->format, lum, lum, lum));

			look(&player);

			if (view(&player, &target, &score, display)) {
				break;
			}

			SDL_UpdateWindowSurface(window);
			SDL_PumpEvents();
		}

		printf("Score: %d\n", score);

		while (SDL_GetKeyboardState(NULL)[RESET]) {
			SDL_PumpEvents();
		}
	}
}