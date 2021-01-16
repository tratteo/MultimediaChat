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

void ClientHandler::UserLogged()
{
    std::thread([&]()
    {
        usleep(100000);
        Packet packet;
        DgramPortPayload portPayload;
        portPayload.Create(sessionData->GetUdp()->GetPort());
        packet.FromData(PAYLOAD_DED_DGRAM_PORT, portPayload.Serialize(), portPayload.Size());
        Send(&packet, sessionData->GetFd());    
    }).detach();
    //send the random port to the client
    
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
                        std::cout << credentials.Username() << " has registered"<< std::endl;
                        packet.CreateTrivial(PAYLOAD_REGISTERED);
                        Send(&packet, sessionData->GetFd());
                        UserLogged();
                        //TODO send udp stats
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
                                //TODO send udp stats
                                UserLogged();
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
                    //std::cout<<"Received audio header"<<std::endl;
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
                            std::thread audioRecvThread = std::thread(&ClientHandler::UDPReceive, this, vmhPayload);
                            audioRecvThread.detach();
                            //std::cout<<"Sending ack to client"<<std::endl;
                            packet.CreateTrivial(PAYLOAD_ACK);
                            bool res = Send(&packet, sessionData->GetFd());

      
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
                case PAYLOAD_DED_DGRAM_PORT:
                {
                    Packet packet;
                    packet.FromByteBuf(buf);
                    DgramPortPayload portPayload;
                    portPayload.Deserialize(packet.GetData());
                    std::cout<<"The client udp is on "<<portPayload.ToString()<<std::endl;
                    sessionData->udpPort = portPayload.Port();
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
        //received = Read(buf, DGRAM_PACKET_SIZE + sizeof(int), sessionData->GetUdp()->GetFd());
        received = read(sessionData->GetUdp()->GetFd(), buf, DGRAM_PACKET_SIZE + sizeof(int));
        int index = ReadUInt(buf);
        //std::cout << "Received seg: " << index << std::endl;
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
    std::cout << "Bytes: " << tot << ", packets: " << packets << std::endl;
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

        packet.FromData(PAYLOAD_AUDIO_HEADER, header.Serialize(), header.Size());
        Send(&packet, destData->GetFd());
/* 
        bool ack = false;
        acks.push_back(&ack);
        while(!ack) usleep(200000); */
        usleep(25000);
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
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(destData->udpPort);
        addr.sin_addr.s_addr = inet_addr(destData->GetIp());
        //std::cout<<"Sending to port: "<<addr.sin_port<<std::endl;
        while (!file.eof())
        {
            PutUInt(buffer, index);
            file.read(buffer + sizeof(int), DGRAM_PACKET_SIZE);
            std::streamsize bytesRead = file.gcount();
            int sent = sendto(destData->GetUdp()->GetFd(), buffer, bytesRead + sizeof(int), MSG_CONFIRM, (struct sockaddr*)&addr, sizeof(addr));
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
        std::cout << "Bytes: " << totalSent << ", packets: " << packetsSent << std::endl;
        file.close();
    }
}

void ClientHandler::HandleConnection()
{
    clientThread = std::thread(&ClientHandler::Loop, this);
    clientThread.detach();
}