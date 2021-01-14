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

	enum Type
	{
		IN,
		OUT
	};

	UDPSocket(char* ip, int port, Type type);
	~UDPSocket();
	inline Type GetType() { return type; }
	inline int GetPort() { return port; }
	inline char* GetIp() { return ip; }
	inline int GetFd() { return fd; }
	inline struct sockaddr_in* GetSockAddr() { return &servAddr; }
	private:
	struct sockaddr_in servAddr;
	int port;
	char* ip;
	int fd;
	Type type;
};