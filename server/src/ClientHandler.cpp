#include "../include/ClientHandler.hpp"

ClientHandler::ClientHandler(int clientFd)
{
    this->clientFd = clientFd;
}

ClientHandler::~ClientHandler()
{

}
void ClientHandler::CloseConnection()
{
    shutdownReq = true;
    if(close(clientFd) < 0)
    {
        handle_error("Unable to close client descriptor");
    }
}

void ClientHandler::Loop()
{
    char buf[BUF_SIZE] = {0};
    while(!shutdownReq)
    {
        int bytesRead = read(clientFd, buf, BUF_SIZE);
        if(bytesRead == -1)
        {
            handle_error("Unable to read");
        }
        else
        {
            std::cout << "Received: " << buf << std::endl;
            Command(buf);
            memset(&buf, 0, sizeof(buf));
        }
    }
}

void ClientHandler::Command(char* command)
{
    std::string strMsg(command);
    if(strMsg == "DC")
    {
        CloseConnection();
    }    
}

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}