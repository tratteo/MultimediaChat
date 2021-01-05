#pragma once
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "errhandler.hpp"
#define PORT 8080

class CSocket
{
	private:

	bool connected;
	char* server_ip;
	int socket_fd;
	struct sockaddr_in serv_addr;

	~CSocket();

	public:

	CSocket(char* server_ip);
	void init(int type, int protocol);
	void try_connect();
	bool is_connected();
	
};

