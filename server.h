#include <string.h>
#include <SDL.h>
#include <SDL_net.h>

/* A structure that manages a server to connect
     to a single client
 */
typedef struct Server {
	IPaddress address;
	TCPsocket server;
	TCPsocket socket;
} Server;

#ifndef SDL_NETWORKING
#define SDL_NETWORKING
/* Initialises SDL with the given flags and
     intialises SDL Net after
 */
void sdlnet_init(int flags) {
	SDL_Init(flags);
	SDLNet_Init();
}

/* Shuts down SDL Net and SDL after
 */
void sdlnet_quit() {
	SDLNet_Quit();
	SDL_Quit();
}
#endif

/* Sets up the server and waits for a client at the given port
 */
void server_init(Server* server, int port) {
	SDLNet_ResolveHost(&server->address, NULL, port);
	server->server = SDLNet_TCP_Open(&server->address);
	while(!(server->socket = SDLNet_TCP_Accept(server->server)));
}

/* Shuts down the server
 */
void server_quit(const Server* server) {
	SDLNet_TCP_Close(server->socket);
	SDLNet_TCP_Close(server->server);
}

/* Sends a message to the client
 */
void server_send(const Server* server, const char* message) {
	SDLNet_TCP_Send(server->socket, message, strlen(message) + 1);
}

/* Receives a message from the client
   Stores the message in the given buffer
 */
void server_read(const Server* server, char* buffer, int buffer_size) {
	SDLNet_TCP_Recv(server->socket, buffer, buffer_size);
}