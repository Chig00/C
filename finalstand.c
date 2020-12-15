#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL.h>

#define TITLE "Final Stand by Chigozie Agomo"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define BACKGROUND_COLOUR_R 100
#define BACKGROUND_COLOUR_G 175
#define BACKGROUND_COLOUR_B 255
#define BASE_WIDTH 100
#define BASE_HEIGHT 100
#define BASE_COLOUR_R 100
#define BASE_COLOUR_G 100
#define BASE_COLOUR_B 100
#define MAX_BASE_ROCKETS 3
#define BASE_ROCKET_WIDTH 25
#define BASE_ROCKET_HEIGHT 25
#define BASE_ROCKET_COLOUR_R BASE_COLOUR_R
#define BASE_ROCKET_COLOUR_G BASE_COLOUR_G
#define BASE_ROCKET_COLOUR_B BASE_COLOUR_B
#define BASE_ROCKET_SPEED 3
#define NO_HIT -1
#define OFFSCREEN -2
#define MAX_ENEMY_ROCKETS 20
#define ENEMY_ROCKET_WIDTH 50
#define ENEMY_ROCKET_HEIGHT 50
#define ENEMY_ROCKET_COLOUR_R 150
#define ENEMY_ROCKET_COLOUR_G 0
#define ENEMY_ROCKET_COLOUR_B 0
#define STARTING_DELAY 1
#define DELAY_CONSTANT 50
#define STARTING_SPEED 0.25
#define SPEED_CONSTANT 0.005

typedef struct {
	SDL_Rect rect;
	int colour[3];
} Base;

typedef struct {
	SDL_Rect rect;
	int colour[3];
	double position[2];
	double velocity[2];
	bool active;
} BaseRocket;

typedef struct {
	SDL_Rect rect;
	int colour[3];
	double position[2];
	double velocity[2];
	bool active;
} EnemyRocket;

double get_time() {
	return (double) clock() / CLOCKS_PER_SEC;
}

double get_shot_delay(int score) {
	return STARTING_DELAY * DELAY_CONSTANT / (double) (score + DELAY_CONSTANT);
}

int rand_int(int min, int max) {
	return min + (max - min + 1) * rand() / RAND_MAX;
}

void fire_base_rocket(BaseRocket* base_rockets, int x, int y) {
	for (int i = 0; i < MAX_BASE_ROCKETS; i++) {
		if (!base_rockets[i].active) {
			base_rockets[i].velocity[0] = BASE_ROCKET_SPEED * cos(atan2(y - WINDOW_HEIGHT / 2, x - WINDOW_WIDTH /2));
			base_rockets[i].velocity[1] = BASE_ROCKET_SPEED * sin(atan2(y - WINDOW_HEIGHT / 2, x - WINDOW_WIDTH /2));
			base_rockets[i].position[0] = (WINDOW_WIDTH - BASE_ROCKET_WIDTH) / 2;
			base_rockets[i].position[1] = (WINDOW_HEIGHT - BASE_ROCKET_HEIGHT) / 2;
			base_rockets[i].rect.x = base_rockets[i].position[0];
			base_rockets[i].rect.y = base_rockets[i].position[1];
			base_rockets[i].active = true;
			break;
		}
	}
}

void fire_enemy_rocket(EnemyRocket* enemy_rockets, int score) {
	for (int i = 0; i < MAX_ENEMY_ROCKETS; i++) {
		if (!enemy_rockets[i].active) {
			if (rand_int(0, 1)) {
				if (rand_int(0, 1)) {
					enemy_rockets[i].position[1] = WINDOW_HEIGHT;
				}
				else {
					enemy_rockets[i].position[1] = -ENEMY_ROCKET_HEIGHT;
				}
				enemy_rockets[i].position[0] = rand_int(-ENEMY_ROCKET_WIDTH, WINDOW_WIDTH);
			}
			else {
				if (rand_int(0, 1)) {
					enemy_rockets[i].position[0] = WINDOW_WIDTH;
				}
				else {
					enemy_rockets[i].position[0] = -ENEMY_ROCKET_WIDTH;
				}
				enemy_rockets[i].position[1] = rand_int(-ENEMY_ROCKET_HEIGHT, WINDOW_HEIGHT);
			}
			
			enemy_rockets[i].rect.x = enemy_rockets[i].position[0];
			enemy_rockets[i].rect.y = enemy_rockets[i].position[1];
			
			int x = WINDOW_WIDTH / 2 - (enemy_rockets[i].rect.x + ENEMY_ROCKET_WIDTH / 2);
			int y = WINDOW_HEIGHT / 2 - (enemy_rockets[i].rect.y + ENEMY_ROCKET_HEIGHT / 2);
			
			enemy_rockets[i].velocity[0] = STARTING_SPEED * (1 + SPEED_CONSTANT * score) * cos(atan2(y, x));
			enemy_rockets[i].velocity[1] = STARTING_SPEED * (1 + SPEED_CONSTANT * score) * sin(atan2(y, x));
			
			enemy_rockets[i].active = true;
			break;
		}
	}
}

int update_base_rocket(BaseRocket* base_rocket, EnemyRocket* enemy_rockets, SDL_Surface* display) {
	base_rocket->position[0] += base_rocket->velocity[0];
	base_rocket->position[1] += base_rocket->velocity[1];
	base_rocket->rect.x = base_rocket->position[0];
	base_rocket->rect.y = base_rocket->position[1];
	
	SDL_FillRect(display, &base_rocket->rect, SDL_MapRGB(display->format, base_rocket->colour[0], base_rocket->colour[1], base_rocket->colour[2]));
	
	for (int i = 0; i < MAX_ENEMY_ROCKETS; i++) {
		if (enemy_rockets[i].active) {
			if (SDL_HasIntersection(&base_rocket->rect, &enemy_rockets[i].rect)) {
				return i;
			}
		}
	}
	
	if (base_rocket->rect.x + BASE_ROCKET_WIDTH < 0 || base_rocket->rect.x > WINDOW_WIDTH || base_rocket->rect.y + BASE_ROCKET_HEIGHT < 0 || base_rocket->rect.y > WINDOW_HEIGHT) {
		return OFFSCREEN;
	}
	
	return NO_HIT;
}

bool update_enemy_rocket(EnemyRocket* enemy_rocket, Base base, SDL_Surface* display) {
	enemy_rocket->position[0] += enemy_rocket->velocity[0];
	enemy_rocket->position[1] += enemy_rocket->velocity[1];
	enemy_rocket->rect.x = enemy_rocket->position[0];
	enemy_rocket->rect.y = enemy_rocket->position[1];
	
	SDL_FillRect(display, &enemy_rocket->rect, SDL_MapRGB(display->format, enemy_rocket->colour[0], enemy_rocket->colour[1], enemy_rocket->colour[2]));
	
	return SDL_HasIntersection(&enemy_rocket->rect, &base.rect);
}

int main(int argc, char** argv) {
	srand(time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);	
	SDL_Surface* display = SDL_GetWindowSurface(window);

	Base base = {
		.rect = {
			.x = (WINDOW_WIDTH - BASE_WIDTH) / 2,
			.y = (WINDOW_HEIGHT - BASE_HEIGHT) / 2,
			.w = BASE_WIDTH,
			.h = BASE_HEIGHT
		},
		.colour = {
			BASE_COLOUR_R,
			BASE_COLOUR_G,
			BASE_COLOUR_B
		}
	};

	BaseRocket base_rockets[MAX_BASE_ROCKETS];
	for (int i = 0; i < MAX_BASE_ROCKETS; i++) {
		base_rockets[i].rect.w = BASE_ROCKET_WIDTH;
		base_rockets[i].rect.h = BASE_ROCKET_HEIGHT;
		base_rockets[i].colour[0] = BASE_ROCKET_COLOUR_R;
		base_rockets[i].colour[1] = BASE_ROCKET_COLOUR_G;
		base_rockets[i].colour[2] = BASE_ROCKET_COLOUR_B;
		base_rockets[i].active = false;
	}

	EnemyRocket enemy_rockets[MAX_ENEMY_ROCKETS];
	for (int i = 0; i < MAX_ENEMY_ROCKETS; i++) {
		enemy_rockets[i].rect.w = ENEMY_ROCKET_WIDTH;
		enemy_rockets[i].rect.h = ENEMY_ROCKET_HEIGHT;
		enemy_rockets[i].colour[0] = ENEMY_ROCKET_COLOUR_R;
		enemy_rockets[i].colour[1] = ENEMY_ROCKET_COLOUR_G;
		enemy_rockets[i].colour[2] = ENEMY_ROCKET_COLOUR_B;
		enemy_rockets[i].active = false;
	}

	int score;
	bool quit = false;
	bool end;
	bool click;
	int x;
	int y;
	int index;
	double last_shot;
	double current_time;

	while (!quit) {
		end = false;
		last_shot = get_time();
		score = 0;
		
		while (!SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] && !end) {
			SDL_FillRect(display, NULL, SDL_MapRGB(display->format, BACKGROUND_COLOUR_R, BACKGROUND_COLOUR_G, BACKGROUND_COLOUR_B));
			SDL_FillRect(display, &base.rect, SDL_MapRGB(display->format, base.colour[0], base.colour[1], base.colour[2]));

			current_time = get_time();
			if (current_time >= last_shot + get_shot_delay(score)) {
				fire_enemy_rocket(enemy_rockets, score);
				last_shot = current_time;
			}

			if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
				if (!click) {
					fire_base_rocket(base_rockets, x, y);
					click = true;
				}
			}
			else if (click) {
				click = false;
			}

			for (int i = 0; i < MAX_BASE_ROCKETS; i++) {
				if (base_rockets[i].active) {
					index = update_base_rocket(&base_rockets[i], enemy_rockets, display);
					if (index - NO_HIT) {
						if (index - OFFSCREEN) {
							enemy_rockets[index].active = false;
							score++;
						}
						base_rockets[i].active = false;
					}
				}
			}

			for (int i = 0; i < MAX_ENEMY_ROCKETS; i++) {
				if (enemy_rockets[i].active) {
					if (update_enemy_rocket(&enemy_rockets[i], base, display)) {
						end = true;
					}
				}
			}

			SDL_UpdateWindowSurface(window);
			SDL_PumpEvents();
		}

		printf("Score: %d\n", score);

		while(true) {
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R]) {
				for (int i = 0; i < MAX_ENEMY_ROCKETS; i++) {
					if (i < MAX_BASE_ROCKETS) {
						if (base_rockets[i].active) {
							base_rockets[i].active = false;
						}
					}
					if (enemy_rockets[i].active) {
						enemy_rockets[i].active = false;
					}
				}
				
				break;
			}
			else if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE]) {
				quit = true;
				break;
			}
			
			SDL_PumpEvents();
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}