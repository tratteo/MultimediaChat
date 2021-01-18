#include <iostream>
#include <unistd.h>
#include <list>
#include "../include/ClientHandler.hpp"
#include "../include/Serializer.hpp"
#include "../../common/UserData.hpp"
#include "../../common/CSocket.hpp"
#include "../include/DatabaseHandler.hpp"

void CloseService(int);
void OnHandlerDisconnect(ClientHandler* handler);
DataBaseHandler* dataHandler;
CSocket* servSocket;
std::list<ClientHandler*> handlers;

int main()
{
    servSocket = new CSocket(8080);

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

    dataHandler = new DataBaseHandler();

    std::cout << "Server started" << std::endl;
    while(true)
    {
        ClientSessionData *data; 
        if((data=servSocket->AcceptConnection()) != nullptr)
        {
            std::cout << "New connection accepted, handling..." << std::endl;
            ClientHandler *handler = new ClientHandler(data, dataHandler, OnHandlerDisconnect);
            std::thread([&](){handler->HandleConnection();}).detach();
            //handler->HandleConnection();
            handlers.push_front(handler);
            dataHandler->UserConnected(data);
        }
    }
}

void OnHandlerDisconnect(ClientHandler* handler)
{
    handlers.remove(handler);
}

void CloseService(int signal)
{
    delete servSocket;
    delete dataHandler;
    for (auto& h : handlers)
    {
        delete h;
    }

    exit(EXIT_SUCCESS);
}