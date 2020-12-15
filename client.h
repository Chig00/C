#include <string.h>
#include <SDL.h>
#include <SDL_net.h>

/* A structure that manages a client to connect
     to a single server
 */
typedef struct Client {
	IPaddress address;
	TCPsocket socket;
} Client;

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

/* Sets up the client
 */
void client_init(Client* client, const char* address, int port) {
	SDLNet_ResolveHost(&client->address, address, port);
	client->socket = SDLNet_TCP_Open(&client->address);
}

/* Shuts down the client
 */
void client_quit(const Client* client) {
	SDLNet_TCP_Close(client->socket);
}

/* Sends a message to the server
 */
void client_send(const Client* client, const char* message) {
	SDLNet_TCP_Send(client->socket, message, strlen(message) + 1);
}

/* Receives a message from the server
   Stores the message in the given buffer
 */
void client_read(const Client* client, char* buffer, int buffer_size) {
	SDLNet_TCP_Recv(client->socket, buffer, buffer_size);
}