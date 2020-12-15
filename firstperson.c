#include <stdbool.h>
#include <math.h>
#include <SDL.h>

const char* TITLE = "First Person Test by Chigozie Agomo";
const int WINDOW_W = 800;
const int WINDOW_H = 800;
const int QUIT = SDL_SCANCODE_ESCAPE;
const double PI = acos(-1);
const double SENSITIVITY = 0.001;
const int RECT_W = 100;
const int RECT_H = 100;
const int RECT_R = 255;
const int RECT_G = 255;
const int RECT_B = 255;

typedef struct {
	double x;
	double y;
} Player;

void look(Player* player, int x, int y) {
	player->x += (x - WINDOW_W / 2) * SENSITIVITY;
	player->y += (y - WINDOW_H / 2) * SENSITIVITY;
	
	while (player->x > PI) {
		player->x -= 2 * PI;
	}
	
	while (player->x < -PI) {
		player->x += 2 * PI;
	}
	
	if (player->y > PI / 2) {
		player->y = PI / 2;
	}
	
	else if(player->y < -PI / 2) {
		player->y = -PI / 2;
	}
}

void view(Player player, SDL_Surface* display) {
	if (fabs(player.x) < PI / 4 && fabs(player.y) < PI / 4) {
		SDL_Rect rect = {
			.w = RECT_W,
			.h = RECT_H
		};
		
		rect.x = WINDOW_W / 2 * (1 - player.x / (PI / 4)) - RECT_W / 2,
		rect.y = WINDOW_H / 2 * (1 - player.y / (PI / 4)) - RECT_H / 2,
		
		SDL_FillRect(display, &rect, SDL_MapRGB(display->format, RECT_R, RECT_G, RECT_B));
	}
}

int main(int argc, char** argv) {
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	
	Player player = {.x = 0, .y = 0};
	
	bool quit = false;
	
	while (!quit) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
		quit = SDL_GetKeyboardState(NULL)[QUIT];
		int x;
		int y;
		SDL_GetMouseState(&x, &y);
		
		look(&player, x, y);
		view(player, display);
		
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
		SDL_WarpMouseInWindow(window, WINDOW_W / 2, WINDOW_H / 2);
	}
}