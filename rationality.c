#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

#define TITLE "Rationality Test by Chigozie Agomo"
#define QUIT_KEY SDL_SCANCODE_ESCAPE
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define BG_R 0
#define BG_G 0
#define BG_B 0
#define MAX_MARKERS 50
#define MARKER_WIDTH 2
#define MARKER_HEIGHT 2
#define M_R 255
#define M_G 255
#define M_B 255
#define SPEED 10

typedef struct {
	SDL_Rect rect;
	double pos[2];
	double vel[2];
	bool active;
} Marker;

void create_marker(Marker* markers, double angle) {
	for (int i = 0; i < MAX_MARKERS; i++) {
		if (!markers[i].active) {
			markers[i].rect.x = markers[i].pos[0] = (WINDOW_WIDTH - MARKER_WIDTH) / 2;
			markers[i].rect.y = markers[i].pos[1] = (WINDOW_HEIGHT - MARKER_HEIGHT) / 2;
			markers[i].rect.w = MARKER_WIDTH;
			markers[i].rect.h = MARKER_HEIGHT;
			markers[i].vel[0] = SPEED * cos(angle);
			markers[i].vel[1] = SPEED * sin(angle);
			markers[i].active = true;
			break;
		}
	}
}

void move_markers(Marker* markers) {
	for (int i = 0; i < MAX_MARKERS; i++) {
		if (markers[i].active) {
			markers[i].pos[0] += markers[i].vel[0];
			markers[i].pos[1] += markers[i].vel[1];
			markers[i].rect.x = markers[i].pos[0];
			markers[i].rect.y = markers[i].pos[1];
			
			if (
				markers[i].rect.x < 0 || markers[i].rect.x > WINDOW_WIDTH - MARKER_WIDTH
				|| markers[i].rect.y < 0 || markers[i].rect.y > WINDOW_HEIGHT - MARKER_HEIGHT
			) {
				markers[i].active = false;
			}
		}
	}
}

void render_markers(Marker* markers, SDL_Surface* display) {
	for (int i = 0; i < MAX_MARKERS; i++) {
		if (markers[i].active) {
			SDL_FillRect(display, &markers[i].rect, SDL_MapRGB(display->format, M_R, M_G, M_B));
		}
	}
}

void update_markers(Marker* markers, double angle, SDL_Surface* display) {
	create_marker(markers, angle);
	move_markers(markers);
	render_markers(markers, display);
}

int main(int argc, char** argv) {
	printf("Turn: ");
	double turn;
	scanf("%f", &turn);
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	Marker markers[MAX_MARKERS];
	
	for (double angle = 0; !SDL_GetKeyboardState(NULL)[QUIT_KEY]; angle += turn * 2 * acos(-1)) {
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, BG_R, BG_G, BG_B));
		update_markers(markers, angle, display);
		SDL_UpdateWindowSurface(window);
		SDL_PumpEvents();
	}
}