#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <SDL.h>

const char* const DEF_IMG = "cif.cif";
const int QUIT = SDL_SCANCODE_ESCAPE;

int main(int argc, char* argv[]) {
	const char* img_name;
	
	switch (argc) {
		case 1:
			img_name = DEF_IMG;
			break;
			
		case 2:
			img_name = argv[1];
			break;
			
		default:
			printf("Excessive file input.\n");
			return EXIT_FAILURE;
	}
	
	FILE* img = fopen(img_name, "r");
	
	if (!img) {
		printf("Invalid image name.\n");
		return errno;
	}
	
	int win_w;
	int win_h;
	int pix_pr;
	int pix_pc;
	fscanf(img, "%d%d%d%d", &win_w, &win_h, &pix_pr, &pix_pc);
	SDL_Window* win = SDL_CreateWindow(
		img_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		win_w, win_h, SDL_WINDOW_SHOWN
	);
	SDL_Surface* disp = SDL_GetWindowSurface(win);
	SDL_Rect rect = {
		.w = win_w / pix_pr,
		.h = win_h / pix_pc
	};
	
	for (int i = 0; i < pix_pc; i++) {
		for (int j = 0; j < pix_pr; j++) {
			if (feof(img)) {
				printf("Invalid image.\n");
				return EXIT_FAILURE;
			}
			
			rect.x = j * rect.w;
			rect.y = i * rect.h;
			int r;
			int g;
			int b;
			fscanf(img, "%d%d%d", &r, &g, &b);
			SDL_FillRect(disp, &rect, SDL_MapRGB(disp->format, r, g, b));
			SDL_UpdateWindowSurface(win);
		}
	}
	
	while (!SDL_GetKeyboardState(NULL)[QUIT]) {
		SDL_PumpEvents();
	}
}