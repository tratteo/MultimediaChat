#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
#include <string.h>
#include "../../common/errhandler.hpp" 
#include "../include/ClientSessionData.hpp"
#define BUF_SIZE 512

class ClientHandler
{
    public:
    ClientHandler(ClientSessionData *sessionData);
    ~ClientHandler();
    void HandleConnection();

    private:
    ClientSessionData *sessionData;

    bool shutdownReq = false;
    void CloseConnection();
    std::thread clientThread;

    void Loop();
    void Command(char* command);

};