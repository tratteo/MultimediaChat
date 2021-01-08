#pragma once
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include "../include/CSocket.hpp"
#include "../../common/ByteBufferReader.hpp"
#include "../../common/Packet.cpp"
#include "../../common/Payloads.cpp"
#define BUF_SIZE 512

void CloseService(int);
void ExitWrapper(int code);
void LoginRoutine();

bool shutDown = false;
CSocket* clientSocket;

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
	if(clientSocket->IsConnected())
	{
		std::cout << "1. Login\n0. Quit" << std::endl;
		int choice = 0;
		std::cin >> choice;
		switch (choice)
		{
			case 0:
				ExitWrapper(EXIT_SUCCESS);
			case 1:
				LoginRoutine();
				break;
			default:
				break;
		}

		std::cout<<"Connected"<<std::endl;
		char buf[BUF_SIZE];
		while(true)
		{
			fgets(buf, BUF_SIZE, stdin);
		}
	}
	return EXIT_SUCCESS;
}

void CloseService(int signal)
{
	Packet* packet = new Packet(PAYLOAD_DISCONNECT, nullptr, 0);
	Write(packet->Serialize(), packet->GetTotalLength(), clientSocket->GetFd());
	delete packet;
	ExitWrapper(EXIT_SUCCESS);
}



void LoginRoutine()
{
	std::string username;
	std::string password;

	std::cout << "Username: ";
	std::cin >> username;

	std::cout << "Password: ";
	std::cin >> password;

	CredentialsPayload credentials;
	credentials.Create(username, password);

	
	Packet *packet = new Packet(PAYLOAD_CREDENTIALS, credentials.Serialize(), credentials.size);

	int written = Write(packet->Serialize(), packet->GetTotalLength(), clientSocket->GetFd());
	std::cout << written;
}


void ExitWrapper(int code)
{
	delete clientSocket;
	exit(code);
}
