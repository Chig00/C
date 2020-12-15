#include <stdio.h> // Used to display the instructions
#include <stdlib.h> // Used for memory management
#include <stdbool.h> // Used for true/false valuation
#include <math.h> // Used for acceleration calculations
#include <SDL.h> // Used for graphics and event handling

// Structure for a charged particle to be simulated
typedef struct {
	double charge;
	double position[2];
	double velocity[2];
	int colour[3];
} Charge;

// Container for the charges
typedef struct {
	Charge* charges;
	int count;
} Charges;

// Constants
// General constants
const char* const TITLE = "Electric Force Simulator by Chigozie Agomo";
const int QUIT = SDL_SCANCODE_ESCAPE;
const int RESET = SDL_SCANCODE_R;
const int PAUSE = SDL_SCANCODE_P;

// Charge constants
const double CHARGE = 25;
const int PCHARGE = SDL_BUTTON_LEFT;
const int NCHARGE = SDL_BUTTON_RIGHT;
const int FULL_COLOUR = 0xff;
const int LIGHTENER = 0x7f;
const int CHARGE_WIDTH = 8;
const int CHARGE_HEIGHT = 8;
const double FAINT_CONSTANT = 0.25;
const double BASE_VELOCITY = 0.005;
const double MAX_ACCELERATION = 0.01;

void create(Charges* charges, SDL_Point mouse, Uint32 bitmask, SDL_Surface* display, SDL_Window* window) {
	// The buffer for the stored charges is resized for one more
	charges->charges = realloc(charges->charges, sizeof(Charge) * (charges->count + 1));
	// The new charge's variables are set
	// The charge is determined by whether a left (positive) or right (negative) click was used
	charges->charges[charges->count].charge = CHARGE * (!!(bitmask & SDL_BUTTON(PCHARGE)) - !!(bitmask & SDL_BUTTON(NCHARGE)));
	// The charge is spawned where the mouse clicked
	charges->charges[charges->count].position[0] = mouse.x;
	charges->charges[charges->count].position[1] = mouse.y;
	// The charge's colour is determined by its charge (red for positive, blue for negative)
	charges->charges[charges->count].colour[0] = FULL_COLOUR * !!(bitmask & SDL_BUTTON(PCHARGE));
	charges->charges[charges->count].colour[1] = LIGHTENER;
	charges->charges[charges->count].colour[2] = FULL_COLOUR * !!(bitmask & SDL_BUTTON(NCHARGE));
	
	// An SDL_Rect is created to display the new charge in the window
	SDL_Rect rect = {
		.x = charges->charges[charges->count].position[0] - CHARGE_WIDTH / 2,
		.y = charges->charges[charges->count].position[1] - CHARGE_HEIGHT / 2,
		.w = CHARGE_WIDTH,
		.h = CHARGE_HEIGHT
	};
	
	// The charge is blitted to the window and the window is updated
	SDL_FillRect(
		display,
		&rect,
		SDL_MapRGB(
			display->format,
			charges->charges[charges->count].colour[0],
			charges->charges[charges->count].colour[1],
			charges->charges[charges->count].colour[2]
		)
	);
	SDL_UpdateWindowSurface(window);
	
	// The charges's velocity is determined by the magnitude and direction of the pull back
	// The mouse's position is updated until the mouse buttons are both released
	while (SDL_GetMouseState(&mouse.x, &mouse.y) & (SDL_BUTTON(PCHARGE) | SDL_BUTTON(NCHARGE))) {
		SDL_PumpEvents();
	}
	
	// The velocity is stored
	charges->charges[charges->count].velocity[0] = BASE_VELOCITY * (charges->charges[charges->count].position[0] - mouse.x);
	charges->charges[charges->count].velocity[1] = BASE_VELOCITY * (charges->charges[charges->count].position[1] - mouse.y);
	
	// The number of charges is incremented
	charges->count++;
}

void update(Charges charges, SDL_DisplayMode display_mode, SDL_Surface* display) {
	for (int i = 0; i < charges.count; i++) {
		// The charge's old position is covered by a fainter colour to form a path trace
		SDL_Rect rect = {
			.x = charges.charges[i].position[0] - CHARGE_WIDTH / 2,
			.y = charges.charges[i].position[1] - CHARGE_HEIGHT / 2,
			.w = CHARGE_WIDTH,
			.h = CHARGE_HEIGHT
		};
		
		// The charge is blitted to the window's surface
		SDL_FillRect(
			display,
			&rect,
			SDL_MapRGB(
				display->format,
				charges.charges[i].colour[0] * FAINT_CONSTANT,
				charges.charges[i].colour[1] * FAINT_CONSTANT,
				charges.charges[i].colour[2] * FAINT_CONSTANT
			)
		);
		
		// The charge is accelerated by all of the other charges
		for (int j = 0; j < charges.count; j++) {
			if (i != j) {
				// The distances in the x and y axes are calculated, as well as the true separation
				double xseparation = charges.charges[i].position[0] - charges.charges[j].position[0];
				double yseparation = charges.charges[i].position[1] - charges.charges[j].position[1];
				double separation = pow(pow(xseparation, 2) + pow(yseparation, 2), 0.5);
				
				// If the separation is zero, the charge isn't accelerated to avoid division by zero errors
				if (separation) {
					// Force is directly proportional to the product of the charges and inversely proportional to the square of their separation
					double acceleration = charges.charges[i].charge * charges.charges[j].charge / pow(separation, 2);
					
					if (acceleration > MAX_ACCELERATION) {
						acceleration = MAX_ACCELERATION;
					}
					
					else if (acceleration < -MAX_ACCELERATION) {
						acceleration = -MAX_ACCELERATION;
					}
					
					// The charge is accelerated
					charges.charges[i].velocity[0] += acceleration * xseparation / separation;
					charges.charges[i].velocity[1] += acceleration * yseparation / separation;
				}
			}
		}
	}
	
	// Charges are only moved after all of them have been acceleated to maximise calculation validity
	for (int i = 0; i < charges.count; i++) {
		// The charges are accelerated
		charges.charges[i].position[0] += charges.charges[i].velocity[0];
		charges.charges[i].position[1] += charges.charges[i].velocity[1];
		
		// If a charge comes offscreen, its respective velocity component is reverses and the chare is brought back onscreen
		if (charges.charges[i].position[0] < 0) {
			charges.charges[i].position[0] = 0;
			charges.charges[i].velocity[0] *= -1;
		}
		
		else if (charges.charges[i].position[0] > display_mode.w) {
			charges.charges[i].position[0] = display_mode.w;
			charges.charges[i].velocity[0] *= -1;
		}
		
		if (charges.charges[i].position[1] < 0) {
			charges.charges[i].position[1] = 0;
			charges.charges[i].velocity[1] *= -1;
		}
		
		else if (charges.charges[i].position[1] > display_mode.h) {
			charges.charges[i].position[1] = display_mode.h;
			charges.charges[i].velocity[1] *= -1;
		}
		
		// The charge's new position is covered by its colour
		SDL_Rect rect = {
			.x = charges.charges[i].position[0] - CHARGE_WIDTH / 2,
			.y = charges.charges[i].position[1] - CHARGE_HEIGHT / 2,
			.w = CHARGE_WIDTH,
			.h = CHARGE_HEIGHT
		};
		
		// The charge is blitted to the window's surface
		SDL_FillRect(
			display,
			&rect,
			SDL_MapRGB(
				display->format,
				charges.charges[i].colour[0],
				charges.charges[i].colour[1],
				charges.charges[i].colour[2]
			)
		);
	}
}
	
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	// The computer's monitor's size is found
	SDL_DisplayMode display_mode;
	SDL_GetDesktopDisplayMode(0, &display_mode);
	
	// The window is created and its surface is stored
	SDL_Window* window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		display_mode.w, display_mode.h, SDL_WINDOW_SHOWN
	);
	SDL_Surface* display = SDL_GetWindowSurface(window);
	// A structure for the mouse's position
	SDL_Point mouse;
	// When this variable's value is true, the simulation ends
	bool quit = false;
	
	while (!quit) {
		// The window is cleared
		SDL_FillRect(display, NULL, SDL_MapRGB(display->format, 0, 0, 0));
		// The charge container is initialised
		Charges charges = {0};
		
		// The simulation takes place and takes user input
		while (true) {
			// Each charge is updated by accelerating them based on the position of every other charge
			update(charges, display_mode, display);
			
			// The mouse's location is stored and if the mouse clicked, a new charge is created
			Uint32 bitmask = SDL_GetMouseState(&mouse.x, &mouse.y);
			
			if (bitmask & (SDL_BUTTON(PCHARGE) | SDL_BUTTON(NCHARGE))) {
				create(&charges, mouse, bitmask, display, window);
			}
				
			// This variable represents whether the simulation should stop or not for use when the simulation is paused
			bool end = false;
			
			// If the user pressed p, the simulation pauses
			if (SDL_GetKeyboardState(NULL)[PAUSE]) {
				// The simulation waits for the player to let go of p
				while (SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}
				
				// The simulation waits for the player to either press p again or use escape or r to quit or reset the simulation respectively
				while (!SDL_GetKeyboardState(NULL)[PAUSE]) {
					if ((quit = SDL_GetKeyboardState(NULL)[QUIT]) || SDL_GetKeyboardState(NULL)[RESET]) {
						bool end = true;
						break;
					}
					
					// The mouse's location is stored and if the mouse clicked, a new charge is created
					bitmask = SDL_GetMouseState(&mouse.x, &mouse.y);
					
					if (bitmask & (SDL_BUTTON(PCHARGE) | SDL_BUTTON(NCHARGE))) {
						create(&charges, mouse, bitmask, display, window);
					}
					
					SDL_PumpEvents();
				}
				
				// The game waits for the pause button to be released if the user chose to continue the simulation
				while (SDL_GetKeyboardState(NULL)[PAUSE] && !end) {
					SDL_PumpEvents();
				}
			}
			
			// If the user pressed escape, the simulation ends; if they pressed r, the simulation resets
			if (end || (quit = SDL_GetKeyboardState(NULL)[QUIT]) || SDL_GetKeyboardState(NULL)[RESET]) {
				break;
			}
			
			// The simulation updates the window and the input devices
			SDL_UpdateWindowSurface(window);
			SDL_PumpEvents();
		}
		
		free(charges.charges);
		
		// The new round won't start until the user lets go of the reset key
		if (!quit && SDL_GetKeyboardState(NULL)[RESET]) {
			SDL_PumpEvents();
		}
	}
}