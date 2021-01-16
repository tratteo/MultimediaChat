#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "errhandler.hpp"

class UDPSocket
{

	public:

	UDPSocket(char* ip, int port);
	~UDPSocket();
	inline int GetPort() { return port; }
	inline char* GetIp() { return ip; }
	inline int GetFd() { return fd; }
	struct sockaddr_in GetSockAddr() ;
	private:
	struct sockaddr_in servAddr;
	int port;
	char* ip;
	int fd;
};