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
		loopThread.join();
	}
    if(receiveDaemon.joinable())
    {
	    receiveDaemon.join();
    }
	if(loginThread.joinable())
	{
		//std::cerr<<"Joining login"<<std::endl;
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
	username = PollCinOnce(polledFds, POLLED_SIZE, STDIN_IDX, POLL_DELAY, [&](){return shutDown.load();});

	std::cout << "Password: ";
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
	AppendToConsole("Received audio: " + header.ToString(), false);
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

void Client::AppendToConsole(std::string line, bool inputRequest)
{
	if(inputRequest)
	{
		lastInWritten = line;
		std::flush(std::cout);
		std::cout<<line<<std::endl<<"> ";
	}
	else
	{
		std::flush(std::cout);
		std::cout<<std::endl<<line<<std::endl;
		if(lastInWritten != "")
		{
			std::cout<<lastInWritten<<std::endl<<"> ";
		}
	}
	std::flush(std::cout);
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
					AppendToConsole("Oops, server went off :(", false);
					packet.Purge();
					receiveDaemon.detach();
					delete this;
					return;
					break;
				}
				case PAYLOAD_INVALID_CREDENTIALS:
				{
					logged = false;
					AppendToConsole("Invalid credentials, try again", false);
					if (loginThread.joinable())
					{
						loginThread.join();
					}
					loginThread = std::thread(&Client::LoginRoutine, this);
					break;
				}
				case PAYLOAD_LOGGED_IN:
				{
					AppendToConsole("Login successful", false);
					logged = true;
					break;
				}
				case PAYLOAD_REGISTERED:
				{
					AppendToConsole("User was not present in the database, registration successfull", false);
					logged = true;
					break;
				}
				case PAYLOAD_MSG:
				{
					packet = Packet(buf);
					MessagePayload message = MessagePayload(packet.GetData());
					AppendToConsole( message.From() + " > " + message.Message(), false);
					break;
				}
				case PAYLOAD_INEXISTENT_DEST:
				{
					AppendToConsole( "The user does not exists :-/", false);
					break;
				}
				case PAYLOAD_OFFLINE_USR:
				{
					AppendToConsole("The user you tried to contact is offline", false);
					break;
				}
				case PAYLOAD_AUDIO_HEADER:
				{
					packet = Packet(buf);
					AudioMessageHeaderPayload vmhPayload = AudioMessageHeaderPayload(packet.GetData());
					if(audioRecvThread.joinable())
					{
						audioRecvThread.join();
					}
					audioRecvThread = std::thread(&Client::ReceiveAudio, this, vmhPayload);
					packet.Purge();
					break;
				}
				case PAYLOAD_ACK:
				{
					for(auto &ack : acks)
					{
						*ack = true;
					}
					break;
				}
				case PAYLOAD_DED_DGRAM_PORT:
				{
					packet = Packet(buf);
					DgramPortPayload portPayload = DgramPortPayload(packet.GetData());
					udpPort = portPayload.Port();
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
	AppendToConsole("Audio: " + amhPayload.ToString(), false);
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
		AppendToConsole("Invalid dest for audio", false);
		return;
	}
	usleep(25000);
	int packetsSent = 0;
	int totalSent = 0;
	int index = 0;
	AppendToConsole("Sending audio...", false);
	struct sockaddr_in addr;
	addr.sin_port = htons(udpPort);
	addr.sin_addr.s_addr = inet_addr(serv_ip);
	addr.sin_family = AF_INET;
	float progress = 0;
	while (!file.eof())
	{
		PutUInt(buffer, index);
		file.read(buffer + sizeof(int), DGRAM_PACKET_SIZE);
		std::streamsize bytesRead = file.gcount();
		int sent = sendto(udpSocket->GetFd(), buffer, bytesRead + sizeof(int), MSG_CONFIRM, (struct sockaddr*)&addr, sizeof(addr));
		if (sent == -1)
		{
			std::cout << strerror(errno);
			break;
		}
		int amount = (index * 100) / amhPayload.Segments();
		for(int i = 0; i < amount; i++)
		{
			std::cout<<"#";
		}
		std::cout<<">";
		std::cout<<"\r";
		index++;
		totalSent += sent;
		packetsSent++;
		usleep(250);
	}
	std::cout<<"\n\r";
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
    receiveDaemon = std::thread(&Client::ReceiveDaemon, this);

    LoginRoutine();

	while (!logged)
	{
		if(shutDown.load()) return;
	}

	Addressee:
	AppendToConsole("Who do you want to chat with?", true);
	std::flush(std::cout);
	std::string buf;
	buf = PollCinOnce(polledFds, POLLED_SIZE, STDIN_IDX, POLL_DELAY, [&](){return shutDown.load();});
	lastInWritten = "";

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
	AppendToConsole(
	"\n- Write and press ENTER to send the message.\n"
	"- To register an audio, type " + std::string(REGISTER_KEY) + ", when you are done, press ENTER to send it.\n"
	"- To change addressee, type " + CHANGE_DEST_KEY + "\n" +
	"- To quit type " + QUIT_KEY + "\n\n", false);

	// Can't use the poll routine cause i need the goto
	while(!shutDown.load())
	{
		int res = poll(polledFds, 3, 200);
		if(polledFds[0].revents & POLLIN)
		{
			std::getline(std::cin, buf);
			if(buf == "/c")
			{
				goto Addressee;
			}
			else if (buf == "/q")
			{
				break;
			}
			else if (buf == "/r")
			{
				SoundRegistrer *registrer = new SoundRegistrer();
				AppendToConsole("Press ENTER to stop the registration\nRegistering...", false);
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