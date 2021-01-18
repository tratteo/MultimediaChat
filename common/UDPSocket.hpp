#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include "errhandler.hpp"

class UDPSocket
{

	public:

	UDPSocket(int port);
	~UDPSocket();
	inline int GetPort() { return port; }
	inline int GetFd() { return fd; }
	private:
	struct sockaddr_in servAddr;
	int port;
	int fd;
};