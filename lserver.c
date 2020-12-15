#include <stdio.h>
#include <stdbool.h>
#include "server.h"

int main(int argc, char* argv[]) {
	int port;
	char message[1000];
	
	// SDL and SDL Net are initialised
	sdlnet_init(0);
	
	// If the port was given through the command line, it is used
	if (argc > 1) {
		sscanf(argv[1], "%d", &port);
	}
	
	// Else, it is prompted for
	else {
		printf("Port: ");
		fgets(message, 1000, stdin);
		sscanf(message, "%d", &port);
	}
	
	// The server is initialised
	printf("Waiting for the client...\n");
	Server server;
	server_init(&server, port);
	
	while (true) {
		// The message from the client is displayed
		server_read(&server, message, 1000);
		printf("\nMessage from the client:\n%s", message);
	}
	
	// The server and SDL are shut down
	server_quit(&server);
	sdlnet_quit();
	
	return 0;
}