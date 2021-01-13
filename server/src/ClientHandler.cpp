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
        Send(&packet, sessionData->GetFd());
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
                    if (!dataHandler->IsUserRegistered(credentials.Username()))
                    {
                        UserData* user = new UserData(credentials.Username(), credentials.Password());
                        dataHandler->RegisterUser(user);
                        sessionData->UserLogged(user);
                        std::cout << user->GetUsername() << " has registered." << std::endl;
                        packet.Create(PAYLOAD_REGISTERED);
                        Send(&packet, sessionData->GetFd());

                    }
                    else
                    {
                        UserData* data = dataHandler->GetRegisteredUser(credentials.Username());
                        if (data != nullptr)
                        {
                            if (data->GetPassword() == credentials.Password())
                            {
                                packet.Create(PAYLOAD_LOGGED_IN);
                                std::cout << credentials.Username() << " has logged in"<< std::endl;
                                Send(&packet, sessionData->GetFd());
                                sessionData->UserLogged(data);
                                //TODO logged in
                            }
                            else
                            {
                                //TODO bad credentials
                                packet.Create(PAYLOAD_INVALID_CREDENTIALS);
                                Send(&packet, sessionData->GetFd());
                            }
                        }
                    }

                    break;
                }
                case PAYLOAD_MSG:
                {
                    if (sessionData->IsLogged())
                    {
                        Packet packet;
                        packet.FromByteBuf(buf);
                        MessagePayload message;
                        message.Deserialize(packet.GetData());

                        if(dataHandler->IsUserRegistered(message.To()))
                        {
                            ClientSessionData* destData;
                            if ((destData = dataHandler->GetUserSession(message.To())) != nullptr)
                            {
                                Send(&packet, destData->GetFd());
                                std::cout << message.From() << " whispers to " << message.To() << ": " << message.Message() << std::endl;
                                dataHandler->AddMessage(message);
                            }
                            else
                            {
                                packet.Create(PAYLOAD_OFFLINE_USR);
                                Send(&packet, sessionData->GetFd());
                            }
                        }
                        else
                        {
                            packet.Create(PAYLOAD_INEXISTENT_DEST);
                            Send(&packet, sessionData->GetFd());
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

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}