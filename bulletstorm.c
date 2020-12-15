#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL.h>

#define TITLE "Bullet Storm by Chigozie Agomo"
#define RESET SDL_SCANCODE_R
#define QUIT SDL_SCANCODE_ESCAPE
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define BG_R 0
#define BG_G 0
#define BG_B 0
#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 0.75
#define PLAYER_R 0
#define PLAYER_G 0
#define PLAYER_B 255
#define MAX_BULLETS 1000
#define MIN_AGE 5
#define BULLET_WIDTH 5
#define BULLET_HEIGHT 5
#define BULLET_R 255
#define BULLET_G 0
#define BULLET_B 0
#define INIT_DELAY 0.2
#define HALF_DELAY 100
#define INIT_SPEED 0.1
#define SPEED_BOOST 0.001

const double MAX_DEV = acos(-1) / 4;

typedef struct {
	SDL_Rect rect;
	double pos[2];
} Player;

typedef struct {
	SDL_Rect rect;
	double pos[2];
	double vel[2];
	double birth;
	bool active;
} Bullet;

int rand_int(int low, int high) {
	return low + (high - low + 1) * rand() / (RAND_MAX + 1);
}

double get_time() {
	return (double) clock() / CLOCKS_PER_SEC;
}

double get_delay(int score) {
	return INIT_DELAY * HALF_DELAY / (double) (score + HALF_DELAY);
}

double angle_shift() {
	return 2 * MAX_DEV * rand() / RAND_MAX - MAX_DEV;
}

void shoot_bullet(Bullet* bullets, int score) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!bullets[i].active) {
			if (rand_int(0, 1)) {
				bullets[i].rect.x = (WINDOW_WIDTH + BULLET_WIDTH) * rand_int(0, 1) - BULLET_WIDTH;
				bullets[i].rect.y = rand_int(-BULLET_HEIGHT, WINDOW_HEIGHT);
			}
			else {
				bullets[i].rect.y = (WINDOW_HEIGHT + BULLET_HEIGHT) * rand_int(0, 1) - BULLET_HEIGHT;
				bullets[i].rect.x = rand_int(-BULLET_WIDTH, WINDOW_WIDTH);
			}
			
			bullets[i].pos[0] = bullets[i].rect.x;
			bullets[i].pos[1] = bullets[i].rect.y;
			
			double angle = atan2(
				WINDOW_HEIGHT / 2 - (bullets[i].rect.y + bullets[i].rect.h / 2),
				WINDOW_WIDTH / 2 - (bullets[i].rect.x + bullets[i].rect.w / 2)
			);
			
			bullets[i].vel[0] = INIT_SPEED * (1 + SPEED_BOOST * score) * cos(angle + angle_shift());
			bullets[i].vel[1] = INIT_SPEED * (1 + SPEED_BOOST * score) * sin(angle + angle_shift());
			
			bullets[i].birth = get_time();
			
			bullets[i].active = true;
			
			break;
		}
	}
}

void move_player(Player* player) {
	player->pos[0] += PLAYER_SPEED * (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_RIGHT] - SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LEFT]);
	player->pos[1] += PLAYER_SPEED * (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_DOWN] - SDL_GetKeyboardState(NULL)[SDL_SCANCODE_UP]);
	
	if (player->pos[0] < 0) {
		player->pos[0] = 0;
	}
	else if (player->pos[0] > WINDOW_WIDTH - PLAYER_WIDTH) {
		player->pos[0] = WINDOW_WIDTH - PLAYER_WIDTH;
	}
	
	if (player->pos[1] < 0) {
		player->pos[1] = 0;
	}
	else if (player->pos[1] > WINDOW_HEIGHT - PLAYER_HEIGHT) {
		player->pos[1] = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}
	
	player->rect.x = player->pos[0];
	player->rect.y = player->pos[1];
}

void move_bullets(Bullet* bullets, int* score) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].active) {
			bullets[i].pos[0] += bullets[i].vel[0];
			bullets[i].pos[1] += bullets[i].vel[1];
			
			if (
				(bullets[i].pos[0] < 0 || bullets[i].pos[0] > WINDOW_WIDTH - BULLET_WIDTH || bullets[i].pos[1] < 0 || bullets[i].pos[1] > WINDOW_WIDTH - BULLET_WIDTH)
				&& get_time() - bullets[i].birth > MIN_AGE
			) {
				bullets[i].active = false;
				++*score;
				continue;
			}
			
			bullets[i].rect.x = bullets[i].pos[0];
			bullets[i].rect.y = bullets[i].pos[1];
		}
	}
}

void render_player(Player player, SDL_Surface* display) {
	SDL_FillRect(display, &player.rect, SDL_MapRGB(display->format, PLAYER_R, PLAYER_G, PLAYER_B));
}

void render_bullets(Bullet* bullets, SDL_Surface* display) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].active) {
			SDL_FillRect(display, &bullets[i].rect, SDL_MapRGB(display->format, BULLET_R, BULLET_G, BULLET_B));
		}
	}
}

bool end_check(Player player, Bullet* bullets) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].active) {
			if (SDL_HasIntersection(&player.rect, &bullets[i].rect)) {
				return true;
			}
		}
	}
	
	return false;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	
	Player player = {
		.rect = {
			.w = PLAYER_WIDTH,
			.h = PLAYER_HEIGHT
		}
	};
	
	Bullet bullets[MAX_BULLETS];
	
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].rect.w = BULLET_WIDTH;
		bullets[i].rect.h = BULLET_HEIGHT;
	}
	
	bool quit = false;
	
	while (!quit) {
		player.rect.x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2;
		player.rect.y = (WINDOW_HEIGHT - PLAYER_HEIGHT) / 2;
		player.pos[0] = player.rect.x;
		player.pos[1] = player.rect.y;
		
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].active) {
				bullets[i].active = false;
			}
		}
		
		int score = 0;
		bool end = false;
		double curr_time;
		double last_shot = get_time();
		
		while (!quit && !end) {
			curr_time = get_time();
			if (curr_time > last_shot + get_delay(score)) {
				shoot_bullet(bullets, score);
				last_shot = curr_time;
			}
			
			move_player(&player);
			move_bullets(bullets, &score);
			render_player(player, display);
			render_bullets(bullets, display);
			
			quit = SDL_GetKeyboardState(NULL)[QUIT];
			end = SDL_GetKeyboardState(NULL)[RESET];
			
			if (end_check(player, bullets)) {
				printf("Score: %d\nPress the reset key (r) to reset the game or the quit key (esc) to quit.\n", score);
				
				while (!quit && !end) {
					quit = SDL_GetKeyboardState(NULL)[QUIT];
					end = SDL_GetKeyboardState(NULL)[RESET];
					SDL_PumpEvents();
				}
			}
			
			SDL_UpdateWindowSurface(window);
			SDL_PumpEvents();
			SDL_FillRect(display, NULL, SDL_MapRGB(display->format, BG_R, BG_G, BG_B));
		}
		
		while (SDL_GetKeyboardState(NULL)[RESET]) {
			SDL_PumpEvents();
		}
	}
}