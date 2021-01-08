#include "../include/ClientHandler.hpp"

ClientHandler::ClientHandler(ClientSessionData *sessionData, DataBaseHandler *dataHandler)
{
    this->sessionData = sessionData;
    this->dataHandler = dataHandler;
}

ClientHandler::~ClientHandler()
{
    CloseConnection();
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
    int bytesRead;
    while (!shutdownReq)
    {
        bytesRead = Read(buf, BUF_SIZE, sessionData->GetFd());
        if (bytesRead > 0)
        {
            switch ((uint16_t)buf[0])
            {
                case PAYLOAD_DISCONNECT:
                {
                    delete this;
                    break;
                }
                case PAYLOAD_CREDENTIALS:
                {
                    sessionData->logged = true;
                    Packet* packet = new Packet(buf);
                    CredentialsPayload credentials;
                    credentials.Deserialize(packet->GetData());

                    std::cout << "Username: " << credentials.username << ", Password: " << credentials.password << std::endl;
                    UserData* user = new UserData(credentials.username, credentials.password);
                    dataHandler->AddUser(user);

                    delete packet;
                    delete user;
                    break;
                }
                case PAYLOAD_MSG:
                {
                    if (sessionData->logged)
                    {
                        //TODO redirect message
                    }
                    break;
                }
                default:
                    break;
            }
            memset(&buf, 0, BUF_SIZE);
        }

    }   
}

void ClientHandler::LoginRoutine()
{
}

void ClientHandler::ReceiveDeamon()
{

}

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}