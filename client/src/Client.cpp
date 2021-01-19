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
	shutDown.store(false);

	polledFds[STDIN_IDX].fd = 0;
	polledFds[STDIN_IDX].events = POLLIN;
	polledFds[TCP_IDX].fd = clientSocket->GetFd();
	polledFds[TCP_IDX].events = POLLIN;
	polledFds[UDP_IDX].fd = udpSocket->GetFd();
	polledFds[UDP_IDX].events = POLLIN;
}

Client::~Client()
{
    shutDown.store(true);
    Packet packet = Packet(PAYLOAD_DISCONNECT);
	Send(&packet, clientSocket->GetFd());
	packet.Purge();

	if(loopThread.joinable())
	{
		//std::cerr<<"Joining loop"<<std::endl;
		loopThread.join();
	}
    if(receiveDaemon.joinable())
    {
		//std::cerr<<"Joining receive"<<std::endl;
	    receiveDaemon.join();
    }
	if(loginThread.joinable())
	{
		//std::cerr<<"Joining login"<<std::endl;
		loginThread.join();
	}
	if(audioRecvThread.joinable())
	{
		//std::cerr<<"Joining audio"<<std::endl;
		audioRecvThread.join();
	}
    delete clientSocket;
    delete udpSocket;
}

void Client::LoginRoutine()
{
    std::string password;
	std::cout << "Username: ";
	std::flush(std::cout);
	bool u = false;
	bool p = false;
	username = PollCinOnce(polledFds, POLLED_SIZE, STDIN_IDX, POLL_DELAY, [&](){return shutDown.load();});

	std::cout << "Password: ";
	std::flush(std::cout);
	password = PollCinOnce(polledFds, POLLED_SIZE, STDIN_IDX, POLL_DELAY, [&](){return shutDown.load();});

	CredentialsPayload credentials = CredentialsPayload(username, password);
	
	Packet packet;
	char* temp = credentials.Serialize();
	packet.FromData(PAYLOAD_CREDENTIALS, temp, credentials.Size());
	Send(&packet, clientSocket->GetFd());
	packet.Purge();
	delete[] temp;
	return;
}

void Client::ReceiveAudio(AudioMessageHeaderPayload header)
{
    char buf[DGRAM_PACKET_SIZE +  sizeof(int)] = { 0 };
	int tot = 0;
	int packets = 0;
	char matrix[header.Segments()][DGRAM_PACKET_SIZE] = { 0 };
	int lengths[header.Segments()] = { 0 };
	int i = 1, received = 0;
	PollFdLoop(polledFds, POLLED_SIZE, UDP_IDX, POLL_DELAY, DGRAM_PACKET_SIZE + sizeof(int), [&](){return shutDown.load() || i > header.Segments();}, [&i, &received, &packets, &tot, &matrix, &lengths](char* buf, int bytesRead)
    {
        if (bytesRead > 0)
        {
            int index = ReadUInt(buf);
            tot += bytesRead;
            packets++;
            i++;
            lengths[index] = bytesRead - sizeof(int);
            memcpy(matrix[index], buf + sizeof(int), DGRAM_PACKET_SIZE);
        }
    });
	std::cout<<"Received audio: "<<header.ToString()<<std::endl;
    //std::cout << "Bytes: " << tot << ", packets: " << packets << std::endl;
	std::ofstream out(RECEIVED_FILE, std::ios::trunc | std::ios::out);
	for (int i = 0; i < header.Segments(); i++)
	{
		out.write(matrix[i], lengths[i]);
	}
	out.close();

	SoundPlayer *player = new SoundPlayer();
	player->PlaySound();
	delete player;
	return;
}

void Client::ReceiveDaemon()
{
	Packet packet;
	int bytesRead;
	PollFdLoop(polledFds, POLLED_SIZE, TCP_IDX, POLL_DELAY, BUF_SIZE,[&](){return shutDown.load();}, [&](char* buf, int bytesRead)
	{
		if (bytesRead > 0)
		{
			lastReceived = (uint8_t)buf[0];
			switch ((uint16_t)buf[0])
			{
				case PAYLOAD_DISCONNECT:
				{
					std::cout << "Oops, server went off :(" << std::endl;
					packet.Purge();
					receiveDaemon.detach();
					delete this;
					return;
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
					packet = Packet(buf);
					MessagePayload message = MessagePayload(packet.GetData());
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
					packet = Packet(buf);
					AudioMessageHeaderPayload vmhPayload = AudioMessageHeaderPayload(packet.GetData());
					//std::cout << vmhPayload.ToString() << std::endl;
					if(audioRecvThread.joinable())
					{
						audioRecvThread.join();
					}
					audioRecvThread = std::thread(&Client::ReceiveAudio, this, vmhPayload);
					//TODO Send ack
					packet.Purge();
					packet.CreateTrivial(PAYLOAD_ACK);
					Send(&packet, clientSocket->GetFd());
					packet.Purge();
					break;
				}
				case PAYLOAD_ACK:
				{
					//std::cout<<"ACK"<<std::endl;
					for(auto &ack : acks)
					{
						*ack = true;
					}
					break;
				}
				case PAYLOAD_DED_DGRAM_PORT:
				{
					//TODO create udp towards the correct port
					packet = Packet(buf);
					DgramPortPayload portPayload = DgramPortPayload(packet.GetData());
					//std::cout<<"The server udp is on "<<portPayload.ToString()<<std::endl;
					udpPort = portPayload.Port();
					//send the port to the server
					portPayload = DgramPortPayload(udpSocket->GetPort());
					char* temp = portPayload.Serialize();
					packet.FromData(PAYLOAD_DED_DGRAM_PORT, temp, portPayload.Size());
					Send(&packet, clientSocket->GetFd());
					delete[] temp;
					break;
				}
			}
		}
		packet.Purge();
	});
	return;
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
	AudioMessageHeaderPayload amhPayload = AudioMessageHeaderPayload(username, dest, segs, size);
	std::cout<<"Audio: "<<amhPayload.ToString()<<std::endl;
	char* temp = amhPayload.Serialize();
	packet.FromData(PAYLOAD_AUDIO_HEADER, temp, amhPayload.Size());
	Send(&packet, clientSocket->GetFd());
	delete[] temp;
	packet.Purge();

	char buf[BUF_SIZE] = { 0 };
	int bytesRead = 0;
	bool ack = false;
	acks.push_back(&ack);
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
	std::cout<<"Ack received"<<std::endl;
	usleep(25000);
	int packetsSent = 0;
	int totalSent = 0;
	int index = 0;
	std::cout<<"Sending audio..."<<std::endl;
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
	file.close();
	delete[] buffer;
}

void Client::Loop()
{
	if(!clientSocket->TryConnect())
    {
        std::cerr<<"Unble to connect to server"<<std::endl;
        return;
    }
    //Start the receive daemon thread
    receiveDaemon = std::thread(&Client::ReceiveDaemon, this);

    LoginRoutine();
    std::cout << "Asking for login..." << std::endl;

	while (!logged)
	{
		if(shutDown.load()) return;
	}

	Addressee:
	std::cout<<"Who do you want to chat with?"<<std::endl;
	std::flush(std::cout);
	std::string buf;
	buf = PollCinOnce(polledFds, POLLED_SIZE, STDIN_IDX, POLL_DELAY, [&](){return shutDown.load();});
	
	UserPayload dest = UserPayload(buf);
	Packet packet;
	char* temp = dest.Serialize();
	packet.FromData(PAYLOAD_USER, temp, dest.Size());
	delete[] temp;

	uint8_t status = PAYLOAD_ACK;
	bool ack = false;
	acks.push_back(&ack);
	Send(&packet, clientSocket->GetFd());
	packet.Purge();
	while(!ack)
	{
		if(shutDown.load()) return;
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

	// Can't use the poll routine cause i need the goto
	while(!shutDown.load())
	{
		int res = poll(polledFds, 3, 200);
		if(polledFds[0].revents & POLLIN)
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
				std::cout << "Press ENTER to stop the registration" << std::endl;
				std::cout << "Registering..." << std::endl;
				registrer->Register([]() -> bool { std::cin.ignore(); return true; });
				SendAudio(currentDest);
				delete registrer;
			}
			else
			{
				MessagePayload messagePayload = MessagePayload(username, currentDest, buf);
				char* temp = messagePayload.Serialize();
				packet.FromData(PAYLOAD_MSG, temp, messagePayload.Size());
				Send(&packet, clientSocket->GetFd());
				delete[] temp;
				packet.Purge();
			}
		}		
	}
	return;
}

void Client::Join()
{
	loopThread.join();
}

void Client::Run()
{
	loopThread = std::thread(&Client::Loop, this);
}