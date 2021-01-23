#include "../include/Client.hpp"

Client::Client(char* servIp)
{
	//Create the TCP and UDP sockets
	clientSocket = new CSocket(servIp, 8080);
	this->serv_ip = servIp;

	//Horrible workaround :-)
	UDPSocket *sock = new UDPSocket(0);
    int port = sock->GetPort();
    delete sock;
    udpSocket = new UDPSocket(port);

	shutDown.store(false);

	//Setup the poll fds for the poll()
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
	try
	{
		if(IsConnected())
		{
			Packet packet = Packet(PAYLOAD_DISCONNECT);
			Send(&packet, clientSocket->GetFd());
			packet.Purge();
			connected = false;
		}
	}
	catch(const std::exception& e)
	{
	}
	
	//Join the joinable
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
		loginThread.join();
	}
	if(audioRecvThread.joinable())
	{
		audioRecvThread.join();
	}

	//Clear junk
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
	//Prepare the data structures and start receiving packets
    char buf[DGRAM_PACKET_SIZE +  sizeof(int)] = { 0 };
	int tot = 0;
	int packets = 0;
	char matrix[header.Segments()][DGRAM_PACKET_SIZE] = { 0 };
	int lengths[header.Segments()] = { 0 };
	int i = 1, rec = 0;
	int timeout = (header.Segments() << 2) / POLL_DELAY;

	//A huge lambda capture list :D
	PollFdLoop(polledFds, POLLED_SIZE, UDP_IDX, POLL_DELAY, [&](){ return shutDown.load() || i >= header.Segments() || rec >= timeout; }, [&buf, this, &i, &rec, &packets, &tot, &matrix, &lengths](bool pollin, int recycle)
    {
		rec = recycle;
		if(pollin)
		{
			int bytesRead = read(this->polledFds[UDP_IDX].fd, buf, DGRAM_PACKET_SIZE + sizeof(int));
			if(bytesRead < 0)
			{
				std::cout<<"Unable to read from UDP: "<<strerror(errno)<<std::endl;
			}
			if (bytesRead > 0)
			{
				//Read the index of the packet
				int index = ReadUInt(buf);

				tot += bytesRead;
				packets++;
				i++;

				//Update the structures
				lengths[index] = bytesRead - sizeof(int);
				memcpy(matrix[index], buf + sizeof(int), DGRAM_PACKET_SIZE);
			}
		}
    });
	packets++;
	if(packets < header.Segments())
	{
		AppendToConsole("Some packets may have been lost :(",false);
	}
	AppendToConsole("Original audio: " + header.ToString(), false);
	int lost = header.Segments() - packets;
	float percentage = (float) lost / header.Segments();
	AppendToConsole("Received: "+std::to_string(packets)+" segments, lost: "+std::to_string(lost)+"("+std::to_string(percentage)+"%)",false);
	std::ofstream out(RECEIVED_FILE, std::ios::trunc | std::ios::out);

	//Save the file to disk
	for (int i = 0; i < header.Segments(); i++)
	{
		out.write(matrix[i], lengths[i]);
	}
	out.close();

	//Play the received sound
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
		std::cout<<line<<std::endl;
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
	char buf[BUF_SIZE];
	PollFdLoop(polledFds, POLLED_SIZE, TCP_IDX, POLL_DELAY,[&](){return shutDown.load();}, [this, &buf, &packet](bool pollin, int recycle)
	{
		if(pollin)
		{
			int bytesRead = read(this->polledFds[TCP_IDX].fd, buf, BUF_SIZE);
		
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
						std::cout<<"Received audio: "<<vmhPayload.ToString()<<std::endl;
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
					case PAYLOAD_DUPLICATE_USER:
					{
						AppendToConsole("Wait a user with the same name is already online :O", false);
						break;
					}
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

	//Prepare the header of the audio
	char* buffer = new char[DGRAM_PACKET_SIZE + sizeof(int)];
	int segs = ceil((float)size / DGRAM_PACKET_SIZE);

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

	//Wait for server ack, if the user is offline or disappeared O.O, the server will notify us
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

	usleep(50000);
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
			std::cout << "Unable to send audio: "<<strerror(errno);
			break;
		}

		//Little graphic trick, not working so good tho 
		int amount = (index * 20) / amhPayload.Segments();
		for(int i = 0; i < amount; i++)
		{
			std::cout<<"#";
		}
		for(int i = amount; i < 20; i++)
		{
			std::cout<<"-";
		}
		std::cout<<">";
		std::cout<<"\r";
		index++;
		totalSent += sent;
		packetsSent++;
		usleep(DGRAM_SEND_DELAY);
	}
	std::cout<<std::endl;
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
	connected = true;
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
		//Pool the stdin
		int res = poll(polledFds, POLLED_SIZE, POLL_DELAY);
		if(polledFds[STDIN_IDX].revents & POLLIN)
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
				//Start the registration and send the audio
				SoundRegistrer *registrer = new SoundRegistrer();
				AppendToConsole("Press ENTER to stop the registration\nRegistering...", false);
				registrer->Register([]() -> bool { std::cin.ignore(); return true; });
				SendAudio(currentDest);
				delete registrer;
			}
			else
			{
				//Create and send a text message
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