#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include <cmath>
#include "../../common/CSocket.hpp"
#include "../../common/SoundRegistrer.hpp"
#include "../../common/NetworkHandler.hpp"
#include "../../common/Packet.hpp"
#include "../../common/Payloads.hpp"
#include "../../common/ByteBufferUtils.hpp"
#include "../../common/UDPSocket.hpp"
#define BUF_SIZE 512

void CloseService(int);
void UDPReceive(AudioMessageHeaderPayload header);
void ExitWrapper(int code);
void LoginRoutine();
void ReceiveDaemon();
void SendAudio(std::string dest);
int udpPort;
bool logged = false;
bool shutDown = false;
CSocket* clientSocket;
UDPSocket* inUdpSock;
SoundPlayer *player;

std::thread receiveDaemon;
std::thread audioRecvThread;
std::thread loginThread;
std::string username;
std::list<bool*> acks;
uint8_t lastReceived;
char* serv_ip;

int main(int argv, char** argc)
{
	if (argv < 2)
	{
		handle_fatal_error("No arguments found");
	}

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

	serv_ip = argc[1];

	std::cout << "Multimedia Chat Client" << std::endl;

	player = new SoundPlayer();
	clientSocket = new CSocket(serv_ip, 8080);

	//Horrible workaround :-)
	UDPSocket *sock = new UDPSocket(serv_ip, 0);
    int port = sock->GetPort();
    delete sock;
    inUdpSock = new UDPSocket(serv_ip, port);


	if(!clientSocket->TryConnect()) exit(EXIT_FAILURE);

	receiveDaemon = std::thread(&ReceiveDaemon);
/* 	receiveDaemon.detach(); */

	LoginRoutine();

	std::cout << "Asking for login..." << std::endl;

	while (!logged)
	{
		;
	}
	while (true)
	{
		std::cout << "1. Write message\n2. Register\n0. Bye bye" << std::endl;
		std::string buf;
		int choice = 0;
		std::getline(std::cin, buf);
		try
		{
			choice = std::stoi(buf);
		}
		catch (std::exception e)
		{
			std::cout << "Numbers please" << std::endl;
			continue;
		}

		switch (choice)
		{
			case 1:
			{
				std::string line;
				MessagePayload payload;

				std::cout << "Addressee: ";
				std::getline(std::cin, line);
				std::string dest = line;

				std::cout << "Message: ";
				std::getline(std::cin, line);
				std::string message = line;

				payload.Create(username, dest, message);
				//std::cout << "F: " << payload.from << ",l: "<<payload.fromLen<< ", T: " << payload.to <<", l: "<<payload.toLen<< ", M: " << ", l: "<<payload.messageLen<< payload.message << std::endl;
				Packet packet;
				packet.FromData(PAYLOAD_MSG, payload.Serialize(), payload.Size());
				Send(&packet, clientSocket->GetFd());
				break;
			}
			case 2:
			{

				std::string line;
				std::cout << "Addressee: ";
				std::getline(std::cin, line);
				std::string dest = line;
				SoundRegistrer *registrer = new SoundRegistrer();
				std::cout << "Press enter to stop the registration" << std::endl;
				std::cout << "Registering..." << std::endl;
				registrer->Register([]() -> bool { std::cin.ignore(); return true; });
				SendAudio(dest);
				//TODO send data over udp
				delete registrer;
				break;
			}
			case 0:
			{
				CloseService(0);
				ExitWrapper(EXIT_SUCCESS);
			}
		}
	}
	
	return EXIT_SUCCESS;
}

void CloseService(int signal)
{
	Packet packet;
	packet.CreateTrivial(PAYLOAD_DISCONNECT);
	Send(&packet, clientSocket->GetFd());
	ExitWrapper(EXIT_SUCCESS);
}

void ReceiveDaemon()
{
	char buf[BUF_SIZE] = { 0 };
	int bytesRead;
	int flags = fcntl(clientSocket->GetFd(), F_GETFL, 0);
	fcntl(clientSocket->GetFd(), F_SETFL, flags | O_NONBLOCK);
	while (!shutDown)
	{
		bytesRead = Read(buf, BUF_SIZE, clientSocket->GetFd());
		if (bytesRead > 0)
		{
			switch ((uint16_t)buf[0])
			{
				case PAYLOAD_DISCONNECT:
				{
					std::cout << "Oops, server went terribly wrong :(" << std::endl;
					ExitWrapper(0);
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
					loginThread = std::thread(&LoginRoutine);
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

					audioRecvThread = std::thread(&UDPReceive, vmhPayload);
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
					acks.clear();
					break;
				}
				case PAYLOAD_DED_DGRAM_PORT:
				{
					//TODO create udp towards the correct port
					Packet packet;
					packet.FromByteBuf(buf);
					DgramPortPayload portPayload;
					portPayload.Deserialize(packet.GetData());
					std::cout<<"The server udp is on "<<portPayload.ToString()<<std::endl;
					udpPort = portPayload.Port();


					//send the port to the server
					portPayload.Create(inUdpSock->GetPort());
					packet.FromData(PAYLOAD_DED_DGRAM_PORT, portPayload.Serialize(), portPayload.Size());
					Send(&packet, clientSocket->GetFd());

					break;
				}
			}
			lastReceived = (uint8_t)buf[0];
		}
	}
}

void LoginRoutine()
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

void UDPReceive(AudioMessageHeaderPayload header)
{
	char buf[DGRAM_PACKET_SIZE +  sizeof(int)] = { 0 };
	int tot = 0;
	int packets = 0;
	char matrix[header.Segments()][DGRAM_PACKET_SIZE] = { 0 };
	int lengths[header.Segments()] = { 0 };
	int i = 1, received = 0;
	//std::cout<<"Receiving: "<<std::endl;
	while (i <= header.Segments())
	{
		received = read(inUdpSock->GetFd(),buf, DGRAM_PACKET_SIZE + sizeof(int) );
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

	player->PlaySound();
	std::cout<<"Receive thread ended"<<std::endl;
}

void ExitWrapper(int code)
{
	shutDown = true;
	receiveDaemon.join();
	if(loginThread.joinable())
	{
		loginThread.join();
	}
	if(audioRecvThread.joinable())
	{
		audioRecvThread.join();
	}
	delete player;
	delete clientSocket;
	delete inUdpSock;

	exit(code);
}

void SendAudio(std::string dest)
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
		//std::cout << "Sending seg: " << index << std::endl;

		//Packet packet;
		//packet.FromData(DGRAM_AUDIO_PACKET, buffer, DGRAM_PACKET_SIZE + sizeof(int));

		//int sent = WriteTo(buffer, bytesRead + sizeof(int), inUdpSock->GetFd(), &addr);
		int sent = sendto(inUdpSock->GetFd(), buffer, bytesRead+ sizeof(int), MSG_CONFIRM, (struct sockaddr*)&addr, sizeof(addr));
		//int sent = Write(buffer, bytesRead + sizeof(int), udpSocket);
		//int sent = sendto(sock, buffer, s, 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
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
	file.close();
	delete buffer;
}
