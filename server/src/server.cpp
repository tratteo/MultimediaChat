#include <iostream>
#include <unistd.h>
#include <list>
#include "../include/ClientHandler.hpp"
#include "../include/Serializer.hpp"
#include "../../common/UserData.hpp"
#include "../../common/CSocket.hpp"
#include "../include/DatabaseHandler.hpp"

void CloseService(int);
DataBaseHandler* dataHandler;
CSocket* servSocket;
std::list<ClientHandler*> handlers;

int main()
{
    //Making the main the smaller i can :-)
    
    servSocket = new CSocket(10000);

	signal(SIGINT, CloseService);
	signal(SIGTERM, CloseService);

    dataHandler = new DataBaseHandler();

    std::cout << "Server started" << std::endl;
    while(true)
    {
        ClientSessionData *data; 
        if((data = servSocket->AcceptConnection()) != nullptr)
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
    dataHandler->SerializeDatabase();
    std::cout<<"Cleaning all the junk..."<<std::endl;
    for (auto& h : handlers)
    {
        delete h;
    }
    delete servSocket;
    delete dataHandler;
    exit(EXIT_SUCCESS);
}