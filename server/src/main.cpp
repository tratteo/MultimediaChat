
#include "../include/SSocket.hpp"
#include "../include/ClientHandler.hpp"
#include <iostream>
#include <unistd.h>

void CloseService(int);

int main()
{
    SSocket *serverSocket = new SSocket();
    std::cout<<"Initializing server..."<<std::endl;
    serverSocket->init(SOCK_STREAM, 0);

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

    while(true)
    {
        ClientSessionData *data; 
        if((data=serverSocket->AcceptConnection()) != nullptr)
        {
            std::cout<<"Accepted"<<std::endl;
            ClientHandler *handler = new ClientHandler(data);
            handler->HandleConnection();
        }
    }
}

void CloseService(int signal)
{
    exit(EXIT_SUCCESS);
}