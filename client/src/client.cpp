#pragma once
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <thread>
#include <iostream>
#include "../include/CSocket.hpp"
#include "../../common/ByteBufferReader.hpp"
#include "../../common/Packet.cpp"
#include "../../common/Payloads.cpp"
#define BUF_SIZE 512

void CloseService(int);
void ExitWrapper(int code);
void LoginRoutine();
void ReceiveDaemon();

bool logged = false;
bool shutDown = false;
CSocket* clientSocket;
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

	LoginRoutine();
	std::cout << "Asking for login..." << std::endl;

	while (!logged)
	{

	}

	while (true)
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
		Write(packet.Serialize(), packet.GetTotalLength(), clientSocket->GetFd());
	}
	
	return EXIT_SUCCESS;
}

void CloseService(int signal)
{
	Packet packet;
	packet.Create(PAYLOAD_DISCONNECT);
	Write(packet.Serialize(), packet.GetTotalLength(), clientSocket->GetFd());
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
					std::cout << "Oops, server went terribly wrong :(, closing..." << std::endl;
					ExitWrapper(0);
					//TODO server disconnected
					break;
				}
				case PAYLOAD_INVALID_CREDENTIALS:
				{
					std::cout << "Invalid credentials, try again" << std::endl;
					std::thread loginThread(&LoginRoutine);
					loginThread.detach();
					//TODO invalid credentials, re ask for login
					break;
				}
				case PAYLOAD_LOGGED_IN:
				{
					std::cout << "Login successful" << std::endl;
					logged = true;
					//TODO user has logged in
					break;
				}
				case PAYLOAD_REGISTERED:
				{
					std::cout << "User was not present in the database, registration successfull" << std::endl;
					logged = true;
					//TODO user has correctly been registered
					break;
				}
				case PAYLOAD_MSG:
				{
					Packet packet;
					packet.FromByteBuf(buf);
					MessagePayload message;
					message.Deserialize(packet.GetData());
					std::cout << message.from << " whispers to you: " << message.message << std::endl;
					//TODO user has received a message
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
	
	Packet *packet = new Packet(PAYLOAD_CREDENTIALS, credentials.Serialize(), credentials.size);

	int written = Write(packet->Serialize(), packet->GetTotalLength(), clientSocket->GetFd());

	delete packet;
}


void ExitWrapper(int code)
{
	shutDown = true;
	delete clientSocket;
	exit(code);
}
