#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>

#define TITLE "Duel by Chigozie Agomo"
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define QUIT_KEY SDL_SCANCODE_ESCAPE
#define BG_R 0
#define BG_G 0
#define BG_B 0
#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT 100
#define BASE_HP 100
#define RUN_SPEED 5
#define MAX_PLAYER_JUMPS 2
#define JUMP_SPEED -10
#define MAX_FALL_SPEED 25
#define GRAVITY 0.1
#define P1_R 255
#define P1_G 0
#define P1_B 0
#define P1_UP SDL_SCANCODE_W
#define P1_DOWN SDL_SCANCODE_S
#define P1_LEFT SDL_SCANCODE_A
#define P1_RIGHT SDL_SCANCODE_D
#define P1_ATTACK SDL_SCANCODE_Q
#define P2_R 0
#define P2_G 0
#define P2_B 255
#define P2_UP SDL_SCANCODE_UP
#define P2_DOWN SDL_SCANCODE_DOWN
#define P2_LEFT SDL_SCANCODE_LEFT
#define P2_RIGHT SDL_SCANCODE_RIGHT
#define P2_ATTACK SDL_SCANCODE_SLASH
#define SWORD_WIDTH 150
#define SWORD_HEIGHT 20
#define SWORD_R 150
#define SWORD_G 150
#define SWORD_B 150
#define SWORD_LIFE 0.1
#define SWORD_DEATH 0.5
#define DAMAGE 20

typedef struct {
	SDL_Rect rect;
	int colour[3];
	bool render;
	bool active;
	double deactivate;
} Sword;

typedef struct {
	Sword sword;
	SDL_Rect rect;
	int colour[3];
	double position[2];
	double velocity[2];
	int hp;
	int up;
	int down;
	int left;
	int right;
	int attack;
	int direction;
	int jumps;
	bool jumping;
	bool diving;
	bool attacking;
} Player;

double get_time() {
	return (double) clock() / CLOCKS_PER_SEC;
}

void activate_sword(Sword* sword) {
	sword->deactivate = get_time() + SWORD_LIFE;
	sword->active = true;
	sword->render = true;
}
void deactivate_sword(Sword* sword, double curr_time) {
	sword->active = false;
	sword->deactivate = curr_time + SWORD_DEATH;
}

void update_sword(Player* player, Player* other) {
	if (player->direction == -1) {
		player->sword.rect.x = player->rect.x - player->sword.rect.w;
	}
	else if (player->direction == 1) {
		player->sword.rect.x = player->rect.x + player->rect.w;
	}
	
	player->sword.rect.y = player->rect.y + (player->rect.h - player->sword.rect.h) / 2;
	
	double curr_time = get_time();
	
	if (SDL_HasIntersection(&player->sword.rect, &other->rect)) {
		other->hp -= DAMAGE;
		deactivate_sword(&player->sword, curr_time);
	}
	
	if (player->sword.deactivate < curr_time) {
		deactivate_sword(&player->sword, curr_time);
	}
}

bool update_player(Player* player, Player* other) {
	if (player->hp <= 0) {
		return true;
	}
	
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	
	player->velocity[0] = RUN_SPEED * (keyboard[player->right] - keyboard[player->left]);
	
	if (player->velocity[0]) {
		player->direction = player->velocity[0] / abs(player->velocity[0]);
	}
	
	if (keyboard[player->up]) {
		if (!player->jumping) {
			if (player->jumps) {
				player->velocity[1] = JUMP_SPEED;
				player->jumps--;
			}
			
			player->jumping = true;
		}
	}
	else if (player->jumping) {
		player->jumping = false;
	}
	
	if (player->rect.y == WINDOW_HEIGHT - PLAYER_HEIGHT) {
		player->jumps = MAX_PLAYER_JUMPS;
	}
	
	if (keyboard[player->down]) {
		if (!player->diving) {
			if (player->rect.y < WINDOW_HEIGHT - PLAYER_HEIGHT) {
				player->velocity[1] = MAX_FALL_SPEED;
			}
			
			player->diving = true;
		}
	}
	else if (player->diving) {
		player->diving = false;
	}
	
	player->velocity[1] += GRAVITY;
	
	if (player->velocity[1] > MAX_FALL_SPEED) {
		player->velocity[1] = MAX_FALL_SPEED;
	}
	
	player->position[0] += player->velocity[0];
	player->position[1] += player->velocity[1];
	
	if (player->position[0] < 0) {
		player->position[0] = 0;
	}
	else if(player->position[0] > WINDOW_WIDTH - PLAYER_WIDTH) {
		player->position[0] = WINDOW_WIDTH - PLAYER_WIDTH;
	}
	
	if (player->position[1] < 0) {
		player->position[1] = 0;
	}
	else if (player->position[1] > WINDOW_HEIGHT - PLAYER_HEIGHT) {
		player->position[1] = WINDOW_HEIGHT - PLAYER_HEIGHT;
	}
	
	player->rect.x = player->position[0];
	player->rect.y = player->position[1];
	
	if (keyboard[player->attack]) {
		if (!player->attacking) {
			if (!player->sword.active && get_time() >= player->sword.deactivate) {
				activate_sword(&player->sword);
			}
			
			player->attacking = true;
		}
	}
	else if (player->attacking) {
		player->attacking = false;
	}
	
	if (player->sword.active) {
		update_sword(player, other);
	}
	
	if (player->sword.render) {
		if (player->direction == -1) {
			player->sword.rect.x = player->rect.x - player->sword.rect.w;
		}
		else if (player->direction == 1) {
			player->sword.rect.x = player->rect.x + player->rect.w;
		}
		
		player->sword.rect.y = player->rect.y + (player->rect.h - player->sword.rect.h) / 2;
		
		if (player->sword.deactivate <= get_time()) {
			player->sword.render = false;
		}
	}
	
	return false;
}

int update_players(Player players[]) {
	if (update_player(&players[0], &players[1])) {
		return 2;
	}
	else if (update_player(&players[1], &players[0])) {
		return 1;
	}
	
	return 0;
}

void render_players(Player players[], SDL_Surface* display) {
	if (players[0].sword.render) {
		SDL_FillRect(
			display,
			&players[0].sword.rect,
			SDL_MapRGB(
				display->format,
				players[0].sword.colour[0],
				players[0].sword.colour[1],
				players[0].sword.colour[2]
			)
		);
	}
	
	if (players[1].sword.render) {
		SDL_FillRect(
			display,
			&players[1].sword.rect,
			SDL_MapRGB(
				display->format,
				players[1].sword.colour[0],
				players[1].sword.colour[1],
				players[1].sword.colour[2]
			)
		);
	}
	
	SDL_FillRect(
		display,
		&players[0].rect,
		SDL_MapRGB(
			display->format,
			players[0].colour[0],
			players[0].colour[1],
			players[0].colour[2]
		)
	);
	SDL_FillRect(
		display,
		&players[1].rect,
		SDL_MapRGB(
			display->format,
			players[1].colour[0],
			players[1].colour[1],
			players[1].colour[2]
		)
	);
}

int main(int argc, char** argv) {
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	
	Player players[] = {
		{
			.sword = {
				.rect = {.w = SWORD_WIDTH, .h = SWORD_HEIGHT},
				.colour = {SWORD_R, SWORD_G, SWORD_B},
				.render = false,
				.active = false,
				.deactivate = 0
			},
			.rect = {
				.x = 0,
				.y = WINDOW_HEIGHT - PLAYER_HEIGHT,
				.w = PLAYER_WIDTH,
				.h = PLAYER_HEIGHT
			},
			.colour = {P1_R, P1_G, P1_B},
			.position = {
				0,
				WINDOW_HEIGHT - PLAYER_HEIGHT
			},
			.velocity = {0, 0},
			.hp = BASE_HP,
			.up = P1_UP,
			.down = P1_DOWN,
			.left = P1_LEFT,
			.right = P1_RIGHT,
			.attack = P1_ATTACK,
			.direction = 1,
			.jumps = MAX_PLAYER_JUMPS,
			.jumping = false,
			.diving = false,
			.attacking = false
		},
		{
			.sword = {
				.rect = {.w = SWORD_WIDTH, .h = SWORD_HEIGHT},
				.colour = {SWORD_R, SWORD_G, SWORD_B},
				.render = false,
				.active = false,
				.deactivate = 0
			},
			.rect = {
				.x = WINDOW_WIDTH - PLAYER_WIDTH,
				.y = WINDOW_HEIGHT - PLAYER_HEIGHT,
				.w = PLAYER_WIDTH,
				.h = PLAYER_HEIGHT
			},
			.colour = {P2_R, P2_G, P2_B},
			.position = {
				WINDOW_WIDTH - PLAYER_WIDTH,
				WINDOW_HEIGHT - PLAYER_HEIGHT
			},
			.velocity = {0, 0},
			.hp = BASE_HP,
			.up = P2_UP,
			.down = P2_DOWN,
			.left = P2_LEFT,
			.right = P2_RIGHT,
			.attack = P2_ATTACK,
			.jumps = MAX_PLAYER_JUMPS,
			.direction = -1,
			.jumping = false,
			.diving = false,
			.attacking = false
		}
	};
	
	int winner = 0;
	
	while (!SDL_GetKeyboardState(NULL)[QUIT_KEY] && !winner) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, BG_R, BG_G, BG_B));
		winner = update_players(players);
		render_players(players, display);
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
	
	if (winner) {
		printf("This game's winner is player %d!\n", winner);
	}
	else {
		printf("No contest.\n");
	}
	
	while (!SDL_GetKeyboardState(NULL)[QUIT_KEY]) {
		SDL_PumpEvents();
	}
}