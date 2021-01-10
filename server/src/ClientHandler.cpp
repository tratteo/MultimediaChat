#include "../include/ClientHandler.hpp"

ClientHandler::ClientHandler(ClientSessionData *sessionData, DataBaseHandler *dataHandler, void (*OnDisconnect)(ClientHandler*))
{
    this->sessionData = sessionData;
    this->dataHandler = dataHandler;
    this->OnDisconnect = OnDisconnect;
}

ClientHandler::~ClientHandler()
{
    CloseConnection();
    delete sessionData;
}

void ClientHandler::CloseConnection()
{
    shutdownReq = true;
    
    Packet packet;
    packet.Create(PAYLOAD_DISCONNECT);
    try
    {
        Write(packet.Serialize(), packet.GetTotalLength(), sessionData->GetFd());
    }
    catch (std::exception e)
    {

    }

    if(close(sessionData->GetFd()) < 0)
    {
        handle_error("Unable to close client descriptor");
    }
    if (sessionData->GetOwner() != nullptr)
    {
        std::cout << "User " << sessionData->GetOwner()->GetUsername() << " disconnected" << std::endl;
    }
    else
    {
        std::cout << "Connection " << sessionData->GetIp() << " disconnected without logging in" << std::endl;
    }

    OnDisconnect(this);
    dataHandler->UserDisconnected(sessionData);
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
                    Packet packet;
                    CredentialsPayload credentials;
                    packet.FromByteBuf(buf);
                    credentials.Deserialize(packet.GetData());
                    UserData* user = new UserData(credentials.username, credentials.password);
                    if (!dataHandler->IsUserRegistered(credentials.username))
                    {
                        dataHandler->RegisterUser(user);
                        sessionData->RegisterOwner(user);
                        std::cout << credentials.username << " has registered." << std::endl;
                        packet.Create(PAYLOAD_REGISTERED);
                        Write(packet.Serialize(), packet.GetTotalLength(), sessionData->GetFd());
                        sessionData->logged = true;
                    }
                    else
                    {
                        UserData* data = dataHandler->GetRegisteredUser(credentials.username);
                        if (data != nullptr)
                        {
                            if (data->GetPassword() == credentials.password)
                            {
                                packet.Create(PAYLOAD_LOGGED_IN);
                                std::cout << credentials.username << " has logged in"<< std::endl;
                                Write(packet.Serialize(), packet.GetTotalLength(), sessionData->GetFd());
                                sessionData->logged = true;
                                sessionData->RegisterOwner(user);
                                //TODO logged in
                            }
                            else
                            {
                                delete user;
                                //TODO bad credentials
                                packet.Create(PAYLOAD_INVALID_CREDENTIALS);
                                Write(packet.Serialize(), packet.GetTotalLength(), sessionData->GetFd());
                            }
                        }
                        else
                        {
                            delete user;
                        }
                   
                    }

                    break;
                }
                case PAYLOAD_MSG:
                {
                    if (sessionData->logged)
                    {
                        Packet packet;
                        packet.FromByteBuf(buf);
                        MessagePayload message;
                        message.Deserialize(packet.GetData());

                        if(dataHandler->IsUserRegistered(message.to))
                        {
                            std::cout << message.from << " whispers to " << message.to << ": " << message.message << std::endl;
                            ClientSessionData* destData;
                            if ((destData = dataHandler->GetUserSession(message.to)) != nullptr)
                            {
                                Write(packet.Serialize(), packet.GetTotalLength(), destData->GetFd());
                            }
                        }
                        else
                        {
                            packet.Create(PAYLOAD_INEXISTENT_DEST);
                            Write(packet.Serialize(), packet.GetTotalLength(), sessionData->GetFd());
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            std::cout << "Client crashed " << std::endl;
            shutdownReq = true;
        }
        memset(&buf, 0, BUF_SIZE);
    }   

}

void ClientHandler::ReceiveDeamon()
{

}

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}