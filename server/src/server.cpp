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
std::list<std::thread> clientThreads;

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
        if((data = servSocket->AcceptConnection()) != nullptr)
        {
            std::cout << "New connection accepted, handling..." << std::endl;
            ClientHandler *handler = new ClientHandler(data, dataHandler, OnHandlerDisconnect);
            clientThreads.push_back(std::thread([&](){handler->HandleConnection();}));
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
    dataHandler->SerializeDatabase();
    for(auto &t : clientThreads)
    {
        if(t.joinable())
        {
            t.join();
        }
    }
    for (auto& h : handlers)
    {
        delete h;
    }
    delete servSocket;
    delete dataHandler;
    exit(EXIT_SUCCESS);
}