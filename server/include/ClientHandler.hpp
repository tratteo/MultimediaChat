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
#include "../../common/packets/CredentialsPacket.cpp"
#include "../../common/ByteBufferReader.hpp"

#define BUF_SIZE 512

class ClientHandler
{
    public:
    ClientHandler(ClientSessionData *sessionData, std::list<UserData*> users);
    ~ClientHandler();
    void HandleConnection();

    private:
    std::list<UserData*> users;
    ClientSessionData *sessionData;

    void LoginRoutine();
    bool shutdownReq = false;
    void CloseConnection();
    std::thread clientThread;

    void Loop();
    void Command(char* command);

};