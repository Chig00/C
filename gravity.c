#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>

typedef struct {
	double position[2];
	double velocity[2];
} Mass;

typedef struct {
	Mass* masses;
	int count;
} Masses;

const char* const TITLE = "Gravity Simulator by Chigozie Agomo";
const int BACKGROUND[] = {0, 0, 0};
const int CLICK = SDL_BUTTON_LEFT;
const int MASS_WIDTH = 8;
const int MASS_HEIGHT = 8;
const int MASS_COLOUR[] = {0xff, 0xff, 0xff};
const int TRACE_COLOUR[] = {0x80, 0x80, 0x80};
const double BASE_VELOCITY = 0.005;
const double BASE_MASS = 1000;
const double MAX_ACCELERATION = 0.01;
const int QUIT = SDL_SCANCODE_ESCAPE;
const int RESET = SDL_SCANCODE_R;
const int PAUSE = SDL_SCANCODE_P;

void create(Masses* masses, SDL_Point mouse, SDL_Surface* window_surface, SDL_Window* window) {
	masses->masses = realloc(masses->masses, sizeof(Mass) * (masses->count + 1));
	
	masses->masses[masses->count].position[0] = mouse.x;
	masses->masses[masses->count].position[1] = mouse.y;
	
	SDL_Rect rect = {
		.x = mouse.x - MASS_WIDTH / 2,
		.y = mouse.y - MASS_HEIGHT / 2,
		.w = MASS_WIDTH,
		.h = MASS_HEIGHT
	};
	
	SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, MASS_COLOUR[0], MASS_COLOUR[1], MASS_COLOUR[2]));
	SDL_UpdateWindowSurface(window);
	
	while (SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(CLICK)) {
		SDL_PumpEvents();
	}
	
	masses->masses[masses->count].velocity[0] = (masses->masses[masses->count].position[0] - mouse.x) * BASE_VELOCITY;
	masses->masses[masses->count].velocity[1] = (masses->masses[masses->count].position[1] - mouse.y) * BASE_VELOCITY;
	
	masses->count++;
}

void update(Masses masses, SDL_Surface* window_surface) {
	for (int i = 0; i < masses.count; i++) {
		SDL_Rect rect = {
			.x = masses.masses[i].position[0] - MASS_WIDTH / 2,
			.y = masses.masses[i].position[1] - MASS_HEIGHT / 2,
			.w = MASS_WIDTH,
			.h = MASS_HEIGHT
		};
	
		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, TRACE_COLOUR[0], TRACE_COLOUR[1], TRACE_COLOUR[2]));
		
		for (int j = 0; j < masses.count; j++) {
			if (i != j) {
				double x_separation = masses.masses[j].position[0] - masses.masses[i].position[0];
				double y_separation = masses.masses[j].position[1] - masses.masses[i].position[1];
				double separation2 = x_separation * x_separation + y_separation * y_separation;
				
				if (separation2) {
					double acceleration = BASE_MASS / separation2;
					
					if (acceleration > MAX_ACCELERATION) {
						acceleration = MAX_ACCELERATION;
					}
					
					else if (acceleration < -MAX_ACCELERATION) {
						acceleration = -MAX_ACCELERATION;
					}
					
					masses.masses[i].velocity[0] += acceleration * x_separation / pow(separation2, 0.5);
					masses.masses[i].velocity[1] += acceleration * y_separation / pow(separation2, 0.5);
				}
			}
		}
	}
	
	for (int i = 0; i < masses.count; i++) {
		masses.masses[i].position[0] += masses.masses[i].velocity[0];
		masses.masses[i].position[1] += masses.masses[i].velocity[1];
		
		if (masses.masses[i].position[0] < 0) {
			masses.masses[i].position[0] = 0;
			masses.masses[i].velocity[0] *= -1;
		}
		
		else if (masses.masses[i].position[0] > window_surface->w) {
			masses.masses[i].position[0] = window_surface->w;
			masses.masses[i].velocity[0] *= -1;
		}
		
		if (masses.masses[i].position[1] < 0) {
			masses.masses[i].position[1] = 0;
			masses.masses[i].velocity[1] *= -1;
		}
		
		else if (masses.masses[i].position[1] > window_surface->h) {
			masses.masses[i].position[1] = window_surface->h;
			masses.masses[i].velocity[1] *= -1;
		}
		
		SDL_Rect rect = {
			.x = masses.masses[i].position[0] - MASS_WIDTH / 2,
			.y = masses.masses[i].position[1] - MASS_HEIGHT / 2,
			.w = MASS_WIDTH,
			.h = MASS_HEIGHT
		};
		
		SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, MASS_COLOUR[0], MASS_COLOUR[1], MASS_COLOUR[2]));
	}
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_DisplayMode display_mode;
	SDL_GetDesktopDisplayMode(0, &display_mode);
	
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		display_mode.w, display_mode.h, SDL_WINDOW_FULLSCREEN
	);
	SDL_Surface* window_surface = SDL_GetWindowSurface(window);
	
	SDL_Point mouse;
	bool quit = false;
	
	while (!quit) {
		SDL_FillRect(window_surface, NULL, SDL_MapRGB(window_surface->format, BACKGROUND[0], BACKGROUND[1], BACKGROUND[2]));
		Masses masses = {0};
		
		while (true) {
			const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
			
			update(masses, window_surface);
			
			if (SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(CLICK)) {
				create(&masses, mouse, window_surface, window);
			}
			
			if ((quit = keyboard_state[QUIT]) || keyboard_state[RESET]) {
				break;
			}
			
			if (keyboard_state[PAUSE]) {
				while (SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}
					
				bool end = false;
				
				while (!(keyboard_state = SDL_GetKeyboardState(NULL))[PAUSE]) {
					if (SDL_GetMouseState(&mouse.x, &mouse.y) & SDL_BUTTON(CLICK)) {
						create(&masses, mouse, window_surface, window);
					}
					
					if ((quit = keyboard_state[QUIT]) || keyboard_state[RESET]) {
						end = true;
						break;
					}
					
					SDL_PumpEvents();
				}
				
				if (end) {
					break;
				}
				
				while (SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}
			}
			
			SDL_UpdateWindowSurface(window);
			SDL_PumpEvents();
		}
		
		free(masses.masses);
		
		while (!quit && SDL_GetKeyboardState(NULL)[RESET]) {
			SDL_PumpEvents();
		}
	}
}