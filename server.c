#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_net.h>

const char* const MESSAGE = "Server to Client";

/* Hosts a server
 */
int main(int argc, char* argv[]) {
	// SDL Net is initialised
	SDL_Init(0);
	SDLNet_Init();
	
	// The IP address is resolved
	IPaddress address;
	SDLNet_ResolveHost(&address, NULL, 1234);
	
	// A socket to accept connections is created
	TCPsocket server = SDLNet_TCP_Open(&address);
	
	// An incoming connection is accepted
	TCPsocket socket;
	printf("Waiting for the client...\n");
	while(!(socket = SDLNet_TCP_Accept(server)));
	printf("Client connected!\n");
	
	// A message is sent to the client
	SDLNet_TCP_Send(socket, MESSAGE, strlen(MESSAGE) + 1);
	
	// A message is received from the client
	char message[20];
	SDLNet_TCP_Recv(socket, message, 20);
	printf("%s\n", message);
	
	// SDL Net is shut down
	SDLNet_TCP_Close(socket);
	SDLNet_TCP_Close(server);
	SDLNet_Quit();
	SDL_Quit();
	
	return 0;
}