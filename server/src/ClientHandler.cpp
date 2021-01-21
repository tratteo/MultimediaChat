#include "../include/ClientHandler.hpp"

ClientHandler::ClientHandler(ClientSessionData *sessionData, DataBaseHandler *dataHandler, void (*OnDisconnect)(ClientHandler*))
{
    this->sessionData = sessionData;
    this->dataHandler = dataHandler;
    this->OnDisconnect = OnDisconnect;
    shutdownReq.store(false);

    polledFds[TCP_IDX].fd = sessionData->GetFd();
	polledFds[TCP_IDX].events = POLLIN;
	polledFds[UDP_IDX].fd = sessionData->GetUdp()->GetFd();
	polledFds[UDP_IDX].events = POLLIN;
}

ClientHandler::~ClientHandler()
{
    shutdownReq.store(true);

    if(mainThread.joinable())
    {
        //std::cout<<"Joining main"<<std::endl;
        mainThread.join();
    }
    if(udpThread.joinable())
    {
        //std::cout<<"Joining udp"<<std::endl;
        udpThread.join();
    }

    if(active)
    {
        CloseConnection();
    }

    if(sessionData != nullptr)
    {    
        delete sessionData;
        sessionData = nullptr;
    }
}

void ClientHandler::CloseConnection()
{
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
        packet.Purge();
    }

    if (sessionData->GetOwner() != nullptr)
    {
        std::cout << "User " << sessionData->GetOwner()->GetUsername() << " disconnected" << std::endl;
    }
    else
    {
        std::cout << "Connection " << sessionData->GetIp() << " disconnected without logging in" << std::endl;
    }

    dataHandler->UserDisconnected(sessionData);
    //OnDisconnect(this);
    active = false;
}

void ClientHandler::NotifyUDPPort()
{
    usleep(100000);
    Packet packet;
    DgramPortPayload portPayload = DgramPortPayload(sessionData->GetUdp()->GetPort());
    char* temp = portPayload.Serialize();
    packet.FromData(PAYLOAD_DED_DGRAM_PORT, temp, portPayload.Size());
    Send(&packet, sessionData->GetFd());   
    packet.Purge(); 
    delete[] temp;
}

void ClientHandler::Loop()
{
    Packet packet;
    char buf[BUF_SIZE];

    PollFdLoop(polledFds, POLLED_SIZE, TCP_IDX, POLL_DELAY, [&](){return shutdownReq.load();}, [this, &buf, &packet](bool pollin, int recycle)
    {
        if(pollin)
        {
            int bytesRead = read(polledFds[TCP_IDX].fd, buf, BUF_SIZE);
            if (bytesRead > 0)
            {
                switch ((uint16_t)buf[0])
                {
                    case PAYLOAD_DISCONNECT:
                    {
                        closedLocal = false;
                        shutdownReq.store(true);
                        active = false;
                        return;
                    }
                    case PAYLOAD_CREDENTIALS:
                    {
                        usleep(25000);
                        packet = Packet(buf);
        
                        CredentialsPayload credentials = CredentialsPayload(packet.GetData());
                        if (!dataHandler->IsUserRegistered(credentials.Username()))
                        {
                            UserData* user = new UserData(credentials.Username(), credentials.Password());
                            dataHandler->RegisterUser(user);
                            sessionData->UserLogged(user);
                            std::cout << credentials.Username() << " has registered"<< std::endl;
                            packet.CreateTrivial(PAYLOAD_REGISTERED);
                            Send(&packet, sessionData->GetFd());
                            NotifyUDPPort();
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
                                    NotifyUDPPort();
                                }
                                else
                                {
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
                            packet = Packet(buf);
                            MessagePayload message = MessagePayload(packet.GetData());

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
                        packet = Packet(buf);
                        AudioMessageHeaderPayload vmhPayload = AudioMessageHeaderPayload(packet.GetData());

                        //std::cout << vmhPayload.ToString() << std::endl;

                        if(dataHandler->IsUserRegistered(vmhPayload.To()))
                        {
                            ClientSessionData *destData = dataHandler->GetUserSession(vmhPayload.To());
                            if(destData != nullptr)
                            {                
                                udpThread = std::thread(&ClientHandler::UDPReceive, this, vmhPayload);
                                udpThread.detach();
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
                    case PAYLOAD_DED_DGRAM_PORT:
                    {
                        packet = Packet(buf);
                        DgramPortPayload portPayload = DgramPortPayload(packet.GetData());
                        //std::cout<<"The client udp is on "<<portPayload.ToString()<<std::endl;
                        sessionData->udpPort = portPayload.Port();
                        break;
                    }
                    case PAYLOAD_USER:
                    {
                        packet = Packet(buf);
                        UserPayload user = UserPayload(packet.GetData());
                        if(dataHandler->IsUserRegistered(user.Username()))
                        {
                            if(dataHandler->GetUserSession(user.Username()) != nullptr)
                            {
                                packet.CreateTrivial(PAYLOAD_ACK);
                                Send(&packet, sessionData->GetFd());
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
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        packet.Purge();
    });
    std::flush(std::cout);
    CloseConnection();
    return;
}

void ClientHandler::UDPReceive(AudioMessageHeaderPayload header)
{
    int tot = 0;
    int packets = 0;
    char buf[DGRAM_PACKET_SIZE + sizeof(int)];
    char matrix[header.Segments()][DGRAM_PACKET_SIZE] = { 0 };
    int lengths[header.Segments()] = { 0 };
    int i = 1, rec = 0;
	PollFdLoop(polledFds, POLLED_SIZE, UDP_IDX, POLL_DELAY, [&](){ return shutdownReq.load() || i >= header.Segments() || rec >= (header.Segments() << 2); }, [this, &buf, &i, &rec, &packets, &tot, &matrix, &lengths](bool pollin, int recycle)
    {
        if(pollin)
        {
            int bytesRead = read(polledFds[UDP_IDX].fd, buf, DGRAM_PACKET_SIZE + sizeof(int));
            if(bytesRead < 0)
            {
                std::cout<<"Unable to read from UDP: "<<strerror(errno)<<std::endl;
            }
            if (bytesRead > 0)
            {
                int index = ReadUInt(buf);
                tot += bytesRead;
                packets++;
                i++;
                lengths[index] = bytesRead - sizeof(int);
                memcpy(matrix[index], buf + sizeof(int), DGRAM_PACKET_SIZE);
                std::cout<<i<<std::endl;
            }
        }
		
    });
    packets++;
    if(packets < header.Segments())
	{
		std::cout<<"Some packets may have been lost :("<<std::endl;
	}
	std::cout<<"Original audio: " + header.ToString()<<std::endl;
	int lost = header.Segments() - packets ;
	float percentage = (float) lost / header.Segments();
	std::cout<<"Received: "<<packets<<" segments, lost: "<<lost<<" ("<<percentage<<"%)"<<std::endl;
    std::ofstream out(RECEIVED_FILE, std::ios::trunc | std::ios::out);
    for (int i = 0; i < header.Segments(); i++)
    {
        out.write(matrix[i], lengths[i]);
    }
    out.close();

    Packet packet;
    ClientSessionData* destData = dataHandler->GetUserSession(header.To());
    if(destData != nullptr && !shutdownReq.load())
    {
        char* temp = header.Serialize();
        packet.FromData(PAYLOAD_AUDIO_HEADER, temp, header.Size());
        Send(&packet, destData->GetFd());
        delete[] temp;
        packet.Purge();

        usleep(500000);
        char* buffer = new char[DGRAM_PACKET_SIZE + sizeof(int)];
        int packetsSent = 0;
        int totalSent = 0;
        int index = 0;
        std::cout<<"Forwarding audio to "<<header.To()<<"..."<<std::endl;
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
        while (!file.eof())
        {
            if(shutdownReq.load())
            {
                break;
            }
            PutUInt(buffer, index);
            file.read(buffer + sizeof(int), DGRAM_PACKET_SIZE);
            std::streamsize bytesRead = file.gcount();
            int sent = sendto(destData->GetUdp()->GetFd(), buffer, bytesRead + sizeof(int), MSG_CONFIRM, (struct sockaddr*)&addr, sizeof(addr));
            if (sent == -1)
            {
                std::cout << strerror(errno);
                exit(1);
            }
            int amount = (index * 100) / header.Segments();
            for(int i = 0; i < amount; i++)
            {
                std::cout<<"#";
            }
            std::cout<<">";
            std::cout<<"\r";
            index++;
            totalSent += sent;
            packetsSent++;
            usleep(DGRAM_SEND_DELAY);
	    }
        std::cout<<"\n\r";
        std::cout << "Bytes: " << totalSent << ", packets: " << packetsSent << std::endl;
        file.close();
        delete[] buffer;
    }
}

void ClientHandler::HandleConnection()
{
    mainThread = std::thread(&ClientHandler::Loop, this);
    active = true;
}