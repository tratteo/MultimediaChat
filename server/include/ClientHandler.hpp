#pragma once
#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <list>
#include "../include/UserData.hpp"
#include "../../common/errhandler.hpp" 
#include "../include/ClientSessionData.hpp"
#include "../../common/Payloads.hpp"
#include "../../common/Packet.hpp"
#include "../../common/NetworkHandler.hpp"
#include "../include/DatabaseHandler.hpp"

#define BUF_SIZE 512

class ClientHandler
{
    public:
    ClientHandler(ClientSessionData *sessionData, DataBaseHandler *dataHandler, void ( *OnDisconnect )(ClientHandler* ));
    ~ClientHandler();
    void HandleConnection();
    void CloseConnection();

    private:
    int udpFd;
    struct sockaddr_in servaddr, cliaddr;
    ClientSessionData *sessionData;
    DataBaseHandler *dataHandler;
    bool shutdownReq = false;
    bool closedLocal = true;
    std::thread clientThread;
    std::thread udpThread;
    void ( *OnDisconnect )(ClientHandler*);
    void Loop();
};