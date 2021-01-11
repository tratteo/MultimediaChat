#pragma once
#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
#include <string.h>
#include <list>
#include "../include/UserData.hpp"
#include "../../common/errhandler.hpp" 
#include "../include/ClientSessionData.hpp"
#include "../../common/Payloads.cpp"
#include "../../common/Packet.cpp"
#include "../../common/ByteBufferReader.hpp"
#include "../include/DatabaseHandler.hpp"

#define BUF_SIZE 512

class ClientHandler
{
    public:
    ClientHandler(ClientSessionData *sessionData, DataBaseHandler *dataHandler, void ( *OnDisconnect )(ClientHandler* ));
    ~ClientHandler();
    void HandleConnection();

    private:
    
    ClientSessionData *sessionData;
    DataBaseHandler *dataHandler;


    bool shutdownReq = false;
    void CloseConnection();
    std::thread clientThread;
    void ( *OnDisconnect )(ClientHandler*);
    void Loop();
};