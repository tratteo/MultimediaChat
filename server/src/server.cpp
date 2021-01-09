#pragma once
#include <iostream>
#include <unistd.h>
#include <list>
#include "../include/SSocket.hpp"
#include "../include/ClientHandler.hpp"
#include "../include/Serializer.hpp"
#include "../include/UserData.hpp"
#include "../include/DatabaseHandler.hpp"

void CloseService(int);

DataBaseHandler* dataHandler;
std::list<ClientHandler*> handlers;

int main()
{
    SSocket *serverSocket = new SSocket();
    std::cout << "Initializing server..." << std::endl;
    serverSocket->Init(SOCK_STREAM, 0);

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

    dataHandler = new DataBaseHandler();

    std::cout << "Server started" << std::endl;
    while(true)
    {
        ClientSessionData *data; 
        if((data=serverSocket->AcceptConnection()) != nullptr)
        {
            std::cout << "New connection accepted, handling..." << std::endl;
            ClientHandler *handler = new ClientHandler(data, dataHandler);
            handler->HandleConnection();
            handlers.push_front(handler);
        }
    }
}

void CloseService(int signal)
{
    delete dataHandler;
    for (auto& h : handlers)
    {
        delete h;
    }

    exit(EXIT_SUCCESS);
}