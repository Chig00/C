#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

typedef struct {
	SDL_Rect rect;
	int colour[3];
	double pos[2];
	double vel[2];
	int mass;
} Object;

const char* TITLE = "Gravity Simulator by Chigozie Agomo";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

const int BG_R = 0;
const int BG_G = 0;
const int BG_B = 0;

const int QUIT = SDL_SCANCODE_ESCAPE;
const int PAUSE = SDL_SCANCODE_P;

const double G = 6.67e-11;

const int OBJECT_WIDTH = 2;
const int OBJECT_HEIGHT = 2;
const int R_0 = 255;
const int G_0 = 0;
const int B_0 = 0;
const int R_1 = 0;
const int G_1 = 0;
const int B_1 = 255;

int main(int argc, char** argv) {
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);

	Object objects = {
		{
			.rect = {
				.w = OBJECT_WIDTH,
				.h = OBJECT_HEIGHT
			},
			.colour = {
				R_0,
				G_0,
				B_0
			}
		},
		{
			.rect = {
				.w = OBJECT_WIDTH,
				.h = OBJECT_HEIGHT
			},
			.colour = {
				R_1,
				G_1,
				B_1
			}
		}
	};

	for (int i = 0; i < 2; i++) {
		printf("Object %d's postion: ", i + 1);
		scanf("%f%f", &objects[i].pos[0], &objects[i].pos[1]);
		objects[i].rect.x = objects[i].pos[0] - objects[i].rect.w / 2;
		objects[i].rect.y = objects[i].pos[1] - objects[i].rect.h / 2;

		printf("Object %d's velocity: ", i + 1);
		scanf("%f%f", &objects[i].vel[0], &objects[i].vel[1]);

		printf("Objects %d's mass: ", i + 1);
		scanf("%d", &objects[i].mass);
	}

	bool quit = false;

	SDL_FillRect(display, NULL, SDL_MapRGB(display->format, BG_R, BG_G, BG_B));

	while (!quit) {
		quit = SDL_GetKeyboardState(NULL)[QUIT];

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

		for (int i = 0; i < 2; i++) {
			r2 = pow(objects[i].pos[0] - objects[1 - i].pos[0], 2) + pow(objects[i].pos[1] - objects[1 - i].pos[1], 2);
			acc = G * objects[1 - i].mass / r2;
			angle = atan2(objects[1 - i].pos[1] - objects[i].pos[1], objects[1 - i].pos[0] - objects[i].pos[0]);

			objects[i].vel[0] += acc * cos(angle);
			objects[i].vel[1] += acc * sin(angle);
		}

		for (int i = 0; i < 2; i++) {
			objects[i].pos[0] += objects[i].vel[0];
			objects[i].pos[1] += objects[i].vel[1];

			objects[i].rect.x = objects[i].pos[0] - objects[i].rect.w / 2;
			objects[i].rect.y = objects[i].pos[1] - objects[i].rect.h / 2;

			SDL_FillRect(display, &objects[i].rect, SDL_MapRGB(display->format, objects[i].colour[0], objects[i].colour[1], objects[i].colour[2]));

			if (SDL_HasIntersection(&objects[i].rect, &objects[1 - i].rect) || objects[i].rect.x < 0 || objects[i].rect.x > WINDOW_WIDTH - objects[i].rect.w
			    || objects[i].rect.y < 0 || objects[i].rect.y > WINDOW_HEIGHT - objects[i].rect.h) {
				quit = true;
				break;
			}
		}

		SDL_UpateWindowSurface(window);
		SDL_PumpEvents();
	}
}