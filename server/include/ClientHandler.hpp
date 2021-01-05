#include <thread>
#include <stdio.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <signal.h>
#include "../../common/errhandler.hpp"
#include <string.h> 
#define BUF_SIZE 512

class ClientHandler
{
    public:
    ClientHandler(int clientFd);
    ~ClientHandler();
    void HandleConnection();

    private:
    bool shutdownReq = false;
    void CloseConnection();
    std::thread clientThread;
    int clientFd;
    void Loop();
    void Command(char* command);

};