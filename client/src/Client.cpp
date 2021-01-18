#include "../include/Client.hpp"

Client::Client(char* servIp)
{
	clientSocket = new CSocket(servIp, 8080);
	this->serv_ip = servIp;
	//Horrible workaround :-)
	UDPSocket *sock = new UDPSocket(0);
    int port = sock->GetPort();
    delete sock;
    udpSocket = new UDPSocket(port);
}

Client::~Client()
{
    Packet packet;
	packet.CreateTrivial(PAYLOAD_DISCONNECT);
	Send(&packet, clientSocket->GetFd());

    shutDown = true;
    if(receiveDaemon.joinable())
    {
	    receiveDaemon.join();
    }
	if(loginThread.joinable())
	{
		loginThread.join();
	}
	if(audioRecvThread.joinable())
	{
		audioRecvThread.join();
	}
    delete clientSocket;
    delete udpSocket;
}

void Client::LoginRoutine()
{
    std::string password;
	std::cout << "Username: ";
	std::getline(std::cin, username);
	std::cout << "Password: ";
	std::getline(std::cin, password);

	CredentialsPayload credentials;
	credentials.Create(username, password);
	
	Packet packet;
	packet.FromData(PAYLOAD_CREDENTIALS, credentials.Serialize(), credentials.Size());

	Send(&packet, clientSocket->GetFd());
}

void Client::ReceiveAudio(AudioMessageHeaderPayload header)
{
    char buf[DGRAM_PACKET_SIZE +  sizeof(int)] = { 0 };
	int tot = 0;
	int packets = 0;
	char matrix[header.Segments()][DGRAM_PACKET_SIZE] = { 0 };
	int lengths[header.Segments()] = { 0 };
	int i = 1, received = 0;

	while (i <= header.Segments() && !shutDown)
	{
		received = read(udpSocket->GetFd(),buf, DGRAM_PACKET_SIZE + sizeof(int) );
		//std::cout << "Received seg: " << index << std::endl;
		if (received > 0)
		{
			int index = ReadUInt(buf);
			tot += received;
			packets++;
			i++;
			lengths[index] = received - sizeof(int);
			memcpy(matrix[index], buf + sizeof(int), DGRAM_PACKET_SIZE);
			memset(&buf, 0, DGRAM_PACKET_SIZE + sizeof(int));
		}	
	}
    std::cout << "Bytes: " << tot << ", packets: " << packets << std::endl;
	std::ofstream out(RECEIVED_FILE, std::ios::trunc | std::ios::out);
	for (int i = 0; i < header.Segments(); i++)
	{
		out.write(matrix[i], lengths[i]);
	}
	out.close();

	SoundPlayer *player = new SoundPlayer();
	player->PlaySound();
	delete player;
	std::cout<<"Receive thread ended"<<std::endl;
}

void Client::ReceiveDaemon()
{
    char buf[BUF_SIZE] = { 0 };
	int bytesRead;
	while (!shutDown)
	{
		bytesRead = Read(buf, BUF_SIZE, clientSocket->GetFd());
		if (bytesRead > 0)
		{
			lastReceived = (uint8_t)buf[0];
			switch ((uint16_t)buf[0])
			{
				case PAYLOAD_DISCONNECT:
				{
					std::cout << "Oops, server went off :(" << std::endl;
					delete this;
                    exit(EXIT_SUCCESS);
					break;
				}
				case PAYLOAD_INVALID_CREDENTIALS:
				{
					logged = false;
					std::cout << "Invalid credentials, try again" << std::endl;
					if (loginThread.joinable())
					{
						loginThread.join();
					}
					loginThread = std::thread(&Client::LoginRoutine, this);
					break;
				}
				case PAYLOAD_LOGGED_IN:
				{
					std::cout << "Login successful" << std::endl;
					logged = true;
					break;
				}
				case PAYLOAD_REGISTERED:
				{
					std::cout << "User was not present in the database, registration successfull" << std::endl;
					logged = true;
					break;
				}
				case PAYLOAD_MSG:
				{
					Packet packet;
					packet.FromByteBuf(buf);
					MessagePayload message;
					message.Deserialize(packet.GetData());
					std::cout << message.From() << " whispers to you: " << message.Message() << std::endl;
					break;
				}
				case PAYLOAD_INEXISTENT_DEST:
				{
					std::cout << "Are you sure the user you wrote to actually exists? Seems not to me" << std::endl;
					break;
				}
				case PAYLOAD_OFFLINE_USR:
				{
					std::cout << "The user you tried to contact is offline" << std::endl;
					break;
				}
				case PAYLOAD_AUDIO_HEADER:
				{
					//TODO prepare client for receiving packets
					Packet packet;
					packet.FromByteBuf(buf);
					AudioMessageHeaderPayload vmhPayload;
					vmhPayload.Deserialize(packet.GetData());
					std::cout << vmhPayload.ToString() << std::endl;

					audioRecvThread = std::thread(&Client::ReceiveAudio, this, vmhPayload);
					audioRecvThread.detach();

					//TODO Send ack
					/* packet.CreateTrivial(PAYLOAD_ACK);
					Send(&packet, clientSocket->GetFd()); */
					break;
				}
				case PAYLOAD_ACK:
				{
					std::cout<<"ACK"<<std::endl;
					for(auto &ack : acks)
					{
						*ack = true;
					}
					break;
				}
				case PAYLOAD_DED_DGRAM_PORT:
				{
					//TODO create udp towards the correct port
					Packet packet;
					packet.FromByteBuf(buf);
					DgramPortPayload portPayload;
					portPayload.Deserialize(packet.GetData());
					//std::cout<<"The server udp is on "<<portPayload.ToString()<<std::endl;
					udpPort = portPayload.Port();


					//send the port to the server
					portPayload.Create(udpSocket->GetPort());
					packet.FromData(PAYLOAD_DED_DGRAM_PORT, portPayload.Serialize(), portPayload.Size());
					Send(&packet, clientSocket->GetFd());

					break;
				}
			}
		}
	}
}

void Client::SendAudio(std::string dest)
{
    std::ifstream file(BUFFER_FILE, std::ifstream::in | std::ios::ate);
	if (file.fail())
	{
		std::cerr << "Unable to open recording file" << std::endl;
		return;
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	char* buffer = new char[DGRAM_PACKET_SIZE + sizeof(int)];
	int segs = ceil((float)size / DGRAM_PACKET_SIZE);

	//Handshake
	Packet packet;
	AudioMessageHeaderPayload amhPayload;
	amhPayload.Create(username, dest, segs, size);
	std::cout<<"Audio: "<<amhPayload.ToString()<<std::endl;
	packet.FromData(PAYLOAD_AUDIO_HEADER, amhPayload.Serialize(), amhPayload.Size());
	Send(&packet, clientSocket->GetFd());

	char buf[BUF_SIZE] = { 0 };
	int bytesRead = 0;
	bool ack = false;
	acks.push_back(&ack);
	std::cout << "Waiting for ack" << std::endl;
	bool stop = false;
	while (!ack)
	{
		if(lastReceived == PAYLOAD_INEXISTENT_DEST || lastReceived == PAYLOAD_OFFLINE_USR)
		{
			stop = true;
			break;
		} 
	}
	if(stop) 
	{
		std::cout<<"Invalid dest for audio"<<std::endl;
		return;
	}

	usleep(25000);
	int packetsSent = 0;
	int totalSent = 0;
	int index = 0;
	std::cout<<"Trasmitting data.."<<std::endl;
	struct sockaddr_in addr;
	addr.sin_port = htons(udpPort);
	addr.sin_addr.s_addr = inet_addr(serv_ip);
	addr.sin_family = AF_INET;
	while (!file.eof())
	{
		PutUInt(buffer, index);
		file.read(buffer + sizeof(int), DGRAM_PACKET_SIZE);
		std::streamsize bytesRead = file.gcount();
		int sent = sendto(udpSocket->GetFd(), buffer, bytesRead + sizeof(int), MSG_CONFIRM, (struct sockaddr*)&addr, sizeof(addr));
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
	std::cout << "Bytes: " << totalSent << " ,packets: " << packetsSent << std::endl;
	file.close();
	delete buffer;
}

int Client::Run()
{
    if(!clientSocket->TryConnect())
    {
        std::cerr<<"Unble to connect to server"<<std::endl;
        return EXIT_FAILURE;
    }
    //Start the receive daemon thread
    receiveDaemon = std::thread(&Client::ReceiveDaemon, this);

    LoginRoutine();
    std::cout << "Asking for login..." << std::endl;

	while (!logged)
	{
		;
	}

	Addressee:
		std::cout<<"Who do you want to chat with?"<<std::endl;

		std::string buf;

		std::getline(std::cin, buf);
		UserPayload dest;
		dest.Create(buf);
		Packet packet;
		packet.FromData(PAYLOAD_USER, dest.Serialize(), dest.Size());

		uint8_t status = PAYLOAD_ACK;
		bool ack = false;
		acks.push_back(&ack);
		Send(&packet, clientSocket->GetFd());
		while(!ack)
		{
			if(status == PAYLOAD_INEXISTENT_DEST || status == PAYLOAD_OFFLINE_USR)
			{
				acks.remove(&ack);
				goto Addressee;
			}
			usleep(10000);
			status = lastReceived;
		}
		acks.remove(&ack);

		currentDest = buf;
		std::cout<<std::endl<<
		"- Write and press ENTER to send the message.\n"
		"- To register an audio, type " << REGISTER_KEY << ", when you are done, press ENTER to send it.\n"
		"- To change addressee, type " << CHANGE_DEST_KEY << std::endl <<
		"- To quit type " << QUIT_KEY << std::endl<<std::endl;
		
		while(true)
		{
			std::getline(std::cin, buf);
			if(buf == "/c")
			{
				//System("clear");
				goto Addressee;
			}
			else if (buf == "/q")
			{
				break;
			}
			else if (buf == "/r")
			{
				SoundRegistrer *registrer = new SoundRegistrer();
				std::cout << "Press enter to stop the registration" << std::endl;
				std::cout << "Registering..." << std::endl;
				registrer->Register([]() -> bool { std::cin.ignore(); return true; });
				SendAudio(currentDest);
				delete registrer;
			}
			else
			{
				MessagePayload messagePayload;
				messagePayload.Create(username, currentDest, buf);
				packet.FromData(PAYLOAD_MSG, messagePayload.Serialize(), messagePayload.Size());
				Send(&packet, clientSocket->GetFd());
			}
		}
	
	return EXIT_SUCCESS;
}