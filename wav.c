#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

const char* const DEF_SONG = "ssbmbf.wav";
const int DELAY = 10000;

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_AUDIO);
	const char* song_name;
	
	switch (argc) {
		case 1:
			song_name = DEF_SONG;
			break;
			
		case 2:
			song_name = argv[1];
			break;
			
		default:
			printf("Excessive file input.\n");
			return EXIT_FAILURE;
	}
	
	SDL_AudioSpec song;
	Uint8* buf;
	Uint32 len;
	SDL_LoadWAV(song_name, &song, &buf, &len);
	SDL_OpenAudio(&song, NULL);
	SDL_PauseAudio(false);
	SDL_QueueAudio(1, buf, len);
	SDL_QueueAudio(1, buf, len);
	while (true);
}