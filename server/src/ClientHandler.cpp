#include "../include/ClientHandler.hpp"

ClientHandler::ClientHandler(ClientSessionData *sessionData, std::list<UserData*> users)
{
    this->sessionData = sessionData;
    this->users = users;
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
    int bytesRead = -1;
    while (buf[0] != PACKET_CREDENTIALS)
    {
        bytesRead = Read(buf, BUF_SIZE, sessionData->GetFd());
    }
    char* packetByteBuf = new char[bytesRead];
    for (int i = 0; i < bytesRead; i++)
    {
        packetByteBuf[i] = buf[i];
    }
    std::cout << "Read: " << bytesRead << std::endl;
    CredentialsPacket* packet = new CredentialsPacket(packetByteBuf);

    std::cout << "Received: " << packet->GetUsername() << ", " << packet->GetPassword() << std::endl;

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

void ClientHandler::LoginRoutine()
{
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