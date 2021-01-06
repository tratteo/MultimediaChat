#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include "../include/CSocket.hpp"
#define BUF_SIZE 512

void CloseService(int);
void ExitWrapper(int code);
int Read(char* bf);
int Write(char* buf);
void LoginRoutine();
void RegisterRoutine();

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

		while(true)
		{
			std:: cout << "1. Login\n2. Register\n0. Quit" << std::endl;
			int choice = 0;
			std::cin>>choice;
			switch (choice)
			{
				case 0:
					ExitWrapper(EXIT_SUCCESS);
				case 1:
					LoginRoutine();
					break;
				case 2: 
					RegisterRoutine();
					break;
				default:
					break;
			}
		}

		LoginRoutine();
		std::cout<<"Connected"<<std::endl;
		char buf[BUF_SIZE];
		while(true)
		{
			fgets(buf, BUF_SIZE, stdin);
			write(clientSocket->GetFd(), buf, strlen(buf));
		}
	}
	return EXIT_SUCCESS;
}

void CloseService(int signal)
{
	char* closeMsg = (char*)"DC\n";
	write(clientSocket->GetFd(), closeMsg, strlen(closeMsg));
	ExitWrapper(EXIT_SUCCESS);
}

void LoginRoutine()
{
	char buf[BUF_SIZE] = {0};

	int bytesRead = Read(buf);
	std::cout << buf;
	memset(&buf, 0, sizeof(buf));

	fgets(buf, BUF_SIZE, stdin);
	Write(buf);
	memset(&buf, 0, sizeof(buf));

	bytesRead = Read(buf);
	std::cout << buf;
	memset(&buf, 0, sizeof(buf));

	fgets(buf, BUF_SIZE, stdin);
	Write(buf);
	memset(&buf, 0, sizeof(buf));
}

void RegisterRoutine()
{

}

int Write(char* buf)
{
	return write(clientSocket->GetFd(), buf, strlen(buf));
}

int Read(char* buf)
{
	int bytesRead = read(clientSocket->GetFd(), buf, BUF_SIZE);
	if(bytesRead == -1)
	{
		handle_error("Unable to read");
		return -1;
	}
	return bytesRead;
}

void ExitWrapper(int code)
{
	free(clientSocket);
	exit(code);
}
