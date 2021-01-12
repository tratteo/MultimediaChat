#pragma once
#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
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
    ClientSessionData *sessionData;
    DataBaseHandler *dataHandler;
    bool shutdownReq = false;
    std::thread clientThread;
    void ( *OnDisconnect )(ClientHandler*);
    void Loop();
};