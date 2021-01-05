#include "../include/ClientHandler.hpp"

ClientHandler::ClientHandler(ClientSessionData *sessionData)
{
    this->sessionData = sessionData;
}

ClientHandler::~ClientHandler()
{

}
void ClientHandler::CloseConnection()
{
    shutdownReq = true;
    if(close(sessionData->GetFd()) < 0)
    {
        handle_error("Unable to close client descriptor");
    }
    std::cout << "Client: "<< sessionData->GetIp() << " disconnected" <<std::endl;
}

void ClientHandler::Loop()
{
    char buf[BUF_SIZE] = {0};
    while(!shutdownReq)
    {
        int bytesRead = read(sessionData->GetFd(), buf, BUF_SIZE);
        if(bytesRead == -1)
        {
            handle_error("Unable to read");
        }
        else
        {
            std::cout << "Received: " << buf;
            Command(buf);
            memset(&buf, 0, sizeof(buf));
        }
    }
}

void ClientHandler::Command(char* command)
{
    std::string strMsg(command);
    if(strMsg == "DC\n")
    {
        CloseConnection();
    }    
}

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}