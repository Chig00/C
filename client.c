#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_net.h>

const char* const MESSAGE = "Client to Server";

/* Hosts a server
 */
int main(int argc, char* argv[]) {
	// SDL Net is initialised
	SDL_Init(0);
	SDLNet_Init();
	
	// The IP address is resolved
	IPaddress address;
	SDLNet_ResolveHost(&address, "127.0.0.1", 1234);
	
	// A socket to to connect to the server is created
	TCPsocket socket = SDLNet_TCP_Open(&address);
	
	// A message is sent to the server
	SDLNet_TCP_Send(socket, MESSAGE, strlen(MESSAGE) + 1);
	
	// A message is received from the server
	char message[20];
	SDLNet_TCP_Recv(socket, message, 20);
	printf("%s\n", message);
	
	// SDL Net is shut down
	SDLNet_TCP_Close(socket);
	SDLNet_Quit();
	SDL_Quit();
	
	return 0;
}