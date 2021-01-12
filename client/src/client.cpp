#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include "../include/CSocket.hpp"
#include "../../common/NetworkHandler.hpp"
#include "../../common/Packet.hpp"
#include "../../common/Payloads.hpp"
#define BUF_SIZE 512

void CloseService(int);
void ExitWrapper(int code);
void LoginRoutine();
void ReceiveDaemon();

bool logged = false;
bool shutDown = false;
CSocket* clientSocket;
std::thread loginThread;
std::string username;

int main(int argv, char** argc)
{
	if (argv < 2)
	{
		handle_fatal_error("No arguments found");
	}

	signal(SIGINT, CloseService);
	signal(SIGKILL, CloseService);
	signal(SIGTERM, CloseService);

	std::cout << "Multimedia Chat Client" << std::endl;

	clientSocket = new CSocket(argc[1]);
	clientSocket->Init(SOCK_STREAM, 0);

	clientSocket->TryConnect();
	if (!clientSocket->IsConnected()) exit(EXIT_FAILURE);

	std::thread daemon(&ReceiveDaemon);
	daemon.detach();

	loginThread = std::thread(&LoginRoutine);
	loginThread.join();

	std::cout << "Asking for login..." << std::endl;

	while (!logged)
	{

	}

	while (true)
	{
		std::cout << "1. Write message" << std::endl << "0. Bye bye" << std::endl;
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
				packet.FromData(PAYLOAD_MSG, payload.Serialize(), payload.size);
				Send(&packet, clientSocket->GetFd());
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
	packet.Create(PAYLOAD_DISCONNECT);
	Send(&packet, clientSocket->GetFd());
	ExitWrapper(EXIT_SUCCESS);
}

void ReceiveDaemon()
{
	char buf[BUF_SIZE] = { 0 };
	int bytesRead;
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
					std::cout << message.from << " whispers to you: " << message.message << std::endl;
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
			}
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
	packet.FromData(PAYLOAD_CREDENTIALS, credentials.Serialize(), credentials.size);

	Send(&packet, clientSocket->GetFd());
}

void ExitWrapper(int code)
{
	shutDown = true;
	delete clientSocket;
	exit(code);
}
