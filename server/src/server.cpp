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

int main()
{
    SSocket *serverSocket = new SSocket();
    std::cout<<"Initializing server..."<<std::endl;
    serverSocket->init(SOCK_STREAM, 0);

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

    dataHandler = new DataBaseHandler();


    while(true)
    {
        ClientSessionData *data; 
        if((data=serverSocket->AcceptConnection()) != nullptr)
        {
            std::cout<<"Accepted"<<std::endl;
            ClientHandler *handler = new ClientHandler(data, dataHandler);
            handler->HandleConnection();
        }
    }
}

void CloseService(int signal)
{
    delete dataHandler;
    exit(EXIT_SUCCESS);
}