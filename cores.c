#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

const int TARGET = 100000000;

double now() {
	return (double) clock() / CLOCKS_PER_SEC;
}

int function(void* arguments) {
	for (int i = 0; i < TARGET; i++);
}

int main(int argc, char* argv[]) {
	double best = 0;
	SDL_Thread** threads = NULL;
	
	for (int i = 1;; i++) {
		double start = now();
		
		for (int j = 0; j < i; j++) {
			function(NULL);
		}
		
		double single = now() - start;
		
		start = now();
		threads = realloc(threads, sizeof(SDL_Thread*) * (i - 1));
		
		for (int j = 0; j < i - 1; j++) {
			threads[j] = SDL_CreateThread(function, NULL, NULL);
		}
		
		function(NULL);
		
		for (int j = 0; j < i - 1; j++) {
			SDL_WaitThread(threads[j], NULL);
		}
		
		double multi = now() - start;
		
		if (single / multi > best) {
			best = single / multi;
		}
		
		else {
			printf("Cores: %d\n", i - 1);
			break;
		}
	}
	
	free(threads);
}