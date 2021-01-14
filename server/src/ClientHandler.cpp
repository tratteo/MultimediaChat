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

    if (closedLocal)
    {
        std::cout << "Closed local" << std::endl;
        Packet packet;
        packet.CreateTrivial(PAYLOAD_DISCONNECT);
        try
        {
            Send(&packet, sessionData->GetFd());
        }
        catch (std::exception e)
        {

        }
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
                    closedLocal = false;
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
                        packet.CreateTrivial(PAYLOAD_REGISTERED);
                        Send(&packet, sessionData->GetFd());

                    }
                    else
                    {
                        UserData* data = dataHandler->GetRegisteredUser(credentials.Username());
                        if (data != nullptr)
                        {
                            if (data->GetPassword() == credentials.Password())
                            {
                                packet.CreateTrivial(PAYLOAD_LOGGED_IN);
                                std::cout << credentials.Username() << " has logged in"<< std::endl;
                                Send(&packet, sessionData->GetFd());
                                sessionData->UserLogged(data);
                                //TODO logged in
                            }
                            else
                            {
                                //TODO bad credentials
                                packet.CreateTrivial(PAYLOAD_INVALID_CREDENTIALS);
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
                                packet.CreateTrivial(PAYLOAD_OFFLINE_USR);
                                Send(&packet, sessionData->GetFd());
                            }
                        }
                        else
                        {
                            packet.CreateTrivial(PAYLOAD_INEXISTENT_DEST);
                            Send(&packet, sessionData->GetFd());
                        }
                    }
                    break;
                }
                case PAYLOAD_AUDIO_HEADER:
                {
                    std::cout<<"Received audio header"<<std::endl;
                    //TODO send ack and prepare to receive udp
                    //TODO prepare client for receiving packets
                    Packet packet;
                    packet.FromByteBuf(buf);
                    AudioMessageHeaderPayload vmhPayload;
                    vmhPayload.Deserialize(packet.GetData());
                    std::cout << vmhPayload.ToString() << std::endl;

                    if(dataHandler->IsUserRegistered(vmhPayload.To()))
                    {
                        ClientSessionData *destData = dataHandler->GetUserSession(vmhPayload.To());
                        if(destData != nullptr)
                        {
                            std::cout<<"Sending ack to client"<<std::endl;
                            packet.CreateTrivial(PAYLOAD_ACK);
                            bool res = Send(&packet, sessionData->GetFd());

                            std::thread audioRecvThread = std::thread(&ClientHandler::UDPReceive, this, vmhPayload);
                            audioRecvThread.detach();
                        }
                        else
                        {
                            packet.CreateTrivial(PAYLOAD_OFFLINE_USR);
                            Send(&packet, sessionData->GetFd());
                            break;
                        }
                    }
                    else
                    {
                        packet.CreateTrivial(PAYLOAD_INEXISTENT_DEST);
                        Send(&packet, sessionData->GetFd());
                        break;
                    }
                    break;
                }
                case PAYLOAD_ACK:
                {
                    // Notify all intereset acks
                    for(auto &ack : acks)
					{
						*ack = true;
					}
					acks.clear();
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

void ClientHandler::UDPReceive(AudioMessageHeaderPayload header)
{
    char buf[DGRAM_PACKET_SIZE + sizeof(int)] = { 0 };
    int tot = 0;
    int packets = 0;
    char matrix[header.Segments()][DGRAM_PACKET_SIZE] = { 0 };
    int lengths[header.Segments()] = { 0 };
    int i = 1, received = 0;
    while (i <= header.Segments())
    {
        std::cout<<"Receiving"<<std::endl;
        received = Read(buf, DGRAM_PACKET_SIZE + sizeof(int), sessionData->GetUDP(UDPSocket::IN)->GetFd());
        int index = ReadUInt(buf);
        std::cout << "Received seg: " << index << std::endl;
        if (received > 0)
        {
            tot += received;
            packets++;
            i++;
        }
        lengths[index] = received - sizeof(int);
        memcpy(matrix[index], buf + sizeof(int), DGRAM_PACKET_SIZE);
        memset(&buf, 0, DGRAM_PACKET_SIZE + sizeof(int));
    }
    std::cout << "TOT: " << tot << "Packts: " << packets << std::endl;
    std::ofstream out(RECEIVED_FILE, std::ios::trunc | std::ios::out);
    for (int i = 0; i < header.Segments(); i++)
    {
        out.write(matrix[i], lengths[i]);
    }
    out.close();

    Packet packet;
    ClientSessionData* destData = dataHandler->GetUserSession(header.To());
    if(destData != nullptr)
    {
        //TODO user is online, forward the message
        //TODO send header
        //wait for ack
        //start trasmission

        /*packet.FromData(PAYLOAD_AUDIO_HEADER, header.Serialize(), header.Size());
        Send(&packet, destData->GetFd());

        bool ack = false;
        acks.push_back(&ack);
        while(!ack) usleep(200000);

        char* buffer = new char[DGRAM_PACKET_SIZE + sizeof(int)];
        int packetsSent = 0;
        int totalSent = 0;
        int index = 0;
        std::cout<<"Trasmitting data.."<<std::endl;
        std::ifstream file(RECEIVED_FILE, std::ifstream::in);
        if (file.fail())
        {
            std::cerr << "Unable to open recording file" << std::endl;
            return;
        }
        while (!file.eof())
        {
            PutUInt(buffer, index);
            file.read(buffer + sizeof(int), DGRAM_PACKET_SIZE);
            std::streamsize bytesRead = file.gcount();
            int sent = WriteTo(buffer, bytesRead + sizeof(int), destData->GetUDP(UDPSocket::OUT)->GetFd(), destData->GetUDP(UDPSocket::OUT)->GetSockAddr());
            if (sent == -1)
            {
                std::cout << strerror(errno);
                exit(1);
            }
            index++;
            totalSent += sent;
            packetsSent++;
            usleep(100);
	    }
        std::cout << "TOT: " << totalSent << "Packets: " << packetsSent << std::endl;
        file.close();*/
    }
}

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}