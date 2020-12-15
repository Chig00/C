#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL.h>

#define EXTRA_THREADS 3

typedef struct {
	bool** grid;
	int width;
	int height;
	int index;
	SDL_Surface* window_surface;
	SDL_Rect rect;
} ThreadPackage;

const char* const TITLE = "Distribution Visualiser by Chigozie Agomo";
const int WINDOW_POSITION[] = {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED};
const Uint32 WINDOW_FLAGS = SDL_WINDOW_FULLSCREEN;
const int QUIT = SDL_SCANCODE_ESCAPE;
const int RESET = SDL_SCANCODE_R;
const int PAUSE = SDL_SCANCODE_P;
const int DISTRIBUTIONS = 6;

double distribute(int distribution) {
	double sum;
	
	switch (distribution) {
		case 1:
			sum = 0;
			
			for (int i = 0; i < 5; i++) {
				sum += distribute(0) / 5;
			}
			
			return sum;
			
		case 2:
			sum = 0;
			
			while (distribute(0) < 0.99) {
				sum += 0.0005;
			}
			
			return sum;
			
		case 3:
			sum = 0.5;
			
			for (int i = -2;; i--) {
				double result = distribute(0);
				
				if (0.1 <= result && result < 0.5) {
					sum -= pow(2, i);
				}
				
				else if (0.5 <= result && result < 0.9) {
					sum += pow(2, i);
				}
				
				else {
					return sum;
				}
			}
			
		case 4:
			sum = 0;
			
			for (int i = 0; i < 12; i++) {
				sum += distribute(0) / 12;
			}
			
			return sum;
			
		case 5:
			return 1 / (10 * distribute(0) + 1) - 0.1;
			
		default:
			return (double)rand() / (RAND_MAX + 1);
	}
}

int update(void* thread_package) {
	bool** grid = ((ThreadPackage*)thread_package)->grid;
	int width = ((ThreadPackage*)thread_package)->width;
	int height = ((ThreadPackage*)thread_package)->height;
	int index = ((ThreadPackage*)thread_package)->index;
	
	for (int i = index; i < width; i += EXTRA_THREADS + 1) {
		for (int j = height - 1; j > 0; j--) {
			if (!grid[i][j] && grid[i][j - 1]) {
				grid[i][j] = true;
				grid[i][j - 1] = false;
			}
		}
	}
}

int blit(void* thread_package) {
	bool** grid = ((ThreadPackage*)thread_package)->grid;
	int width = ((ThreadPackage*)thread_package)->width;
	int height = ((ThreadPackage*)thread_package)->height;
	int index = ((ThreadPackage*)thread_package)->index;
	SDL_Surface* window_surface = ((ThreadPackage*)thread_package)->window_surface;
	SDL_Rect rect = ((ThreadPackage*)thread_package)->rect;
	
	for (int i = index; i < width; i += EXTRA_THREADS + 1) {
		for (int j = 0; j < height; j++) {
			if (grid[i][j]) {
				rect.x = i;
				rect.y = j;
				SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, 0xff, 0xff, 0xff));
			}
		}
	}
}

int main(int argc, char* argv[]) {
	srand(time(NULL));
	rand();
	
	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_DisplayMode display_mode;
	SDL_GetDesktopDisplayMode(0, &display_mode);
	
	SDL_Window* window =
		SDL_CreateWindow(
			TITLE, WINDOW_POSITION[0], WINDOW_POSITION[1],
			display_mode.w, display_mode.h, WINDOW_FLAGS
		)
	;
	SDL_Surface* window_surface = SDL_GetWindowSurface(window);
	
	bool** grid = malloc(sizeof(bool*) * display_mode.w);
	
	for (int i = 0; i < display_mode.w; i++) {
		grid[i] = malloc(sizeof(bool) * display_mode.h);
	}
	
	bool quit = false;
	SDL_Rect rect = {.w = 1, .h = 1};
	
	SDL_Thread* threads[EXTRA_THREADS];
	ThreadPackage thread_packages[] = 
		{
			{
				.grid = grid,
				.width = display_mode.w,
				.height = display_mode.h,
				.index = 0,
				.window_surface = window_surface,
				.rect = rect
			},
			{
				.grid = grid,
				.width = display_mode.w,
				.height = display_mode.h,
				.index = 1,
				.window_surface = window_surface,
				.rect = rect
			},
			{
				.grid = grid,
				.width = display_mode.w,
				.height = display_mode.h,
				.index = 2,
				.window_surface = window_surface,
				.rect = rect
			}
		}
	;
	
	int runs = 1;
	
	while (!quit) {
		for (int i = 0; i < display_mode.w; i++) {
			for (int j = 0; j < display_mode.h; j++) {
				grid[i][j] = false;
			}
		}
	
		int distribution = DISTRIBUTIONS * distribute(0);
		
		if (argc > 1) {
			if (runs < argc) {
				sscanf(argv[runs++], "%d", &distribution);
			}
		}
		
		while (true) {
			const Uint8* keyboard = SDL_GetKeyboardState(NULL);
			
			if ((quit = keyboard[QUIT]) || keyboard[RESET]) {
				break;
			}
			
			if (keyboard[PAUSE]) {
				while (SDL_GetKeyboardState(NULL)[PAUSE]) {
					SDL_PumpEvents();
				}
				
				bool end = false;
				
				while (!(keyboard = SDL_GetKeyboardState(NULL))[PAUSE]) {
					if ((quit = keyboard[QUIT]) || keyboard[RESET]) {
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
			
			
			for (int i = 0; i < EXTRA_THREADS; i++) {
				threads[i] = SDL_CreateThread(update, NULL, &thread_packages[i]);
			}
			
			for (int i = EXTRA_THREADS; i < display_mode.w; i += EXTRA_THREADS + 1) {
				for (int j = display_mode.h - 1; j > 0; j--) {
					if (!grid[i][j] && grid[i][j - 1]) {
						grid[i][j] = true;
						grid[i][j - 1] = false;
					}
				}
			}
			
			for (int i = 0; i < EXTRA_THREADS; i++) {
				SDL_WaitThread(threads[i], NULL);
			}
			
			double distribution_result = distribute(distribution);
			
			if (0 <= distribution_result && distribution_result < 1) {
				grid[(int)(display_mode.w * distribution_result)][0] = true;
			}
			
			SDL_FillRect(window_surface, NULL, SDL_MapRGB(window_surface->format, 0, 0, 0));
			
			for (int i = 0; i < EXTRA_THREADS; i++) {
				threads[i] = SDL_CreateThread(blit, NULL, &thread_packages[i]);
			}
			
			for (int i = EXTRA_THREADS; i < display_mode.w; i += EXTRA_THREADS + 1) {
				for (int j = 0; j < display_mode.h; j++) {
					if (grid[i][j]) {
						rect.x = i;
						rect.y = j;
						SDL_FillRect(window_surface, &rect, SDL_MapRGB(window_surface->format, 0xff, 0xff, 0xff));
					}
				}
			}
			
			for (int i = 0; i < EXTRA_THREADS; i++) {
				SDL_WaitThread(threads[i], NULL);
			}
			
			SDL_UpdateWindowSurface(window);
			SDL_PumpEvents();
		}
		
		while (!quit && SDL_GetKeyboardState(NULL)[RESET]) {
			SDL_PumpEvents();
		}
	}
	
	for (int i = 0; i < display_mode.w; i++) {
		free(grid[i]);
	}
	
	free(grid);
	
	SDL_DestroyWindow(window);
	SDL_Quit();
}