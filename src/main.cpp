#include <unistd.h>
#include <iostream>
#include "CSocket.hpp"

int main(int argv, char** argc)
{
	CSocket* clientSocket;
	if (argv < 2)
	{
		handle_fatal_error("No arguments found");
	}

	clientSocket = new CSocket(argc[1]);
	clientSocket->init(SOCK_STREAM, 0);

	free(clientSocket);
	return EXIT_SUCCESS;
}