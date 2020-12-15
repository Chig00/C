#include <SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

const char* TITLE = "Mobile First Person by Chigozie Agomo";
const int WINDOW_W = 800;
const int WINDOW_H = 800;
const int QUIT = SDL_SCANCODE_ESCAPE;
const double MOV_SPD = 0.005;
const int MOV_F = SDL_SCANCODE_W;
const int MOV_L = SDL_SCANCODE_A;
const int MOV_B = SDL_SCANCODE_S;
const int MOV_R = SDL_SCANCODE_D;
const int MOV_U = SDL_SCANCODE_SPACE;
const int MOV_D = SDL_SCANCODE_C;
const double SENSITIVITY = 0.001;
const double PI = acos(-1);
const int OBJ_X = 0;
const int OBJ_Y = 0;
const int OBJ_Z = 10;
const int RECT_W = 100000;
const int RECT_H = 100000;

typedef struct {
	double pos_x;
	double pos_y;
	double pos_z;
	double view_x;
	double view_y;
} Player;

void move(Player* player) {
	player->pos_x += MOV_SPD * (SDL_GetKeyboardState(NULL)[MOV_R] - SDL_GetKeyboardState(NULL)[MOV_L]);
	player->pos_y += MOV_SPD * (SDL_GetKeyboardState(NULL)[MOV_D] - SDL_GetKeyboardState(NULL)[MOV_U]);
	player->pos_z += MOV_SPD * (SDL_GetKeyboardState(NULL)[MOV_F] - SDL_GetKeyboardState(NULL)[MOV_B]);
}

void look(Player* player, int m_x, int m_y) {
	player->view_x += SENSITIVITY * (m_x - WINDOW_W / 2);
	player->view_y += SENSITIVITY * (m_y - WINDOW_H / 2);
	
	while (player->view_x > PI) {
		player->view_x -= 2 * PI;
	}
	
	while (player->view_x < -PI) {
		player->view_x += 2 * PI;
	}
	
	if (player->view_y > PI / 2) {
		player->view_y = PI / 2;
	}
	
	else if (player->view_y < -PI / 2) {
		player->view_y = -PI / 2;
	}
}

void update(Player* player) {
	int m_x;
	int m_y;
	SDL_GetMouseState(&m_x, &m_y);
	
	move(player);
	look(player, m_x, m_y);
}

void view(Player player, SDL_Surface* display) {
	double distance = pow(pow(player.pos_x - OBJ_X, 2) + pow(player.pos_y - OBJ_Y, 2) + pow(player.pos_z - OBJ_Z, 2), 2);
	SDL_Rect rect;
	rect.w = RECT_W / distance;
	rect.h = RECT_H / distance;
	
	double ang_x = atan2(OBJ_X - player.pos_x, OBJ_Z - player.pos_z) - player.view_x;
	double ang_y = atan2(OBJ_Y - player.pos_y, OBJ_Z - player.pos_z) - player.view_y;
	
	rect.x = WINDOW_W / 2 * (1 + ang_x / (PI / 3)) - rect.w;
	rect.y = WINDOW_H / 2 * (1 + ang_y / (PI / 3)) - rect.h;
	
	SDL_FillRect(display, &rect, SDL_MapRGB(display->format, 255, 255, 255));
}

int main(int argc, char** argv) {
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	Player player = {0};
	bool quit = false;
	
	while (!quit) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
		SDL_WarpMouseInWindow(window, WINDOW_W / 2, WINDOW_H / 2);
		
		quit = SDL_GetKeyboardState(NULL)[QUIT];
		
		update(&player);
		view(player, display);
		
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
}