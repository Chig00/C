#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

const int DEFAULT_THREADS = 4;
const int TARGET = 100000000;

double get_time() {
	return (double) clock() / CLOCKS_PER_SEC;
}

int function(void* arguments) {
	for (int i = 0; i < TARGET; i++);
}

int main(int argc, char* argv[]) {
	int thread_count;
	
	if (argc == 2) {
		sscanf(argv[1], "%d", &thread_count);
	}
	
	else {
		thread_count = DEFAULT_THREADS;
	}
	
	double start = get_time();
	
	for (int i = 0; i < thread_count; i++) {
		function(NULL);
	}
	
	printf("Single threaded: %f\n", get_time() - start);
	
	start = get_time();
	
	SDL_Thread** threads = malloc(sizeof(SDL_Thread*) * thread_count);
	
	for (int i = 0; i < thread_count; i++) {
		threads[i] = SDL_CreateThread(function, NULL, NULL);
	}
	
	for (int i = 0; i < thread_count; i++) {
		SDL_WaitThread(threads[i], NULL);
	}
	
	printf("Multi threaded: %f\n", get_time() - start);
	
	free(threads);
}