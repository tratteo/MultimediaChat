#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include "../include/CSocket.hpp"
#define BUF_SIZE 512

void CloseService(int);

CSocket* clientSocket;

int main(int argv, char** argc)
{
	if (argv < 2)
	{
		handle_fatal_error("No arguments found");
	}

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

	clientSocket = new CSocket(argc[1]);
	clientSocket->Init(SOCK_STREAM, 0);

	clientSocket->TryConnect();
	if(clientSocket->IsConnected())
	{
		std::cout<<"Connected"<<std::endl;
		char buf[BUF_SIZE];
		while(true)
		{
			fgets(buf, BUF_SIZE, stdin);
			write(clientSocket->GetFd(), buf, strlen(buf));
		}
	}
	return EXIT_SUCCESS;
}

void CloseService(int signal)
{
	char* closeMsg = "DC\n";
	write(clientSocket->GetFd(), closeMsg, strlen(closeMsg));
	free(clientSocket);
	exit(EXIT_SUCCESS);
}