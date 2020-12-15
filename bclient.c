#include <stdio.h>
#include <stdbool.h>
#include "client.h"

int main(int argc, char* argv[]) {
	char* address;
	int port;
	char message[1000];
	
	// SDL and SDL Net are initialised
	sdlnet_init(0);
	
	// If the IP address was given through the command line, it is used
	if (argc > 1) {
		address = argv[1];
		
		// If the port was given through the command line, it is used
		if (argc > 2) {
			sscanf(argv[2], "%d", &port);
		}
		
		// Else, it is prompted for
		else {
			printf("Port: ");
			fgets(message, 1000, stdin);
			sscanf(message, "%d", &port);
		}
	}
	
	// Else, the address and port are prompted for
	else {
		printf("Port: ");
		fgets(message, 1000, stdin);
		sscanf(message, "%d", &port);
		
		printf("IP Address: ");
		fgets(message, 1000, stdin);
		address = message;
	}
	
	// The client is initialised
	Client client;
	client_init(&client, address, port);
	
	while (true) {
		// A message is sent to the server
		printf("\nMessage for the server:\n");
		fgets(message, 1000, stdin);
		client_send(&client, message);
	}
	
	// The server and SDL are shut down
	client_quit(&client);
	sdlnet_quit();
	
	return 0;
}