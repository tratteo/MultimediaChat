#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include <cmath>
#include "../include/Client.hpp"

void CloseService(int signal);

Client *client;

int main(int argv, char** argc)
{
	if (argv < 2)
	{
		handle_fatal_error("No arguments found");
	}

	signal(SIGINT, CloseService);
	signal(SIGTERM, CloseService);

	std::cout << "Multimedia Chat Client" << std::endl;

	client = new Client(argc[1]);
	client->Run();
	client->Join();
	CloseService(0);
}

void CloseService(int signal)
{
	std::cout<<std::endl<<"Cleaning junk..."<<std::endl;
	delete client;
	exit(EXIT_SUCCESS);
}
